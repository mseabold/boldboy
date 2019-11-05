#include "cart_mbc2.h"

#define MBC2_RAM_SZ 0x0200

#define CHECK_RAM_ENABLE_RANGE(_addr)  (_addr >= 0x0000 && _addr < 0x2000)
#define CHECK_ROM_BANK_NO_RANGE(_addr) (_addr >= 0x2000 && _addr < 0x4000)
#define CHECK_MBC2_RAM_RANGE(_addr)    (_addr >= RAM_BASE && _addr < RAM_BASE+MBC2_RAM_SZ)

#define RAM_ENABLE_VAL 0x0a

MBC2::MBC2(uint8_t *buffer, uint32_t sz) : Cartridge(buffer, sz) {
    mCurRomBank = 1;
    mRamEnabled = false;

    if(mRam != NULL)
        delete[] mRam;

    mRam = new uint8_t[MBC2_RAM_SZ];
    mRamSz = MBC2_RAM_SZ;
}


void MBC2::writeAddr(uint16_t addr, uint8_t val) {
    if(CHECK_RAM_ENABLE_RANGE(addr) && ((addr & 0x100) == 0))
        mRamEnabled = (val & 0x0f) == RAM_ENABLE_VAL;
    else if(CHECK_ROM_BANK_NO_RANGE(addr) && ((addr & 0x100) == 0x100))
        mCurRomBank = ((val & 0x0f) != 0) ? (val & 0x0f) : 1;
    else if(CHECK_MBC2_RAM_RANGE(addr) && mRamEnabled)
        mRam[addr-RAM_BASE] = val & 0x0f;
}

uint8_t MBC2::readAddr(uint16_t addr) {
    if(CHECK_ROM_SLOT0(addr))
        return mRom[addr];
    else if(CHECK_ROM_SLOT1(addr)) {
        uint32_t offset = GET_ROM_BANK_OFFSET((addr - ROM_SLOT1_BASE), mCurRomBank);

        if(offset < mRomSz)
            return mRom[offset];
    } else if(CHECK_MBC2_RAM_RANGE(addr) && mRamEnabled) {
        return mRam[addr - RAM_BASE];
    }

    return 0;
}

bool MBC2::hasRam() {
    return true;
}
