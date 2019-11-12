#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "memregion.h"
#include <stddef.h>

#define ROM_BASE     0x0000
#define ROM_SZ       0x8000
#define ROM_SLOT0_BASE 0x0000
#define ROM_SLOT1_BASE 0x4000
#define ROM_BANK_SZ  0x4000

#define RAM_BASE     0xA000
#define RAM_SZ       0x2000
#define RAM_BANK_SZ  0x2000

#define CHECK_ROM_RANGE(_addr) (_addr >= ROM_BASE && _addr < (ROM_BASE+ROM_SZ))
#define CHECK_RAM_RANGE(_addr) (_addr >= RAM_BASE && _addr < (RAM_BASE+RAM_SZ))
#define CHECK_ROM_SLOT0(_addr) (_addr >= ROM_SLOT0_BASE && _addr < ROM_SLOT1_BASE)
#define CHECK_ROM_SLOT1(_addr) (_addr >= ROM_SLOT1_BASE && _addr < ROM_SZ)

#define GET_ROM_BANK_OFFSET(_base, _bank) (_base + _bank * ROM_BANK_SZ)
#define GET_RAM_BANK_OFFSET(_addr, _bank) ((_addr -  RAM_BASE) + (_bank * RAM_BANK_SZ))

#define CART_HDR_TYPE  0x0147
#define CART_HDR_ROMSZ 0x0148
#define CART_HDR_RAMSZ 0x0149
#define CART_HDR_SZ    0x0150

#define CART_HDR_TYPE_ROM_ONLY              0x00
#define CART_HDR_TYPE_MBC1                  0x01
#define CART_HDR_TYPE_MBC1_RAM              0x02
#define CART_HDR_TYPE_MBC1_RAM_BATT         0x03
#define CART_HDR_TYPE_MBC2                  0x05
#define CART_HDR_TYPE_MBC2_BATT             0x06
#define CART_HDR_TYPE_ROM_RAM               0x08
#define CART_HDR_TYPE_ROM_RAM_BATT          0x09
#define CART_HDR_TYPE_MMM01                 0x0b
#define CART_HDR_TYPE_MMM01_RAM             0x0c
#define CART_HDR_TYPE_MMM91_RAM_BATT        0x0d
#define CART_HDR_TYPE_MBC3_RTC              0x0f
#define CART_HDR_TYPE_MBC3_RTC_RAM          0x10
#define CART_HDR_TYPE_MBC3                  0x11
#define CART_HDR_TYPE_MBC3_RAM              0x12
#define CART_HDR_TYPE_MBC3_RAM_BATT         0x13
#define CART_HDR_TYPE_MBC5                  0x19
#define CART_HDR_TYPE_MBC5_RAM              0x1a
#define CART_HDR_TYPE_MBC5_RAM_BATT         0x1b
#define CART_HDR_TYPE_MBC5_RUMBLE           0x1c
#define CART_HDR_TYPE_MBC5_RUMBLE_RAM       0x1d
#define CART_HDR_TYPE_MBC5_RUMBLE_RAM_BATT  0x1e
#define CART_HDR_TYPE_MBC6                  0x20
#define CART_HDR_TYPE_MBC7_ETC              0x22


#define CART_HDR_ROMSZ_256KBIT 0x00
#define CART_HDR_ROMSZ_512KBIT 0x01
#define CART_HDR_ROMSZ_1MBIT   0x02
#define CART_HDR_ROMSZ_2MBIT   0x03
#define CART_HDR_ROMSZ_4MBIT   0x04
#define CART_HDR_ROMSZ_8MBIT   0x05
#define CART_HDR_ROMSZ_16MBIT  0x06
#define CART_HDR_ROMSZ_32MBIT  0x07
#define CART_HDR_ROMSZ_64MBIT  0x08
#define CART_HDR_ROMSZ_9MBIT   0x52
#define CART_HDR_ROMSZ_10MBIT  0x53
#define CART_HDR_ROMSZ_12MBIT  0x54

#define CART_HDR_RAMSZ_NONE    0x00
#define CART_HDR_RAMSZ_16KBIT  0x01
#define CART_HDR_RAMSZ_64KBIT  0x02
#define CART_HDR_RAMSZ_256KBIT 0x03
#define CART_HDR_RAMSZ_1MBIT   0x04

#define CART_16KBIT  2*1024
#define CART_64KBIT  8*1024
#define CART_256KBIT 32*1024
#define CART_512KBIT 64*1024
#define CART_1MBIT   128*1024
#define CART_2MBIT   256*1024
#define CART_4MBIT   512*1024
#define CART_8MBIT   1024*1024
#define CART_16MBIT  2*1024*1024
#define CART_32MBIT  4*1024*1024
#define CART_64MBIT  8*1024*1024
#define CART_9MBIT   9*1024*128
#define CART_10MBIT  10*1024*128
#define CART_12MBIT  16*1024*128

#define CART_RAM_ENABLE_VAL 0x0a

class Cartridge : public MemRegion
{
public:
    ~Cartridge();


    virtual bool hasRam() = 0;

    static bool validate(uint8_t *buffer, uint32_t sz);
    static Cartridge *loadFromBuffer(uint8_t *buffer, uint32_t sz);

protected:
    Cartridge(uint8_t *buffer, uint32_t sz);
    uint8_t *mRom;
    uint32_t mRomSz;
    uint8_t *mRam;
    uint32_t mRamSz;
    uint8_t mType;

    inline uint8_t readRom(uint16_t addr, uint32_t bank) {
        uint32_t offset = GET_ROM_BANK_OFFSET(addr - ROM_SLOT1_BASE, bank);

        if(offset < mRomSz)
            return mRom[offset];

        return 0;
    }

    inline uint8_t readRam(uint16_t addr, uint32_t bank) {
        uint32_t offset = GET_RAM_BANK_OFFSET(addr, bank);

        if(offset < mRamSz)
            return mRam[offset];

        return 0;
    }

    inline void writeRam(uint16_t addr, uint32_t bank, uint8_t val) {
        uint32_t offset = GET_RAM_BANK_OFFSET(addr, bank);

        if(offset < mRamSz)
            mRam[offset] = val;
    }
};

#endif /* CARTRIDGE_H */
