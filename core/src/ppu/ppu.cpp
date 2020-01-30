#include "ppu.h"
#include "io_regs.h"
#include "logger.h"

#define VRAM_BASE         0x8000
#define OAM_BASE 0xFE00
#define OAM_TOP 0xFEA0

#define OFFSET_9000 0x1000
#define OFFSET_9800 0x1800
#define OFFSET_9C00 0x1C00

#define TO_PALLETTE(_pix, _pal) ((_pal >> ((_pix & 0x3) * 2)) & 0x3)

#define TILE_SIZE 16

#define OAM_CYCLES 80
#define DATA_XFER_CYCLES 168
#define HBLANK_CYCLES 208
#define LINE_CYCLES 456

Ppu::Ppu(InterruptController *ic) {
    mIC = ic;
    mMode = IOREG_STAT_MODE_2_OAM_SEARCH;
    mRemCycles = OAM_CYCLES;
    mEnabled = false;

    mLCDC = 0;
    mSCY = 0;
    mSCX = 0;
    mLY = 0;
    mLYC = 0;
    mBGP = 0;
    mOBP0 = 0;
    mOBP1 = 0;
    mWY = 0;
    mWX = 0;
    mTileDataOffset = OFFSET_9000;
}

Ppu::~Ppu() {
}

void Ppu::writeAddr(uint16_t addr, uint8_t val) {
    // Check if the write is in VRAM
    if((addr & 0xE000) == VRAM_BASE) {
        //VRAM Access is not allowed during pixel transfer
        if(mMode != IOREG_STAT_MODE_3_DATA_XFER)
            mVRAM[addr-VRAM_BASE] = val;
    }
    // Check if the write is in the 0xFFXX region, which we'll
    // then check for known IO Registers that are writable
    else if((addr & 0xFF00) == 0xFF00) {
        uint8_t changes = 0;
        switch(addr) {
            case IOREG_LCDC:
                changes = val ^ mLCDC;
                if(changes & IOREG_LCDC_ENABLE_MASK) {
                    if((val & IOREG_LCDC_ENABLE_MASK) == IOREG_LCDC_ENABLE) {
                        mEnabled = true;
                    } else {
                        mLY = 0;
                        mMode = IOREG_STAT_MODE_2_OAM_SEARCH;
                        mRemCycles = OAM_CYCLES;
                        mEnabled = false;
                    }
                }
                mBgMapOffset = ((val & IOREG_LCDC_BG_TILE_MAP_SEL_9800) == IOREG_LCDC_BG_TILE_MAP_SEL_9800)?OFFSET_9800:OFFSET_9C00;
                mTileDataOffset = ((val & IOREG_LCDC_TILE_DATA_SEL_8000) == IOREG_LCDC_TILE_DATA_SEL_8000)?0:OFFSET_9000;
                mWinMapOffset = ((val & IOREG_LCDC_WIN_TILE_MAP_SEL_MASK) == IOREG_LCDC_WIN_TILE_MAP_SEL_9800)?OFFSET_9800:OFFSET_9C00;
                mLCDC = val;
                DLOG("LCDC write: 0x%02x\n", mLCDC);
                break;
            case IOREG_STAT:
                //TODO
                break;
            case IOREG_SCY:
                mSCY = val;
                break;
            case IOREG_SCX:
                mSCX = val;
                break;
            case IOREG_LYC:
                mLYC = val;
                break;
            case IOREG_BGP:
                mBGP = val;
                break;
            case IOREG_OBP0:
                mOBP0 = val;
                break;
            case IOREG_OBP1:
                mOBP1 = val;
                break;
            case IOREG_WY:
                mWY = val;
                break;
            case IOREG_WX:
                mWX = val;
                break;
        }
    }
    // Check if the write is OAM
    else if(((addr & (0xFF00)) == 0xFE00) && (addr < OAM_TOP)) {
        // OAM can only be accessed during blanking
        if(mMode == IOREG_STAT_MODE_0_HBLANK || mMode == IOREG_STAT_MODE_1_VBLANK)
            mOAM[addr-OAM_BASE] = val;
    }
}

uint8_t Ppu::readAddr(uint16_t addr) {
    // Check if the read is in VRAM
    if((addr & 0xE000) == VRAM_BASE) {
        //VRAM Access is not allowed during pixel transfer
        if(mMode != IOREG_STAT_MODE_3_DATA_XFER) {
            return mVRAM[addr-VRAM_BASE];
        }
    }
    // Check if the read is in the 0xFFXX region, which we'll
    // then check for known IO Registers
    else if((addr & 0xFF00) == 0xFF00) {
        switch(addr) {
            case IOREG_LCDC:
                return mLCDC;
            case IOREG_STAT:
                //TODO
                return mMode;
            case IOREG_SCY:
                return mSCY;
            case IOREG_SCX:
                return mSCX;
            case IOREG_LY:
                return mLY;
            case IOREG_LYC:
                return mLYC;
            case IOREG_BGP:
                return mBGP;
            case IOREG_OBP0:
                return mOBP0;
            case IOREG_OBP1:
                return mOBP1;
            case IOREG_WY:
                return mWY;
            case IOREG_WX:
                return mWX;
        }
    }
    // Check if the read is OAM
    else if(((addr & (0xFF00)) == 0xFE00) && (addr < OAM_TOP)) {
        // OAM can only be accessed during blanking
        if(mMode == IOREG_STAT_MODE_0_HBLANK || mMode == IOREG_STAT_MODE_1_VBLANK)
            return mOAM[addr-OAM_BASE];
    }
    return 0;
}

/* For now, the PPU will not be entirely tick-accurate. Instead of handling each individual tick
 * with interleaved VRAM fetching and progressive pixel mixing, we will simply complete each mode
 * at once before we transition to the next. This means all of OAM serach happens at once, and an
 * entire horizontal line is mixed from VRAM and written to the frame buffer at once. This leaves
 * mid-line register modifications up in the air, but it is far simpler to implement. I may revisit
 * this in the future.
 */
void Ppu::tick(uint8_t cycles) {
    uint8_t vtile, htile, mapEntry;
    uint8_t tileH, tileL;
    uint8_t pixIdx;
    uint8_t pix;
    uint8_t scrolledY;

    if(!mEnabled)
        return;

    if(mRemCycles > cycles) {
        mRemCycles -= cycles;
    } else {
        cycles -= mRemCycles;
        switch(mMode) {
            case IOREG_STAT_MODE_2_OAM_SEARCH:
                mRemCycles = DATA_XFER_CYCLES-cycles;
                mMode = IOREG_STAT_MODE_3_DATA_XFER;
                //DLOG("%s\n", "Switch to XFER");
                break;
            case IOREG_STAT_MODE_3_DATA_XFER:
                scrolledY = mLY + mSCY;
                vtile = scrolledY / 8;
                for(htile=0; htile < 20; ++htile) {
                    mapEntry = mVRAM[mBgMapOffset + (vtile * 32 + htile)];
                    if((mLCDC & IOREG_LCDC_TILE_DATA_SEL_MASK) == IOREG_LCDC_TILE_DATA_SEL_8800) {
                        // In "8800" mode, the tile index is signed
                        tileL = mVRAM[mTileDataOffset + ((int8_t)mapEntry * TILE_SIZE) + (scrolledY % 8)*2];
                        tileH = mVRAM[mTileDataOffset + ((int8_t)mapEntry * TILE_SIZE) + (scrolledY % 8)*2 + 1];
                    } else {
                        tileL = mVRAM[mTileDataOffset + (mapEntry * TILE_SIZE) + (scrolledY % 8)*2];
                        tileH = mVRAM[mTileDataOffset + (mapEntry * TILE_SIZE) + (scrolledY % 8)*2 + 1];
                    }

                    for(pixIdx=0;pixIdx < 8;++pixIdx) {
                        pix = (tileL & 0x80)?0x1:0x0;
                        pix |= (tileH & 0x80)?0x2:0x0;
                        frameBuf[mLY][htile*8+pixIdx] = TO_PALLETTE(pix, mBGP);
                        tileL = tileL << 1;
                        tileH = tileH << 1;
                    }
                }
                mRemCycles = HBLANK_CYCLES-cycles;
                mMode = IOREG_STAT_MODE_0_HBLANK;
                break;
            case IOREG_STAT_MODE_0_HBLANK:
                if(mLY < 143) {
                    // If we are still drawing screen lines, move back to OAM search
                    mRemCycles = OAM_CYCLES-cycles;
                    mMode = IOREG_STAT_MODE_2_OAM_SEARCH;
                    ++mLY;
                } else if(mLY == 143) {
                    // Time for VBlank
                    mRemCycles = LINE_CYCLES - cycles;
                    mMode = IOREG_STAT_MODE_1_VBLANK;
                    ++mLY;
                }
                break;
            case IOREG_STAT_MODE_1_VBLANK:
                if(mLY < 153) {
                    // Eat an entire line's worth of cycles for each VBlank line
                    mRemCycles = LINE_CYCLES - cycles;
                    ++mLY;
                } else {
#if 0
                    DLOG("%c",'+');
                    for(vtile=0;vtile<160;vtile++)
                        DLOG("%c", '-');
                    DLOG("%s","+\n");
                    for(vtile = 0;vtile < 144;++vtile) {
                        DLOG("%c",'|');
                        for(htile =0;htile < 160;++htile) {
                            DLOG("%c", frameBuf[vtile][htile]?'*':' ');
                        }
                        DLOG("%s", "|\n");
                    }
                    DLOG("%c",'+');
                    for(vtile=0;vtile<160;vtile++)
                        DLOG("%c", '-');
                    DLOG("%c",'+');
                    DLOG("%s", "\n");
#endif
                    // Switch back to OAM earch and start the next frame
                    mRemCycles = OAM_CYCLES;
                    mMode = IOREG_STAT_MODE_2_OAM_SEARCH;
                    mLY = 0;
                }
        }
    }
}
