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
        bool loadBG(TileLine line);
        bool loadMixSprite(TileLine line, OAMEntry *sprite);
        void clear(bool clearSprites);
        void lock();
        void unlock();
    private:
        PpuRegisters *mRegs;
        Pixel mBGPixs[16];
        Pixel mOBJPixs[16];
        uint8_t mBGHead;
        uint8_t mBGTail;
        uint8_t mBGCount;
        uint8_t mOBJHead;
        uint8_t mOBJTail;
        uint8_t mOBJCount;
        bool mLocked;
};

#endif /* __PIXELFIFO_H__ */
