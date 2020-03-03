#include "ppu/pixelfifo.h"

#define TO_PALLETTE(_pix, _pal) ((_pal >> ((_pix & 0x3) * 2)) & 0x3)

PixelFIFO::PixelFIFO(PpuRegisters *registers) {
    mRegs = registers;
    mHead = 0;
    mTail = 0;
    mCount = 0;
}

uint8_t PixelFIFO::tick() {
    uint8_t pallette;
    if(mCount <= 8 || mLocked)
        return PIXELFIFO_TICK_LOCKED;

    switch(mPixels[mHead].type) {
        case Pixel::ptBackground:
        case Pixel::ptWindow:
            pallette = mRegs->BGP;
            break;
        case Pixel::ptSprite0:
            pallette = mRegs->OBP0;
            break;
        case Pixel::ptSprite1:
            pallette = mRegs->OBP1;
            break;
        default:
            // This should probably never happen, but return BGP just to be sure
            pallette = mRegs->BGP;
            break;
    }

    uint8_t pix = TO_PALLETTE(mPixels[mHead].value, pallette);
    mHead = (mHead + 1) % 16;
    --mCount;

    return pix;
}

uint8_t PixelFIFO::count() {
    return mCount;
}

bool PixelFIFO::loadLine(TileLine line) {
    // Cannot load a line if there is not enough space
    if(mCount > 8)
        return false;

    uint8_t idx;

    for(idx = 0; idx < 8; ++idx, mTail = (mTail + 1) % 16, ++mCount) {
        mPixels[mTail] = line.get(idx);
    }

    return true;
}

bool PixelFIFO::mixSprite(TileLine line, OAMEntry *sprite) {
    // Cannot mix a sprite if less than 8 pixels are available
    if(mCount < 8)
        return false;

    uint8_t spriteIdx, fifoIdx;

    for(spriteIdx = 0, fifoIdx = mHead; spriteIdx < 8; ++spriteIdx, fifoIdx = (fifoIdx + 1) % 16) {
        // If the sprite pixel is 0 (transparent), we can skip it
        if(line.get(spriteIdx).value != 0) {
            switch(mPixels[fifoIdx].type) {
                case Pixel::ptBackground:
                case Pixel::ptWindow:
                    // For BG and WIN pixels in FIFO, either we always overlay the sprite,
                    // or we overlay the sprite only if the existing pixel is 0, depending
                    // on the OAM priority bit
                    if(!sprite->belowBG || mPixels[fifoIdx].value == 0) {
                        mPixels[fifoIdx] = line.get(spriteIdx);
                    }
                    break;
                default:
                    // Sprites cannot be overwritten
                    break;
            }
        }
    }

    return true;
}

void PixelFIFO::clear() {
    mHead = 0;
    mTail = 0;
    mCount = 0;
}

void PixelFIFO::lock() {
    mLocked = true;
}

void PixelFIFO::unlock() {
    mLocked = false;
}
