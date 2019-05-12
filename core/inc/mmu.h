#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include "memregion.h"
#include "cartridge.h"
#include "ioregion.h"

#define MMU_NUM_MEM_REGIONS 8
class Mmu
{
private:
    MemRegion *mRegions[MMU_NUM_MEM_REGIONS];
    IoRegion *mIO;

public:
    Mmu(Cartridge *cart);
    uint8_t readAddr(uint16_t addr);
    void writeAddr(uint16_t addr, uint8_t val);
    virtual ~Mmu();
};

#endif /* MMU_H */
