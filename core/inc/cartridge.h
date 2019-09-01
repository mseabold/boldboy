#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "memregion.h"

class Cartridge : public MemRegion
{
private:

public:
    virtual ~Cartridge() = 0;
};

#endif /* CARTRIDGE_H */
