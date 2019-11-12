#include "cart_mbc5.h"

MBC5::MBC5(uint8_t *buffer, uint32_t sz) : Cartridge(buffer, sz) {
    mCurRomBank = 1;
    mCurRamBank = 0;
    mRamEnabled = false;
}

void MBC5::writeAddr(uint16_t addr, uint8_t val) {
    switch(addr & 0xe000) {
        case 0x0000:
            if(hasRam())
                mRamEnabled = ((val & 0x0f) == CART_RAM_ENABLE_VAL);
            break;
        case 0x2000:
            if(addr < 0x3000)
                mCurRomBank = (mCurRomBank & 0x100) | val;
            else
                mCurRomBank = (mCurRomBank & 0xff) | ((val & 0x01) << 8);
            break;
        case 0x4000:
            mCurRamBank = val & 0x0f;
            break;
        case 0xa000:
            if(mRamEnabled)
                writeRam(addr, mCurRamBank, val);
            break;
    }
}

uint8_t MBC5::readAddr(uint16_t addr) {
    switch(addr & 0xe000) {
        case 0x0000:
            return mRom[addr];
        case 0x4000:
            return readRom(addr, mCurRomBank);
        case 0xa000:
            return readRam(addr, mCurRamBank);
    }
    return 0;
}

bool MBC5::hasRam() {
    switch(mType) {
        case CART_HDR_TYPE_MBC5_RAM:
        case CART_HDR_TYPE_MBC5_RAM_BATT:
        case CART_HDR_TYPE_MBC5_RUMBLE_RAM:
        case CART_HDR_TYPE_MBC5_RUMBLE_RAM_BATT:
            return true;
    }

    return false;
}
