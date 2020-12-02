#include "io/io_oam_dma.h"
#include "io_regs.h"
#include <stddef.h>
#include "logger.h"

OAMDMA::OAMDMA(Ppu *ppu) {
    mCart = NULL;
    mRAM = NULL;
    mPpu = ppu;
    mActive = false;
    mRestarted = false;
    mStart = 0;
}

OAMDMA::~OAMDMA() {
}

bool OAMDMA::isActive() {
    bool ret = mActive && (mCycles <= 160*4 || mRestarted);
    VLOG(ZONE_DMA, "DMA %s\n", ret?"Active":"Inactive");
    return ret;
}

void OAMDMA::setCart(MemRegion *cart) {
    mCart = cart;
}

void OAMDMA::setRAM(MemRegion *RAM) {
    mRAM = RAM;
}

void OAMDMA::writeAddr(uint16_t addr, uint8_t val) {
    if(addr == IOREG_DMA) {
        if(mActive) {
            VLOG(ZONE_DMA, "DMA Restart\n");
            mRestarted = true;
        }

        VLOG(ZONE_DMA, "DMA Start 0x%02x\n", val);
        mActive = true;
        mPpu->startOAMDMA();
        mStart = val;
        mCycles = 162*4; //XXX
    }
}

uint8_t OAMDMA::readAddr(uint16_t addr) {
    if(addr == IOREG_DMA)
        return mStart;

    return 0;
}

void OAMDMA::tick(uint8_t cycles) {
    if(!mActive)
        return;

    DLOG(ZONE_DMA, "DMA Tick: ,%u, %u\n", cycles, mCycles);

    if(cycles >= mCycles ) {
        mCycles = 0;
        mActive = false;
        mPpu->stopOAMDMA();
        mRestarted = false;

        uint16_t addr = (uint16_t)mStart << 8;
        MemRegion *region;
        if(mStart < 0x80)
            region = mCart;
        else if(mStart < 0xa0)
            region = mPpu;
        else
            region = mRAM;

        for(uint16_t i=0; i<160; ++i) {
            DLOG(ZONE_DMA, "DMA 0x%04x to 0x%04x\n", addr + i, 0xfe00+i);
            mPpu->writeAddrDMA(0xfe00+i, region->readAddr(addr + i));
        }
        VLOG(ZONE_DMA, "DMA Done");

    } else {
        mCycles -= cycles;
    }
}
