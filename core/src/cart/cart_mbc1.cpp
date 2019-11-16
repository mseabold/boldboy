#include "cart_mbc1.h"

MBC1::MBC1(uint8_t *buffer, uint32_t sz) : Cartridge(buffer, sz) {
    mMode = mode2MRom8KRam;
    mCurRomBank = 1;
    mCurRamBank = 0;
    mRamEnabled = false;
}

void MBC1::writeAddr(uint16_t addr, uint8_t val) {
    switch(addr & 0xe000) {
        case 0x0000:
            if(hasRam())
                mRamEnabled = (val & 0x0f) == CART_RAM_ENABLE_VAL;
            break;
        case 0x2000:
            // This sets the lower 5 bits of current bank.
            // Note that these bits CANNOT be all 0. It will always fall to 0x01.
            mCurRomBank = (mCurRomBank & 0x60) | ((val & 0x1f)?(val & 0x1f):0x01);
            break;
        case 0x4000:
            switch(mMode) {
                case mode2MRom8KRam:
                    mCurRomBank = (mCurRomBank & 0x1f) | ((val & 0x03) << 5);
                    break;
                case mode512KRom32KRam:
                    mCurRamBank = val & 0x3;
                    break;
            }
            break;
        case 0x6000:
            mMode = (val & 0x01)?mode512KRom32KRam:mode2MRom8KRam;
            break;
        case 0xa000:
            writeRam(addr, mCurRamBank, val);
            break;
    }
}

uint8_t MBC1::readAddr(uint16_t addr) {
    switch(addr & 0xe000) {
        case 0x0000:
        case 0x2000:
            return mRom[addr];
        case 0x4000:
        case 0x6000:
            return readRom(addr, mCurRomBank);
        case 0xa000:
            if(mRamEnabled)
                return readRam(addr, mCurRamBank);
    }

    return 0;
}

bool MBC1::hasRam() {
    switch(mType) {
        case CART_HDR_TYPE_MBC1_RAM:
        case CART_HDR_TYPE_MBC1_RAM_BATT:
            return true;
    }
    return false;
}
