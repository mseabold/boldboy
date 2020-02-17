#ifndef __PPU_H__
#define __PPU_H__

#include "memregion.h"
#include "int_controller.h"
#include "io_regs.h"

#define MAX_NUM_OBJS 10

class Ppu : public MemRegion {
    public:
        typedef enum {
            modeOAMSearch = IOREG_STAT_MODE_2_OAM_SEARCH,
            modeHBlank = IOREG_STAT_MODE_0_HBLANK,
            modeVBlank = IOREG_STAT_MODE_1_VBLANK,
            modeDataTransfer = IOREG_STAT_MODE_3_DATA_XFER
        } PpuMode;
        Ppu(InterruptController *ic);
        ~Ppu();

        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);
        void tick(uint8_t cycles);
        void getFrame(uint8_t frame[144][160]);
        PpuMode getMode();
    private:
        uint8_t mVRAM[0x2000];
        uint8_t mOAM[0xA0];
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
        uint8_t mSTAT;

        uint16_t mBgMapOffset;
        uint16_t mWinMapOffset;
        uint16_t mTileMapOffset;
        uint16_t mTileDataOffset;

        uint16_t mRemCycles;

        bool mEnabled;
        InterruptController *mIC;

        uint8_t mNumObjs;
        uint8_t mFoundObjs[10];

        uint8_t mCurWinY;

        uint8_t frameBuf[144][160];

        void setLine(uint8_t line);
        void setMode(uint8_t mode);


        class Tile {
            public:
                Tile(uint8_t lo, uint8_t hi) {
                    mHi = hi;
                    mLo = lo;
                    mLeft = 8;
                }

                Tile(const Tile &other) {
                    mHi = other.mHi;
                    mLo = other.mLo;
                    mLeft = other.mLeft;
                }

                void shift(uint8_t n) {
                    if(mLeft) {
                        n = (mLeft >= n)?n:mLeft;
                        mHi = mHi << n;
                        mLo = mLo << n;
                        mLeft -= n;
                    }
                }

                uint8_t shiftout(void) {
                    uint8_t pix = ((mHi & 0x80)?0x02:0x00) | ((mLo & 0x80)?0x01:0x00);
                    shift(1);
                    return pix;
                }

                bool isDone(void) {
                    return mLeft == 0;
                }

                uint8_t mHi;
                uint8_t mLo;
                uint8_t mLeft;
        };

        Tile loadTile(bool isWindow, uint8_t y, uint8_t x);

};

#endif /* __PPU_H__ */
