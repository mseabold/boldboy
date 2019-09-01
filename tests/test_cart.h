#ifndef __TEST_CART_H__
#define __TEST_CART_H__

#include "cartridge.h"

class TestCart : public Cartridge {
    public:
        TestCart();
        TestCart(uint16_t romLen, uint8_t *rom);
        ~TestCart();
        void setRom(uint16_t len, uint8_t *rom);
        uint8_t readAddr(uint16_t addr);
        void writeAddr(uint16_t addr, uint8_t val);
    private:
        uint8_t *mRom;
        uint16_t mRomLen;
        uint8_t mRam[0x2000];
};

#endif /* __TEST_CART_H__ */
