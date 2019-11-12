#include "cart_mbc3.h"

#define MBC3_RTC_REG_SECONDS        0x08
#define MBC3_RTC_REG_MINUTES        0x09
#define MBC3_RTC_REG_HOURS          0x0a
#define MBC3_RTC_REG_DAY_LOW        0x0b
#define MBC3_RTC_REG_DAY_HIGH_FLAGS 0x0c

MBC3::MBC3(uint8_t *buffer, uint32_t sz) : Cartridge(buffer, sz) {
    mCurRomBank = 1;
    mCurRamBank = 0;
    mRamRtcEnabled = false;
}

void MBC3::writeAddr(uint16_t addr, uint8_t val) {
    switch(addr & 0xe000) {
        case 0x0000:
            if(hasRam() && hasRTC())
                mRamRtcEnabled = ((val & 0x0f) == CART_RAM_ENABLE_VAL);
            break;
        case 0x2000:
            mCurRomBank = val & 0x7f;

            if(mCurRomBank == 0)
                mCurRomBank =1;

            break;
        case 0x4000:
            mCurRamBank = (val & 0x1f);
            break;
        case 0xa000:
            if(mCurRamBank < MBC3_RTC_REG_SECONDS) {
                writeRam(addr, mCurRamBank, val);
            } else {
                //TODO Handle RTC
            }
            break;
    }
}

uint8_t MBC3::readAddr(uint16_t addr) {
    switch(addr & 0xe000) {
        case 0x0000:
            return mRom[addr];
        case 0x4000:
            return readRom(addr, mCurRomBank);
        case 0xa000:
            if(mCurRamBank < MBC3_RTC_REG_SECONDS) {
                return readRam(addr, mCurRamBank);
            } else {
                //TODO Handle RTC
            }
            break;

    }

    return 0;
}

bool MBC3::hasRam() {
    switch(mType) {
        case CART_HDR_TYPE_MBC3_RAM:
        case CART_HDR_TYPE_MBC3_RAM_BATT:
        case CART_HDR_TYPE_MBC3_RTC_RAM:
            return true;
    }

    return false;
}

bool MBC3::hasRTC() {
    switch(mType) {
        case CART_HDR_TYPE_MBC3_RTC:
        case CART_HDR_TYPE_MBC3_RTC_RAM:
            return true;
    }

    return false;
}
