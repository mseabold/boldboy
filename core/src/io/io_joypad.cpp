#include "io/io_joypad.h"
#include "logger.h"

IoJoypad::IoJoypad(InterruptController *ic) : mIC(ic) {
    mMask = 0;
    mSelection = 0; //TODO What is the initial state of the P1 register?
}

void IoJoypad::writeAddr(uint16_t addr, uint8_t val) {
    if(addr == IOREG_P1) {
        //TODO: Should this be delayed but some number of cycles?
        mSelection = val & IOREG_P1_OUT_MASK;
    }
}

uint8_t IoJoypad::readAddr(uint16_t addr) {
    uint8_t val = mSelection | IOREG_P1_IN_MASK;
    if(addr == IOREG_P1) {
        // Remember: all these pins are active low!
        if(!(mSelection & IOREG_P1_OUT_P14)) {
            val ^= mMask & JOYPAD_BMASK_P14_MASK;
        }

        if(!(mSelection & IOREG_P1_OUT_P15)) {
            val ^= (mMask & JOYPAD_BMASK_P15_MASK) >> JOYPAD_BMASK_P15_SHIFT;
        }

        return val;
    }
    return 0xff;
}

void IoJoypad::buttonPressed(Button b) {
    // TODO Interrupts
    mMask |= b;
}

void IoJoypad::buttonReleased(Button b) {
    mMask &= ~b;
}
