#include "cart_mbc2.h"

#define MBC2_RAM_SZ 0x0200

#define CHECK_MBC2_RAM_RANGE(_addr)    (_addr >= RAM_BASE && _addr < RAM_BASE+MBC2_RAM_SZ)

MBC2::MBC2(uint8_t *buffer, uint32_t sz) : Cartridge(buffer, sz) {
    mCurRomBank = 1;
    mRamEnabled = false;

    if(mRam != NULL)
        delete[] mRam;

    mRam = new uint8_t[MBC2_RAM_SZ];
    mRamSz = MBC2_RAM_SZ;
}


void MBC2::writeAddr(uint16_t addr, uint8_t val) {
    switch(addr & 0xe000) {
        case 0x0000:
            if((addr & 0x100) == 0)
                mRamEnabled = (val & 0x0f) == CART_RAM_ENABLE_VAL;
            break;
        case 0x2000:
            if((addr & 0x100) == 0x100)
                mCurRomBank = ((val & 0x0f) != 0) ? (val & 0x0f) : 1;
            break;
        case 0xa000:
            if(mRamEnabled && CHECK_MBC2_RAM_RANGE(addr))
                mRam[addr-RAM_BASE] = val & 0x0f;
            break;
    }
}

uint8_t MBC2::readAddr(uint16_t addr) {
    switch(addr & 0xe000) {
        case 0x0000:
            return mRom[addr];
        case 0x4000:
            return readRom(addr, mCurRomBank);
        case 0xa000:
            if(mRamEnabled)
                return mRam[addr - RAM_BASE];

    }

    return 0;
}

bool MBC2::hasRam() {
    return true;
}
