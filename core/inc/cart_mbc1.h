#ifndef __CART_MBC1_H__
#define __CART_MBC1_H__

#include "cartridge.h"

class MBC1 : public Cartridge {
    public:
        MBC1(uint8_t *buffer, uint32_t sz);
        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);
        bool hasRam();

    private:
        typedef enum {
            mode2MRom8KRam,
            mode512KRom32KRam
        } MbcMode;
        uint16_t mCurRomBank;
        uint16_t mCurRamBank;
        MbcMode mMode;
        bool mRamEnabled;

};

#endif /* __CART_MBC1_H__ */
