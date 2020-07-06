#include "ppu/fetcher.h"
#include "io_regs.h"
#include "ppu/tileline.h"
#include "logger.h"

#define NEXT_STATE(_state) do {FetcherState newstate = (_state < fsWriteLine1)?(FetcherState)(((uint8_t)_state)+1):_state; VLOG(ZONE_PPU, "Fetcher %u => %u\n", _state, newstate); _state = newstate;} while(0)

Fetcher::Fetcher(PixelFIFO *fifo, uint8_t *VRAM, uint8_t *OAMRAM, PpuRegisters *registers) {
    mFIFO = fifo;
    mRegs = registers;
    mVRAM = VRAM;
    mOAMRAM = OAMRAM;
    reset();
}

void Fetcher::tick() {
    uint8_t x;
    uint8_t y;
    uint8_t count;

    FLOG(ZONE_PPU, "Fetcher: Tick Enter (%u, %u)\n", (unsigned int)mState, (unsigned int)mSpriteState);

    switch(mState)
    {
        case fsReadNum0:
        case fsReadHigh0:
        case fsReadLow0:
            // These states take 2 cycles, so each of these states simply eat the first cycle
            NEXT_STATE(mState);
            break;
        case fsReadNum1:
            // Mid-line SCY changes should be respected, so we will always check the register
            // here when selecting a tile.
            y = (mMode == fmBackground)?(mRegs->LY + mRegs->SCY):mWinY;

            //XXX I'm not sure if mid-line SCX changes affect the fetch address.
            //    I've seen some documentation which suggests that the lower 5 bits
            //    of the map address are latched at the beginning of the line,
            //    which would mean that SCX changes have no effect once shifting
            //    begins. However, I've yet to be able to verify that. For now,
            //    always check SCX here, but this may change to a fixed state
            //    later.
            x = (mTileCnt + ((mMode == fmBackground)?mRegs->SCX/8:0))%32;


            mTileNum = mVRAM[((mMode == fmBackground)?mRegs->bgMapOffset:mRegs->winMapOffset) + y / 8 * 32 + x];
            NEXT_STATE(mState);
            break;

        case fsReadHigh1:
            // XXX Again, I am uncertain whether changing SCY in-between a map read and a data
            //     read is respected. I don't see why not, though.
            y = (mMode == fmBackground)?(mRegs->LY + mRegs->SCY)%8 : mWinY % 8;


            if((mRegs->LCDC & IOREG_LCDC_TILE_DATA_SEL_MASK) == IOREG_LCDC_TILE_DATA_SEL_8800)
                mTileData[0] = mVRAM[mRegs->tileDataOffset + ((int8_t)mTileNum * 16) + (y * 2)];
            else
                mTileData[0] = mVRAM[mRegs->tileDataOffset + (mTileNum * 16) + (y * 2)];

            NEXT_STATE(mState);
            break;

        case fsReadLow1:
            y = (mMode == fmBackground)?(mRegs->LY + mRegs->SCY)%8 : mWinY % 8;
            if((mRegs->LCDC & IOREG_LCDC_TILE_DATA_SEL_MASK) == IOREG_LCDC_TILE_DATA_SEL_8800)
                mTileData[1] = mVRAM[mRegs->tileDataOffset + ((int8_t)mTileNum * 16) + (y * 2)+1];
            else
                mTileData[1] = mVRAM[mRegs->tileDataOffset + (mTileNum * 16) + (y * 2)+1];

            mLine.load(mTileData, (mMode == fmBackground)?Pixel::ptBackground:Pixel::ptWindow, 0);

            //XXX Does the line load here, or is there another tick?
            NEXT_STATE(mState);
            break;
        case fsWriteLine0:
            // Docs/research indicates the fetcher X increments exactly in this step
            ++mTileCnt;

            // Fall through!
        case fsWriteLine1:
            tryWriteLine();
            break;
        case fsIdle:
            // Do nothing, the sprite fetch will pull us out of this state
            break;
        default:
            break;
    }

    // Sprite fetch is allowed to start after tick "5" based on documentation I've seen
    if(mSpriteState != fsIdle && mState >= fsReadLow1) {
        switch(mSpriteState) {
            case fsReadNum0:
            case fsReadHigh0:
            case fsReadLow0:
                NEXT_STATE(mSpriteState);
                break;
            case fsReadNum1:
                // TODO Figure out when memory is actually accessed here. For now, this already cached in my OAMEntry
                mSpriteTileNum = mSprite->tile;
                NEXT_STATE(mSpriteState);
                break;
            case fsReadHigh1:
                // Selecting the Y line does not depend on whether the sprite is in 8 or 16 px height mode,
                // Y is always based on an offset of 16
                if(mSprite->yFlip)
                    y = mSprite->y - mRegs->LY - 1;
                else
                    y = 16 - (mSprite->y - mRegs->LY);

                mSpriteTileData[0] = mVRAM[mSpriteTileNum * (((mRegs->LCDC & IOREG_LCDC_OBJ_SIZE_MASK) == IOREG_LCDC_OBJ_SIZE_8_16)?32:16) + (y*2)];
                NEXT_STATE(mSpriteState);
                break;
            case fsReadLow1:
                // Selecting the Y line does not depend on whether the sprite is in 8 or 16 px height mode,
                // Y is always based on an offset of 16
                if(mSprite->yFlip)
                    y = mSprite->y - mRegs->LY - 1;
                else
                    y = 16 - (mSprite->y - mRegs->LY);

                mSpriteTileData[1] = mVRAM[mSpriteTileNum * (((mRegs->LCDC & IOREG_LCDC_OBJ_SIZE_MASK) == IOREG_LCDC_OBJ_SIZE_8_16)?32:16) + (y*2) + 1];

                mSpriteLine.load(mSpriteTileData, mSprite->OBP1?Pixel::ptSprite1:Pixel::ptSprite0, mSprite->belowBG?PIXEL_FLAGS_SPRITE_BELOW_BG:0, mSprite->xFlip);
                mFIFO->loadMixSprite(mSpriteLine, mSprite);

                if(mState == fsIdle)
                    mState = fsReadNum0;

                mSpriteState = fsIdle;
                mSprite = NULL;

                // TODO move this somewhere?
                mFIFO->unlock();
                break;
            default:
                break;
        }
    }
    FLOG(ZONE_PPU, "%s\n", "Fetcher: Tick Exit");
}

void Fetcher::reset() {
    mState = fsReadNum0;
    mSpriteState = fsIdle;
    mMode = fmBackground;
    mSprite = NULL;
    mTileCnt = 0;
}

void Fetcher::setMode(FetcherMode mode) {
    mMode = mode;
    mTileCnt = 0;
}

void Fetcher::setMode(FetcherMode mode, uint8_t winY) {
    if(mMode != mode)
        mState = fsReadNum0;

    mMode = mode;
    mWinY = winY;
    mTileCnt = 0;
}

Fetcher::FetcherMode Fetcher::getMode() {
    return mMode;
}

void Fetcher::setWinY(uint8_t winY) {
    mWinY = winY;
}

void Fetcher::startSprite(OAMEntry *sprite) {
    mSprite = sprite;
    mSpriteState = fsReadNum0;
}

bool Fetcher::spritePending() {
    return (mSprite != NULL);
}

bool Fetcher::tryWriteLine() {
    if(mFIFO->count() == 0 && mFIFO->loadBG(mLine)) {
        /* If we succesfully loaded the line, then we either need to restart
         * the fetched or move into a pending state if we are awaiting a sprite fetch. */
        VLOG(ZONE_PPU, "%s\n", "Loaded BG Tile");
        mState = (mSpriteState == fsIdle)?fsReadNum0:fsIdle;
        return true;
    }

    /* We cannot load the line yet, so just advance the state. */
    NEXT_STATE(mState);
    return false;
}
