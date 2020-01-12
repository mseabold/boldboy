#ifndef UPPERREGION_H
#define UPPERREGION_H

#include "memregion.h"
#include "ramregion.h"
#include "ppu.h"

class UpperRegion : public MemRegion
{
private:
    RamRegion *mEchoedRam;
    RamRegion *mUpperRam;
    MemRegion *mIO;
    uint8_t mIER;
    Ppu *mPpu;

public:
    UpperRegion(RamRegion *iRam, MemRegion *io, Ppu *ppu);
    ~UpperRegion();
    uint8_t readAddr(uint16_t addr);
    void writeAddr(uint16_t addr, uint8_t val);
};

#endif /* UPPERREGION_H */
