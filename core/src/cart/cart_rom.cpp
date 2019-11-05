#include "cart_rom.h"
#include <string.h>

#define ROMONLY_ROM_SZ 0x8000

RomOnly::RomOnly(uint8_t *buffer, uint32_t sz) : Cartridge(buffer, sz) {
}

RomOnly::~RomOnly() {
}

void RomOnly::writeAddr(uint16_t addr, uint8_t val) {
    // Only RAM can be written.
    if(!hasRam())
        return;

    // Make sure the address is a RAM address and we have enough RAM for it
    if(CHECK_RAM_RANGE(addr) && (addr - RAM_BASE) < mRamSz)
        mRam[addr - RAM_BASE] = val;
}

uint8_t RomOnly::readAddr(uint16_t addr) {
    // Just return the direct ROM mapping
    if(CHECK_ROM_RANGE(addr))
        return mRom[addr];

    // If we have RAM and the address is a valid RAM address, return it
    if(hasRam() && CHECK_RAM_RANGE(addr) && (addr - RAM_BASE) < mRamSz)
        return mRam[addr-RAM_BASE];

    return 0;
}

bool RomOnly::hasRam() {
    switch(mType) {
        case CART_HDR_TYPE_ROM_RAM:
        case CART_HDR_TYPE_ROM_RAM_BATT:
            return true;
    }
    return false;
}
