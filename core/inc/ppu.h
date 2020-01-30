#ifndef __PPU_H__
#define __PPU_H__

#include "memregion.h"
#include "int_controller.h"

class Ppu : public MemRegion {
    public:
        Ppu(InterruptController *ic);
        ~Ppu();

        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);
        void tick(uint8_t cycles);
    private:
        uint8_t mVRAM[0x2000];
        uint8_t mOAM[0xA0];
        uint8_t mMode;
        uint8_t mLCDC;
        uint8_t mSCY;
        uint8_t mSCX;
        uint8_t mLY;
        uint8_t mLYC;
        uint8_t mBGP;
        uint8_t mOBP0;
        uint8_t mOBP1;
        uint8_t mWY;
        uint8_t mWX;

        uint16_t mBgMapOffset;
        uint16_t mWinMapOffset;
        uint16_t mTileMapOffset;
        uint16_t mTileDataOffset;

        uint16_t mRemCycles;

        bool mEnabled;
        InterruptController *mIC;

        uint8_t frameBuf[144][160];
};

#endif /* __PPU_H__ */
