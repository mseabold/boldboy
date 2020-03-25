#include "ppu/pixelfifo.h"
#include "io_regs.h"
#include "logger.h"

#define TO_PALLETTE(_pix, _pal) ((_pal >> ((_pix & 0x3) * 2)) & 0x3)

PixelFIFO::PixelFIFO(PpuRegisters *registers) {
    mRegs = registers;
    mBGHead = 0;
    mBGTail = 0;
    mBGCount = 0;
    mOBJHead = 0;
    mOBJTail = 0;
    mOBJCount = 0;
    mLocked = false;
}

uint8_t PixelFIFO::tick() {
    uint8_t pallette;
    Pixel pix;

    if(mBGCount == 0 || mLocked)
        return PIXELFIFO_TICK_LOCKED;

    pix = mBGPixs[mBGHead];
    pallette = mRegs->BGP;

    if(pix.type == Pixel::ptBackground && ((mRegs->LCDC & IOREG_LCDC_BG_DISPLAY_MASK) == IOREG_LCDC_BG_DISPLAY_OFF)) {
        pix.value = 0;
        pallette = 0;
    }

    if(pix.type == Pixel::ptWindow && ((mRegs->LCDC & IOREG_LCDC_WIN_DISPLAY_MASK) == IOREG_LCDC_WIN_DISPLAY_OFF)) {
        pix.value = 0;
        pallette = 0;
    }

    mBGHead = (mBGHead + 1) % 16;
    --mBGCount;

    if(mOBJCount > 0 && ((mRegs->LCDC & IOREG_LCDC_OBJ_DISPLAY_MASK) == IOREG_LCDC_OBJ_DISPLAY_ON)) {
        if(mOBJPixs[mOBJHead].value != 0) {
            if(!(mOBJPixs[mOBJHead].flags & PIXEL_FLAGS_SPRITE_BELOW_BG) || pix.value == 0) {
                pix = mOBJPixs[mOBJHead];
                pallette = (mOBJPixs[mOBJHead].type == Pixel::ptSprite0)?mRegs->OBP0:mRegs->OBP1;
            }
        }
        mOBJHead = (mOBJHead + 1) % 16;
        --mOBJCount;
    }


    return TO_PALLETTE(pix.value, pallette);
}

uint8_t PixelFIFO::count() {
    return mBGCount;
}

bool PixelFIFO::loadBG(TileLine line) {
    // Cannot load a line if there is not enough space
    if(mBGCount > 8)
        return false;

    uint8_t idx;

    for(idx = 0; idx < 8; ++idx, mBGTail = (mBGTail + 1) % 16, ++mBGCount) {
        mBGPixs[mBGTail] = line.get(idx);
    }

    return true;
}

bool PixelFIFO::loadMixSprite(TileLine line, OAMEntry *sprite) {
    uint8_t spriteIdx, fifoIdx;

    // Mix any existing sprite pixels
    for(spriteIdx = 0, fifoIdx = mOBJHead; spriteIdx < 8 && spriteIdx < mOBJCount; ++spriteIdx, fifoIdx = (fifoIdx + 1) % 16) {
        // The previous sprite has priority, so only take this sprite's pixel if
        // this sprite isn't transparent and the previous one is.
        if(line.get(spriteIdx).value != 0 && mOBJPixs[fifoIdx].value == 0) {
            mOBJPixs[fifoIdx] = line.get(spriteIdx);
        }
    }

    // Now take any remaining pixels (non-overlapping) directly)
    for(; spriteIdx < 8; ++spriteIdx, mOBJTail = (mOBJTail + 1) % 16, ++mOBJCount) {
        mOBJPixs[mOBJTail] = line.get(spriteIdx);
    }

    return true;
}

void PixelFIFO::clear(bool clearSprites) {
    mBGHead = 0;
    mBGTail = 0;
    mBGCount = 0;

    if(clearSprites) {
        mOBJHead = 0;
        mOBJTail = 0;
        mOBJCount = 0;
    }
}

void PixelFIFO::lock() {
    mLocked = true;
}

void PixelFIFO::unlock() {
    mLocked = false;
}
