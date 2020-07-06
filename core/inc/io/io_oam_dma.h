#ifndef __IO_OAM_DMA_H__
#define __IO_OAM_DMA_H__

#include "memregion.h"

class OAMDMA : public MemRegion {
    public:
        OAMDMA(MemRegion *VRAM, MemRegion *OAM);
        ~OAMDMA();
        bool isActive();
        void setCart(MemRegion *cart);
        void setRAM(MemRegion *RAM);

        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);

        void tick(uint8_t cycles);
    private:
        MemRegion *mCart;
        MemRegion *mRAM;
        MemRegion *mVRAM;
        MemRegion *mOAM;
        bool mActive;
        bool mRestarted;
        uint8_t mStart;
        uint16_t mCycles;
};

#endif /* __IO_OAM_DMA_H__ */
