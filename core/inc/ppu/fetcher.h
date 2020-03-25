#ifndef __FETCHER_H__
#define __FETCHER_H__

#include "ppu/tileline.h"
#include "ppu/oamentry.h"
#include "ppu/registers.h"
#include "ppu/pixelfifo.h"

class Fetcher {
    public:
        enum FetcherMode {
            fmBackground,
            fmWindow
        };
        Fetcher(PixelFIFO *fifo, uint8_t *VRAM, uint8_t *OAMRAM, PpuRegisters *registers);

        void tick();
        void reset();
        void setMode(FetcherMode mode);
        void setMode(FetcherMode mode, uint8_t winY);
        FetcherMode getMode();
        void setWinY(uint8_t winY);
        void startSprite(OAMEntry *sprite);
        bool spritePending();

    private:
        enum FetcherState {
            fsReadNum0,
            fsReadNum1,
            fsReadHigh0,
            fsReadHigh1,
            fsReadLow0,
            fsReadLow1,
            fsWriteLine0,
            fsWriteLine1,
            fsIdle
        };

        PpuRegisters *mRegs;
        uint8_t *mVRAM;
        uint8_t *mOAMRAM;

        uint8_t mTileCnt;
        uint8_t mTileNum;
        uint8_t mTileData[2];
        uint8_t mSpriteTileNum;
        uint8_t mSpriteTileData[2];

        uint8_t mWinY;

        FetcherState mState;
        FetcherState mSpriteState;
        FetcherMode mMode;
        TileLine mLine;
        TileLine mSpriteLine;

        OAMEntry *mSprite;
        PixelFIFO *mFIFO;

        bool tryWriteLine();
};

#endif /* __FETCHER_H__ */
