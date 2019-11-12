#include "cartridge.h"
#include "cart_rom.h"
#include "cart_mbc1.h"
#include "cart_mbc2.h"
#include "cart_mbc3.h"
#include "cart_mbc5.h"

#include <string.h>

Cartridge::Cartridge(uint8_t *buffer, uint32_t sz) {
    uint8_t hdrRomSz, hdrRamSz;

    if (sz >= CART_HDR_SZ) {
        mType    = buffer[CART_HDR_TYPE];
        hdrRomSz = buffer[CART_HDR_ROMSZ];
        hdrRamSz = buffer[CART_HDR_RAMSZ];
    } else {
        mType = 0;
        hdrRomSz = 0;
        hdrRamSz = 0;
    }

    switch(hdrRomSz) {
        case CART_HDR_ROMSZ_256KBIT:
            mRomSz = CART_256KBIT;
            break;
        case CART_HDR_ROMSZ_512KBIT:
            mRomSz = CART_512KBIT;
            break;
        case CART_HDR_ROMSZ_1MBIT:
            mRomSz = CART_1MBIT;
            break;
        case CART_HDR_ROMSZ_2MBIT:
            mRomSz = CART_2MBIT;
            break;
        case CART_HDR_ROMSZ_4MBIT:
            mRomSz = CART_4MBIT;
            break;
        case CART_HDR_ROMSZ_8MBIT:
            mRomSz = CART_8MBIT;
            break;
        case CART_HDR_ROMSZ_16MBIT:
            mRomSz = CART_16MBIT;
            break;
        case CART_HDR_ROMSZ_9MBIT:
            mRomSz = CART_9MBIT;
            break;
        case CART_HDR_ROMSZ_10MBIT:
            mRomSz = CART_10MBIT;
            break;
        case CART_HDR_ROMSZ_12MBIT:
            mRomSz = CART_12MBIT;
            break;
        default:
            mRomSz = CART_256KBIT;
            break;
    }

    switch(hdrRamSz) {
        case CART_HDR_RAMSZ_16KBIT:
            mRamSz = CART_16KBIT;
            break;
        case CART_HDR_RAMSZ_64KBIT:
            mRamSz = CART_64KBIT;
            break;
        case CART_HDR_RAMSZ_256KBIT:
            mRamSz = CART_256KBIT;
            break;
        case CART_HDR_RAMSZ_1MBIT:
            mRamSz = CART_1MBIT;
            break;
        default:
            mRamSz = 0;
            break;
    }

    if(mRomSz) {
        mRom = new uint8_t[mRomSz];
        memcpy(mRom, buffer, (mRomSz < sz)?mRomSz:sz);

        if(sz < mRomSz)
            memset(&mRom[sz], 0, mRomSz-sz);
    }

    if(mRamSz) {
        mRam = new uint8_t[mRamSz];
    } else
        mRam = NULL;
}

Cartridge::~Cartridge() {
    if(mRom)
        delete[] mRom;

    if(mRam)
        delete[] mRam;
}

bool Cartridge::validate(uint8_t *buffer, uint32_t sz) {
    if(sz < CART_HDR_RAMSZ)
        return false;

    switch(buffer[CART_HDR_ROMSZ]) {
        case CART_HDR_ROMSZ_256KBIT:
        case CART_HDR_ROMSZ_512KBIT:
        case CART_HDR_ROMSZ_1MBIT:
        case CART_HDR_ROMSZ_2MBIT:
        case CART_HDR_ROMSZ_4MBIT:
        case CART_HDR_ROMSZ_8MBIT:
        case CART_HDR_ROMSZ_16MBIT:
        case CART_HDR_ROMSZ_9MBIT:
        case CART_HDR_ROMSZ_10MBIT:
        case CART_HDR_ROMSZ_12MBIT:
            break;
        default:
            return false;
    }

    switch(buffer[CART_HDR_RAMSZ]) {
        case CART_HDR_RAMSZ_16KBIT:
        case CART_HDR_RAMSZ_64KBIT:
        case CART_HDR_RAMSZ_256KBIT:
        case CART_HDR_RAMSZ_1MBIT:
            break;
        default:
            return false;
    }

    switch(buffer[CART_HDR_TYPE]) {
        case CART_HDR_TYPE_ROM_ONLY:
        case CART_HDR_TYPE_MBC1:
        case CART_HDR_TYPE_MBC1_RAM:
        case CART_HDR_TYPE_MBC1_RAM_BATT:
        case CART_HDR_TYPE_MBC2:
        case CART_HDR_TYPE_MBC2_BATT:
        case CART_HDR_TYPE_ROM_RAM:
        case CART_HDR_TYPE_ROM_RAM_BATT:
        case CART_HDR_TYPE_MMM01:
        case CART_HDR_TYPE_MMM01_RAM:
        case CART_HDR_TYPE_MMM91_RAM_BATT:
        case CART_HDR_TYPE_MBC3_RTC:
        case CART_HDR_TYPE_MBC3_RTC_RAM:
        case CART_HDR_TYPE_MBC3:
        case CART_HDR_TYPE_MBC3_RAM:
        case CART_HDR_TYPE_MBC3_RAM_BATT:
        case CART_HDR_TYPE_MBC5:
        case CART_HDR_TYPE_MBC5_RAM:
        case CART_HDR_TYPE_MBC5_RAM_BATT:
        case CART_HDR_TYPE_MBC5_RUMBLE:
        case CART_HDR_TYPE_MBC5_RUMBLE_RAM:
        case CART_HDR_TYPE_MBC5_RUMBLE_RAM_BATT:
        case CART_HDR_TYPE_MBC6:
        case CART_HDR_TYPE_MBC7_ETC:
            break;
        default:
            return false;
    }

    return true;
}

Cartridge *Cartridge::loadFromBuffer(uint8_t *buffer, uint32_t sz) {
    if(!validate(buffer, sz))
        return NULL;

    Cartridge *cart;

    uint8_t type = buffer[CART_HDR_TYPE];

    switch(type) {
        case CART_HDR_TYPE_ROM_ONLY:
        case CART_HDR_TYPE_ROM_RAM:
        case CART_HDR_TYPE_ROM_RAM_BATT:
            cart = new RomOnly(buffer, sz);
            break;
        case CART_HDR_TYPE_MBC1:
        case CART_HDR_TYPE_MBC1_RAM:
        case CART_HDR_TYPE_MBC1_RAM_BATT:
            cart = new MBC1(buffer, sz);
            break;
        case CART_HDR_TYPE_MBC2:
        case CART_HDR_TYPE_MBC2_BATT:
            cart = new MBC2(buffer, sz);
            break;
        case CART_HDR_TYPE_MBC3_RTC:
        case CART_HDR_TYPE_MBC3_RTC_RAM:
        case CART_HDR_TYPE_MBC3:
        case CART_HDR_TYPE_MBC3_RAM:
        case CART_HDR_TYPE_MBC3_RAM_BATT:
            cart = new MBC3(buffer, sz);
            break;
        case CART_HDR_TYPE_MBC5_RAM:
        case CART_HDR_TYPE_MBC5_RAM_BATT:
        case CART_HDR_TYPE_MBC5_RUMBLE:
        case CART_HDR_TYPE_MBC5_RUMBLE_RAM:
        case CART_HDR_TYPE_MBC5_RUMBLE_RAM_BATT:
            cart = new MBC5(buffer, sz);
            break;
        default:
            return NULL;
    }

    return cart;
}
