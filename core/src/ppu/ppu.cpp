#include "ppu.h"
#include "io_regs.h"
#include "logger.h"
#include "string.h"

#define VRAM_BASE         0x8000
#define OAM_BASE 0xFE00
#define OAM_TOP 0xFEA0

#define TO_PALLETTE(_pix, _pal) ((_pal >> ((_pix & 0x3) * 2)) & 0x3)

#define TILE_SIZE 16

#define OAM_CYCLES 80
#define DATA_XFER_CYCLES 168
#define HBLANK_CYCLES 208
#define LINE_CYCLES 456

#define MODE (mRegs->STAT & IOREG_STAT_MODE_MASK)

static const uint8_t DMG_PALLETTE[] = { 255, 169, 84, 0};

Ppu::Ppu(InterruptController *ic) {
    mIC = ic;
    mEnabled = false;
    mCurWinY = 0;
    mLineCycles = 0;

    mRegs = new PpuRegisters();
    mFIFO = new PixelFIFO(mRegs);
    mFetcher = new Fetcher(mFIFO, mVRAM, mOAM, mRegs);
    setMode(IOREG_STAT_MODE_2_OAM_SEARCH);
}

Ppu::~Ppu() {
    delete mFIFO;
    delete mFetcher;
    delete mRegs;
}

void Ppu::writeAddr(uint16_t addr, uint8_t val) {
    // Check if the write is in VRAM
    if((addr & 0xE000) == VRAM_BASE) {
        //VRAM Access is not allowed during pixel transfer
        if(!mEnabled || MODE != IOREG_STAT_MODE_3_DATA_XFER)
            mVRAM[addr-VRAM_BASE] = val;
    }
    // Check if the write is in the 0xFFXX region, which we'll
    // then check for known IO Registers that are writable
    else if((addr & 0xFF00) == 0xFF00) {
        uint8_t changes = 0;

        // Handle LCDC here as well to clear states
        if(addr == IOREG_LCDC) {
            changes = val ^ mRegs->LCDC;
            if(changes & IOREG_LCDC_ENABLE_MASK) {
                if((val & IOREG_LCDC_ENABLE_MASK) == IOREG_LCDC_ENABLE) {
                    mEnabled = true;
                } else {
                    mRegs->LY = 0;
                    mCurWinY = 0;
                    mEnabled = false;
                    // XXX Should this be possible to trigger an interrupt?
                    setMode(IOREG_STAT_MODE_2_OAM_SEARCH);
                    mLineCycles = 0;
                }
            }
        }

        // Pass on to the registers
        mRegs->write(addr, val);
    }
    // Check if the write is OAM
    else if(((addr & (0xFF00)) == 0xFE00) && (addr < OAM_TOP)) {
        // OAM can only be accessed during blanking
        if(!mEnabled || MODE == IOREG_STAT_MODE_0_HBLANK || MODE == IOREG_STAT_MODE_1_VBLANK)
            mOAM[addr-OAM_BASE] = val;
    }
}

uint8_t Ppu::readAddr(uint16_t addr) {
    // Check if the read is in VRAM
    if((addr & 0xE000) == VRAM_BASE) {
        //VRAM Access is not allowed during pixel transfer
        if(MODE != IOREG_STAT_MODE_3_DATA_XFER) {
            return mVRAM[addr-VRAM_BASE];
        }
    }
    // Check if the read is in the 0xFFXX region, which we'll
    // then check for known IO Registers
    else if((addr & 0xFF00) == 0xFF00) {
        return mRegs->read(addr);
    }
    // Check if the read is OAM
    else if(((addr & (0xFF00)) == 0xFE00) && (addr < OAM_TOP)) {
        // OAM can only be accessed during blanking
        if(MODE == IOREG_STAT_MODE_0_HBLANK || MODE == IOREG_STAT_MODE_1_VBLANK)
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
    uint8_t pix;
    uint8_t consumedCycles;
    uint8_t idx,idx2;
    uint8_t *oamPtr;
    uint8_t oamY;
    OAMEntry temp;
    bool newSpritePending;

    if(!mEnabled)
        return;

    while(cycles) {
        consumedCycles = cycles;

        switch(MODE) {
            case IOREG_STAT_MODE_2_OAM_SEARCH:
                if(mLineCycles + cycles >= OAM_CYCLES) {
                    mNumObjs = 0;
                    mCurObj = 0;

                    for(oamPtr = mOAM; oamPtr < mOAM + 0xA0 && mNumObjs < MAX_NUM_OBJS; oamPtr += 4) {
                        oamY = oamPtr[0];

                        if(oamY > 0 && mRegs->LY < oamY && (oamY - mRegs->LY - 1) < 16 && (oamY - mRegs->LY) >= 9) {
                            mFoundObjs[mNumObjs++] = OAMEntry(oamPtr, (oamPtr-mOAM)/4);
                        }
                    }

                    // Do a quick and dirty sort of the found entries for easier access later.
                    // Considering the max of 10 entries, this should be fairly low cost.
                    for(idx = 0; idx < mNumObjs-1; ++idx) {
                        for(idx2 = idx+1; idx2 < mNumObjs; ++idx2) {
                            if(mFoundObjs[idx2].x < mFoundObjs[idx].x || (mFoundObjs[idx2].x == mFoundObjs[idx].x && mFoundObjs[idx2].oamIdx < mFoundObjs[idx].oamIdx)) {
                                temp = mFoundObjs[idx2];
                                mFoundObjs[idx2] = mFoundObjs[idx];
                                mFoundObjs[idx] = temp;
                            }
                        }
                    }

                    setMode(IOREG_STAT_MODE_3_DATA_XFER);
                    mLinePixCnt = 0;
                    mTossedPixCnt = 0;
                    mPreFifoTicks = 8;
                    consumedCycles = OAM_CYCLES - mLineCycles;

                    // Start with an empty line so that FIFO can start early and shift off-screen
                    // sprite pixels out before shifting to the display
                    mFIFO->loadBG(TileLine::empty());
                }
                //DLOG("%s\n", "Switch to XFER");
                break;
            case IOREG_STAT_MODE_3_DATA_XFER:
                consumedCycles = 0;

                for(idx=0; idx < cycles; ++idx) {
                    ++consumedCycles;

                    // We can safely start a sprite even before we have loaded enough pixels into the FIFO,
                    // because the Fetcher will check the count before starting fetching a Sprite.
                    // This is being done this way in order to atomically handle ticking the Fetcher and
                    // actually starting a Sprite.
#if 1
                    newSpritePending = false;

                    // TODO Verify the fetch timing for off-screen sprites
                    if((mRegs->LCDC & IOREG_LCDC_OBJ_DISPLAY_MASK) == IOREG_LCDC_OBJ_DISPLAY_ON && mCurObj < mNumObjs && mFoundObjs[mCurObj].x == mLinePixCnt + 8 - mPreFifoTicks) {
                        if(!mFetcher->spritePending()) {
                            mFIFO->lock();
                            mFetcher->startSprite(&mFoundObjs[mCurObj]);
                            ++mCurObj;
                        } else
                            newSpritePending = true;
                    }
#endif

                    mFetcher->tick();

                    // If another sprite at the same needs to be handled, we do not want to process the FIFO yet
                    if(newSpritePending)
                        continue;

                    // Eat scrolled off pixels
                    if(mPreFifoTicks > 0) {
                        if(mFIFO->tick() != PIXELFIFO_TICK_LOCKED)
                            --mPreFifoTicks;
                        continue;
                    }

                    // Toss any pixels before we do anything else
                    if(mTossedPixCnt < mRegs->SCX % 8) {
                        pix = mFIFO->tick();

                        if(pix != PIXELFIFO_TICK_LOCKED)
                            ++mTossedPixCnt;

                        // Always short circuit the rest of this tick, even if we didn't actually drop a pixel.
                        // We can't start the window until we load BG tiles and drop the correct number
                        // of pixels, even if there's a window at X position 0.
                        continue;
                    }

                    if((mRegs->LCDC & IOREG_LCDC_WIN_DISPLAY_MASK) == IOREG_LCDC_WIN_DISPLAY_ON && mLinePixCnt >= mRegs->WX - 7 && mRegs->LY >= mRegs->WY && mFetcher->getMode() != Fetcher::fmWindow) {
                        mFetcher->setMode(Fetcher::fmWindow, mCurWinY);
                        mFIFO->clear(false);
                        // TODO Check the exact timing here between switching over.
                        continue;
                    }


                    pix = mFIFO->tick();

                    if(pix != PIXELFIFO_TICK_LOCKED) {
                        //VLOG("Shifted out [%u][%u] = 0x%02x\n", mRegs->LY, mLinePixCnt, pix);
                        frameBuf[mRegs->LY][mLinePixCnt] = DMG_PALLETTE[pix];
                        ++mLinePixCnt;
                    }


                    if(mLinePixCnt == 160) {
                        setMode(IOREG_STAT_MODE_0_HBLANK);
                        mFetcher->reset();
                        mFIFO->clear(true);

                        if((mRegs->LCDC & IOREG_LCDC_WIN_DISPLAY_MASK) == IOREG_LCDC_WIN_DISPLAY_ON && mRegs->LY >= mRegs->WY)
                            ++mCurWinY;

                        // Stop transferring data
                        break;
                    }
                }
                break;
            case IOREG_STAT_MODE_0_HBLANK:
                if(mLineCycles + cycles >= LINE_CYCLES) {
                    consumedCycles = LINE_CYCLES-mLineCycles;
                    if(mRegs->LY < 143) {
                        // If we are still drawing screen lines, move back to OAM search
                        setMode(IOREG_STAT_MODE_2_OAM_SEARCH);
                        setLine(mRegs->LY+1);
                    } else if(mRegs->LY == 143) {
                        // Time for VBlank
                        setMode(IOREG_STAT_MODE_1_VBLANK);
                        setLine(mRegs->LY+1);
                    }
                }
                break;
            case IOREG_STAT_MODE_1_VBLANK:
                    if(mLineCycles + cycles >= LINE_CYCLES) {
                        consumedCycles = LINE_CYCLES-cycles;

                        if(mRegs->LY < 153) {
                            // Eat an entire line's worth of cycles for each VBlank line
                            setLine(mRegs->LY+1);
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
                            mCurWinY = 0;
                            setMode(IOREG_STAT_MODE_2_OAM_SEARCH);
                            setLine(0);
                            VLOG("%s\n", "====== Start Frame");
                        }
                }
        }

        mLineCycles += consumedCycles;
        cycles -= consumedCycles;

        // If we completed the line (End of HBlank or end of VBlank Line,
        // reset the line cycle count
        if(mLineCycles >= LINE_CYCLES) {
            mLineCycles = 0;
        }
    }
}

void Ppu::setLine(uint8_t line) {
    mRegs->LY = line;

    if(mRegs->LY == mRegs->LYC) {
        mRegs->STAT |= IOREG_STAT_COINCIDENCE_FLAG_EQ;

        if(mRegs->STAT & IOREG_STAT_COINCIDENCE_INTR)
            mIC->requestInterrupt(InterruptController::itLCDCStatus);
    } else {
        mRegs->STAT &= ~IOREG_STAT_COINCIDENCE_FLAG_EQ;
    }
}

void Ppu::setMode(uint8_t mode) {
    mRegs->STAT = (mRegs->STAT & ~IOREG_STAT_MODE_MASK) | mode;

    if(mode == IOREG_STAT_MODE_1_VBLANK)
        mIC->requestInterrupt(InterruptController::itVBlank);

    /*
     * The order of STAT bits for enabling interrupts mirrors the order
     * of mode values, so we can simply shift by mode to check the corresponding
     * bit.
     * TODO: There is more precise timing here
     */
    if(mEnabled && mode != IOREG_STAT_MODE_3_DATA_XFER && (mRegs->STAT & (1 << (IOREG_STAT_INTRS_SHIFT + mode))))
        mIC->requestInterrupt(InterruptController::itLCDCStatus);
}

void Ppu::getFrame(uint8_t frame[144][160]) {
    memcpy(frame, frameBuf, 144*160);
}

Ppu::PpuMode Ppu::getMode() {
    return static_cast<PpuMode>(mRegs->STAT & IOREG_STAT_MODE_MASK);
}
