#ifndef __CART_MBC2_H__
#define __CART_MBC2_H__

#include "cartridge.h"

class MBC2 : public Cartridge {
    public:
        MBC2(uint8_t *buffer, uint32_t sz);
        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);
        bool hasRam();

    private:
        uint16_t mCurRomBank;
        bool mRamEnabled;
};



#endif /* __CART_MBC2_H__ */
