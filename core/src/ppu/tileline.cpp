#include "ppu/tileline.h"
#include "logger.h"

TileLine::TileLine() {
    mIsFlipped = false;
}

TileLine::TileLine(uint8_t *tileData, Pixel::PixelType pixelType, uint8_t flags) {
    load(tileData, pixelType, flags, false);
}

TileLine::TileLine(const TileLine &other) {
    for(uint8_t i=0; i<8; i++)
        mPixels[i] = other.mPixels[i];
    mIsFlipped = other.mIsFlipped;
}

void TileLine::load(uint8_t *tileData, Pixel::PixelType pixelType, uint8_t flags, bool flipped) {
    // Start by offseting the hi bits left by one so they are easier to combine
    uint8_t lo = tileData[0];
    uint16_t hi = (uint16_t)tileData[1] << 1;

    //VLOG("Loading tile data 0x%02x%02x\n", tileData[0], tileData[1]);

    // We're from the bottom of the data bytes, which are the last pixels
    for(int8_t pixIdx = 7; pixIdx >= 0; --pixIdx) {
        // Hi bits are shifted one left, so we can simply combine bits 0 and 1 from the values
        uint8_t pix = (hi & 0x02) | (lo & 0x01);
        mPixels[pixIdx] = Pixel(pix, pixelType, flags);

        // Shift the bits down to pull the next hi and lo combination
        hi = hi >> 1;
        lo = lo >> 1;
    }

    for(uint8_t pixIdx = 0; pixIdx < 8; ++ pixIdx) {
        //VLOG("Line[%u] = 0x%02x\n", pixIdx, mPixels[pixIdx].value);
    }

    mIsFlipped = flipped;
}

void TileLine::load(uint8_t *tileData, Pixel::PixelType pixelType, uint8_t flags) {
    load(tileData, pixelType, flags, false);
}

void TileLine::setFlipped(bool flipped) {
    mIsFlipped = flipped;
}

bool TileLine::isFlipped() {
    return mIsFlipped;
}

Pixel TileLine::get(uint8_t idx) {
    if(idx < 8) {
        return mPixels[mIsFlipped?(7 - idx):idx];
    }

    return Pixel();
}

TileLine TileLine::empty() {
    uint8_t data[2] = {0,0};
    return TileLine(data, Pixel::ptBackground, 0);
}
