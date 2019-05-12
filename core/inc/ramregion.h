#ifndef RAMREGION_H
#define RAMREGION_H

#include "memregion.h"
class RamRegion : public MemRegion
{
private:
    uint8_t *mRam;
    uint16_t mSize;
    uint16_t mBaseAddr;

public:
    RamRegion(uint16_t baseAddr, uint16_t size);
    ~RamRegion();

    uint8_t readAddr(uint16_t addr);
    void writeAddr(uint16_t addr, uint8_t val);
};

#endif /* RAMREGION_H */
