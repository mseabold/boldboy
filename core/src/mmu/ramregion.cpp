#include "ramregion.h"

#define REGION_CONTAINS(_x) (addr >= mBaseAddr && addr < (mBaseAddr + mSize))
RamRegion::RamRegion(uint16_t baseAddr, uint16_t size) {
    mRam = new uint8_t[size];
    mBaseAddr = baseAddr;
    mSize = size;
}

RamRegion::~RamRegion() {
    delete mRam;
}

uint8_t RamRegion::readAddr(uint16_t addr) {
    if(REGION_CONTAINS(addr))
        return mRam[addr - mBaseAddr];
    else
        return 0;
}

void RamRegion::writeAddr(uint16_t addr, uint8_t val) {
    if(REGION_CONTAINS(addr))
        mRam[addr - mBaseAddr] = val;
}
