#ifndef UPPERREGION_H
#define UPPERREGION_H

#include "memregion.h"
#include "ramregion.h"
#include "ppu.h"
#include "io/io_oam_dma.h"

class UpperRegion : public MemRegion
{
private:
    RamRegion *mEchoedRam;
    RamRegion *mUpperRam;
    MemRegion *mIO;
    uint8_t mIER;
    Ppu *mPpu;
    OAMDMA *mDMA;

public:
    UpperRegion(RamRegion *iRam, MemRegion *io, Ppu *ppu, OAMDMA *dma);
    ~UpperRegion();
    uint8_t readAddr(uint16_t addr);
    void writeAddr(uint16_t addr, uint8_t val);
};

#endif /* UPPERREGION_H */
