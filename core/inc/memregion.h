#ifndef MEMREGION_H
#define MEMREGION_H

#include <stdint.h>
class MemRegion
{
public:
    virtual ~MemRegion() {}
    virtual uint8_t readAddr(uint16_t addr) = 0;
    virtual void writeAddr(uint16_t addr, uint8_t val) = 0;
};

#endif /* MEMREGION_H */
