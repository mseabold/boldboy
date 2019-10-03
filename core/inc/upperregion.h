#ifndef UPPERREGION_H
#define UPPERREGION_H

#include "memregion.h"
#include "ramregion.h"

class UpperRegion : public MemRegion
{
private:
    RamRegion *mEchoedRam;
    RamRegion *mOAM;
    RamRegion *mUpperRam;
    MemRegion *mIO;
    uint8_t mIER;

public:
    UpperRegion(RamRegion *iRam, MemRegion *io);
    ~UpperRegion();
    uint8_t readAddr(uint16_t addr);
    void writeAddr(uint16_t addr, uint8_t val);
};

#endif /* UPPERREGION_H */
