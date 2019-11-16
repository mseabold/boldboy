#ifndef __EMPTYREGION_H__
#define __EMPTYREGION_H__

#include "memregion.h"

class EmptyRegion : public MemRegion {
    public:
        EmptyRegion() {}
        ~EmptyRegion() {}

        void writeAddr(uint16_t addr, uint8_t val) {}
        uint8_t readAddr(uint16_t addr) {return 0;}
};

#endif /* __EMPTYREGION_H__ */
