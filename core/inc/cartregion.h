#ifndef CARTREGION_H
#define CARTREGION_H

#include "memregion.h"
#include "cartridge.h"

class CartRegion : public MemRegion
{
private:
    Cartridge *mCart;

public:
    CartRegion(Cartridge *cart);
    ~CartRegion();
    uint8_t readAddr(uint16_t addr);
    void writeAddr(uint16_t addr, uint8_t val);
};

#endif /* CARTREGION_H */
