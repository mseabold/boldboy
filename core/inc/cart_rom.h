#ifndef __CART_ROM_H__
#define __CART_ROM_H__

#include "cartridge.h"

class RomOnly : public Cartridge {
    public:
        RomOnly(uint8_t *buffer, uint32_t sz);
        ~RomOnly();

        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);

        bool hasRam();
};

#endif /* __CART_ROM_H__ */
