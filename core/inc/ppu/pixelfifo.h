#ifndef __PIXELFIFO_H__
#define __PIXELFIFO_H__

#include "ppu/pixel.h"
#include "ppu/tileline.h"
#include "ppu/registers.h"
#include "ppu/oamentry.h"

#define PIXELFIFO_TICK_LOCKED 0xff

class PixelFIFO {
    public:
        PixelFIFO(PpuRegisters *registers);

        uint8_t tick();
        uint8_t count();
        bool loadLine(TileLine line);
        bool mixSprite(TileLine line, OAMEntry *sprite);
        void clear();
        void lock();
        void unlock();
    private:
        PpuRegisters *mRegs;
        Pixel mPixels[16];
        uint8_t mHead;
        uint8_t mTail;
        uint8_t mCount;
        bool mLocked;
};

#endif /* __PIXELFIFO_H__ */
