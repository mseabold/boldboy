#ifndef __CART_MBC5_H__
#define __CART_MBC5_H__

#include "cartridge.h"

class MBC5 : public Cartridge {
    public:
        MBC5(uint8_t *buffer, uint32_t sz);
        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);
        bool hasRam();

    private:
        uint16_t mCurRomBank;
        uint16_t mCurRamBank;
        bool mRamEnabled;
};

#endif /* __CART_MBC5_H__ */
