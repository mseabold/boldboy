#ifndef __FETCHER_H__
#define __FETCHER_H__

#include "ppu/tileline.h"
#include "ppu/oamentry.h"
#include "ppu/registers.h"

class Fetcher {
    public:
        enum FetcherMode {
            fmBackground,
            fmWindow
        };
        Fetcher(uint8_t *VRAM, uint8_t *OAMRAM, PpuRegisters *registers);

        bool tick();
        TileLine getLine();
        void reset();
        void setMode(FetcherMode mode);
        void setMode(FetcherMode mode, uint8_t winY);
        FetcherMode getMode();
        void setWinY(uint8_t winY);
        void startSprite(OAMEntry *sprite);

    private:
        enum FetcherState {
            fsReadNum,
            fsReadD0,
            fsReadD1,
            fsIdle,
            fsTileReady,
            fsReadSpriteNum,
            fsReadSpriteD0,
            fsReadSpriteD1,
            fsSpriteReady
        };

        PpuRegisters *mRegs;
        uint8_t *mVRAM;
        uint8_t *mOAMRAM;

        uint8_t mTileCnt;
        uint8_t mTileNum;
        uint8_t mTileData[2];

        bool mTick0;
        uint8_t mWinY;

        FetcherState mState;
        FetcherMode mMode;
        TileLine mLine;

        OAMEntry *mSprite;
};

#endif /* __FETCHER_H__ */
