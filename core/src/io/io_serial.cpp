#include "io_serial.h"
#include "io_regs.h"

IoSerial::IoSerial() {
    mSB = 0;
    mSC = 0;
}

IoSerial::~IoSerial() {
}

uint8_t IoSerial::readAddr(uint16_t addr) {
    switch(addr) {
        case IOREG_SB:
            return mSB;
        case IOREG_SC:
            return mSC; //XXX Will always be zero atm
    }

    return 0;
}

void IoSerial::writeAddr(uint16_t addr, uint8_t val) {
    switch(addr) {
        case IOREG_SB:
            mSB = val;
            break;
        case IOREG_SC:
            mSC = val & IOREG_SC_CARE_MASK;
            if((mSC & IOREG_SC_XFER_START_MASK) == IOREG_SC_START_XFER_VAL) {
                doXfer();
                mSC = 0;
            }
            break;
    }
}

