#include "io_timer.h"

IoTimer::IoTimer(InterruptController *ic) {
    mIC = ic;
    mStarted = false;
    mTIMA = 0;
    mTMA = 0;
    mTAC = 0;
    mTicks = 0;
    mDivider = TIMA_DIVIDER_4_KHZ;
}

IoTimer::~IoTimer() {
}

void IoTimer::writeAddr(uint16_t addr, uint8_t val) {
    switch(addr) {
        case IOREG_TIMA:
            mTIMA = val;
            break;
        case IOREG_TMA:
            mTMA = val;
            break;
        case IOREG_TAC:
            mTAC = val;

            switch(mTAC & IOREG_TAC_STOP_MASK) {
                case IOREG_TAC_STOP:
                    mStarted = false;
                    break;
                case IOREG_TAC_START:
                    mStarted = true;
                    break;
            }

            switch(mTAC & IOREG_TAC_CLOCK_SEL_MASK) {
                case IOREG_TAC_CLOCK_4_KHZ:
                    mDivider = TIMA_DIVIDER_4_KHZ;
                    break;
                case IOREG_TAC_CLOCK_16_KHZ:
                    mDivider = TIMA_DIVIDER_16_KHZ;
                    break;
                case IOREG_TAC_CLOCK_64_KHZ:
                    mDivider = TIMA_DIVIDER_64_KHZ;
                    break;
                case IOREG_TAC_CLOCK_256_KHZ:
                    mDivider = TIMA_DIVIDER_256_KHZ;
                    break;
            }

            break;
    }
}

uint8_t IoTimer::readAddr(uint16_t addr) {
    switch(addr) {
        case IOREG_TIMA:
            return mTIMA;
        case IOREG_TMA:
            return mTMA;
        case IOREG_TAC:
            return mTAC;
    }

    return 0;
}

void IoTimer::tick(uint8_t cycles) {
    if(!mStarted)
        return;

    mTicks += cycles;

    if(mTicks >= mDivider) {
        uint16_t newTIMA = mTIMA + ((uint16_t)mTicks/(uint16_t)mDivider);

        //Check for rollover
        if(newTIMA >= 0xFF) {
            mTIMA = mTMA;
            mIC->requestInterrupt(InterruptController::itTimerOverflow);
        } else
            mTIMA = (uint8_t)newTIMA;

        mTicks %= mDivider;
    }
}
