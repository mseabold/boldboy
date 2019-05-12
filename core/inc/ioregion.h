#ifndef IOREGION_H
#define IOREGION_H

#include "memregion.h"

class IoRegion : public MemRegion
{
private:
    

public:
    IoRegion();
    virtual ~IoRegion();
    uint8_t readAddr(uint16_t addr);
    void writeAddr(uint16_t addr, uint8_t val);
};

#endif /* IOREGION_H */
