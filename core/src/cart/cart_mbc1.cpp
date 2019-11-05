#include "cart_mbc1.h"

#define CHECK_RAM_ENABLE_RANGE(_addr)         (_addr >= 0x0000 && _addr < 0x2000)
#define CHECK_ROM_BANK_NO_RANGE(_addr)        (_addr >= 0x2000 && _addr < 0x4000)
#define CHECK_ROM_UPPER_RAM_BANK_RANGE(_addr) (_addr >= 0x4000 && _addr < 0x6000)
#define CHECK_MODE_CHANGE_RANGE(_addr)        (_addr >= 0x6000 && _addr < 0x8000)

#define RAM_ENABLE_VAL 0x0a

MBC1::MBC1(uint8_t *buffer, uint32_t sz) : Cartridge(buffer, sz) {
    mMode = mode2MRom8KRam;
    mCurRomBank = 1;
    mCurRamBank = 0;
    mRamBankEnabled = false;
}

void MBC1::writeAddr(uint16_t addr, uint8_t val) {
    if(CHECK_RAM_ENABLE_RANGE(addr) && hasRam())
        mRamBankEnabled = (val & 0x0f) == RAM_ENABLE_VAL;
    else if(CHECK_ROM_BANK_NO_RANGE(addr)) {
        // This sets the lower 5 bits of current bank.
        // Note that these bits CANNOT be all 0. It will always fall to 0x01.
        mCurRomBank = (mCurRomBank & 0x60) | ((val & 0x1f)?(val & 0x1f):0x01);
    } else if(CHECK_ROM_UPPER_RAM_BANK_RANGE(addr)) {
        switch(mMode) {
            case mode2MRom8KRam:
                mCurRomBank = (mCurRomBank & 0x1f) | ((val & 0x03) << 5);
            case mode512KRom32KRam:
                mCurRamBank = val & 0x3;
        }
    } else if(CHECK_MODE_CHANGE_RANGE(addr)) {
        mMode = (val & 0x01)?mode512KRom32KRam:mode2MRom8KRam;
    } else if(CHECK_RAM_RANGE(addr) && mRamBankEnabled) {
        uint32_t offset = GET_RAM_BANK_OFFSET((addr - RAM_BASE), mCurRamBank);

        if(offset < mRamSz)
            mRam[offset] = val;
    }
}

uint8_t MBC1::readAddr(uint16_t addr) {
    if(CHECK_ROM_SLOT0(addr))
        return mRom[addr];

    if(CHECK_ROM_SLOT1(addr)) {
        uint32_t offset = GET_ROM_BANK_OFFSET((addr - ROM_SLOT1_BASE), mCurRomBank);

        if(offset < mRomSz)
            return mRom[offset];
    }

    if(CHECK_RAM_RANGE(addr) && mRamBankEnabled) {
        uint32_t offset = GET_RAM_BANK_OFFSET((addr - RAM_BASE), mCurRamBank);

        if(offset < mRamSz)
            return mRam[offset];
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
