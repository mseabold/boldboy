#ifndef __CART_MBC3_H__
#define __CART_MBC3_H__

#include "cartridge.h"

class MBC3 : public Cartridge {
    public:
        MBC3(uint8_t *buffer, uint32_t sz);
        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);
        bool hasRam();
        bool hasRTC();

    private:
        uint16_t mCurRomBank;
        uint16_t mCurRamBank;
        bool mRamRtcEnabled;
};

#endif /* __CART_MBC3_H__ */
