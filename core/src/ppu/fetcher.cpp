#include "ppu/fetcher.h"
#include "ppu/tileline.h"
#include "logger.h"

Fetcher::Fetcher(uint8_t *VRAM, uint8_t *OAMRAM, PpuRegisters *registers) {
    mRegs = registers;
    mVRAM = VRAM;
    mOAMRAM = OAMRAM;
    reset();
}

bool Fetcher::tick() {
    bool tileDone = false;

    // Filter out every-other tick since the fetcher runs at half speed
    if(mTick0) {
        mTick0 = false;
        return (mState == fsTileReady || mState == fsSpriteReady);
    }

    mTick0 = true;

    uint8_t x;
    uint8_t y;

    switch(mState)
    {
        case fsReadNum:
            // Mid-line SCY changes should be respected, so we will always check the register
            // here when selecting a tile.
            y = ((mMode == fmBackground)?(mRegs->LY + mRegs->SCY):mWinY)/8;

            //XXX I'm not sure if mid-line SCX changes affect the fetch address.
            //    I've seen some documentation which suggests that the lower 5 bits
            //    of the map address are latched at the beginning of the line,
            //    which would mean that SCX changes have no effect once shifting
            //    begins. However, I've yet to be able to verify that. For now,
            //    always check SCX here, but this may change to a fixed state
            //    later.
            x = (mTileCnt + ((mMode == fmBackground)?mRegs->SCX/8:0))%32;

            //VLOG("Fetchers reads tile %u (0x%04x) (SCX=%u)\n", mTileCnt, ((mMode == fmBackground)?mRegs->bgMapOffset:mRegs->winMapOffset) + y * 32 + x + 0x8000, mRegs->SCX);
            mTileNum = mVRAM[((mMode == fmBackground)?mRegs->bgMapOffset:mRegs->winMapOffset) + y * 32 + x];
            mState = fsReadD0;
            break;

        case fsReadD0:
            // XXX Again, I am uncertain whether changing SCY in-between a map read and a data
            //     read is respected. I don't see why not, though.
            y = (mMode == fmBackground)?(mRegs->LY + mRegs->SCY)%8 : mWinY % 8;

            if((mRegs->LCDC & IOREG_LCDC_TILE_DATA_SEL_MASK) == IOREG_LCDC_TILE_DATA_SEL_8800)
                mTileData[0] = mVRAM[mRegs->tileDataOffset + ((int8_t)mTileNum * 16) + (y * 2)];
            else
                mTileData[0] = mVRAM[mRegs->tileDataOffset + (mTileNum * 16) + (y * 2)];
            mState = fsReadD1;
            break;

        case fsReadD1:
            y = (mMode == fmBackground)?(mRegs->LY + mRegs->SCY)%8 : mWinY % 8;
            if((mRegs->LCDC & IOREG_LCDC_TILE_DATA_SEL_MASK) == IOREG_LCDC_TILE_DATA_SEL_8800)
                mTileData[1] = mVRAM[mRegs->tileDataOffset + ((int8_t)mTileNum * 16) + (y * 2)+1];
            else
                mTileData[1] = mVRAM[mRegs->tileDataOffset + (mTileNum * 16) + (y * 2)+1];
            mState = fsTileReady;
            ++mTileCnt;

            mLine.load(mTileData, mMode == fmWindow?Pixel::ptWindow:Pixel::ptBackground);

            mState = fsTileReady;

            tileDone = true;
            break;

        case fsTileReady:
            tileDone = true;
            break;

        default:
            break;
    }

    return tileDone;
}

TileLine Fetcher::getLine() {
    // Clear out any sprite and return to normal mode
    mSprite = NULL;
    mState = fsReadNum;
    return mLine;
}

void Fetcher::reset() {
    mState = fsReadNum;
    mMode = fmBackground;
    mSprite = 0;
    mTileCnt = 0;
    mTick0 = true;
}

void Fetcher::setMode(FetcherMode mode) {
    mMode = mode;
    mTileCnt = 0;
}

void Fetcher::setMode(FetcherMode mode, uint8_t winY) {
    mMode = mode;
    mWinY = winY;
}

Fetcher::FetcherMode Fetcher::getMode() {
    return mMode;
}

void Fetcher::setWinY(uint8_t winY) {
    mWinY = winY;
}

void Fetcher::startSprite(OAMEntry *sprite) {
    // XXX Should we force a BG tile to finish?
    mSprite = sprite;
    mState = fsReadSpriteNum;
}

