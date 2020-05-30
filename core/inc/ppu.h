#ifndef __PPU_H__
#define __PPU_H__

#include "memregion.h"
#include "int_controller.h"
#include "io_regs.h"
#include "ppu/oamentry.h"
#include "ppu/registers.h"
#include "ppu/fetcher.h"
#include "ppu/pixelfifo.h"

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
        PpuRegisters *mRegs;

        uint16_t mLineCycles;

        bool mEnabled;
        InterruptController *mIC;

        uint8_t mNumObjs;
        uint8_t mCurObj;
        OAMEntry mFoundObjs[10];

        uint8_t mCurWinY;

        uint8_t frameBuf[144][160];

        void setLine(uint8_t line);
        void setMode(uint8_t mode);

        Fetcher *mFetcher;
        PixelFIFO *mFIFO;

        int16_t mLineXPos;
};

#endif /* __PPU_H__ */
