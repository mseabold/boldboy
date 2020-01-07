#include "io_timer.h"
#include "logger.h"

IoTimer::IoTimer(InterruptController *ic) {
    mIC = ic;
    mStarted = false;
    mTIMA = 0;
    mTMA = 0;
    mTAC = 0;
    mTicks = 0;
    mSysTick = 0;
    mMuxBit = 0;
}

IoTimer::~IoTimer() {
}

void IoTimer::writeAddr(uint16_t addr, uint8_t val) {
    switch(addr) {
        case IOREG_DIV:
            mSysTick = 0;
            //TODO Handle TIMA gltiching
            break;
        case IOREG_TIMA:
            VLOG("Write TIMA: %u\n", val);
            mTIMA = val;
            break;
        case IOREG_TMA:
            VLOG("Write TMA: %u\n", val);
            mTMA = val;
            break;
        case IOREG_TAC:
            mTAC = val;

            switch(mTAC & IOREG_TAC_STOP_MASK) {
                case IOREG_TAC_STOP:
                    mStarted = false;
                    break;
                case IOREG_TAC_START:
                    VLOG("%s\n", "Timer Started");
                    mStarted = true;
                    break;
            }

            switch(mTAC & IOREG_TAC_CLOCK_SEL_MASK) {
                case IOREG_TAC_CLOCK_4_KHZ:
                    mMuxBit = TIMA_BIT_4_KHZ;
                    break;
                case IOREG_TAC_CLOCK_16_KHZ:
                    mMuxBit = TIMA_BIT_16_KHZ;
                    break;
                case IOREG_TAC_CLOCK_64_KHZ:
                    mMuxBit = TIMA_BIT_64_KHZ;
                    break;
                case IOREG_TAC_CLOCK_256_KHZ:
                    mMuxBit = TIMA_BIT_256_KHZ;
                    break;
            }

            break;
    }
}

uint8_t IoTimer::readAddr(uint16_t addr) {
    switch(addr) {
        case IOREG_DIV:
            return (mSysTick >> 8);
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
    uint16_t oldSysTick = mSysTick;

    DLOG("Timer Tick %u cycles\n", cycles);

    // If the timer isn't running, we can just update systick
    if(!mStarted) {
        mSysTick += cycles;
        VLOG("SysTick: 0x%04x\n", mSysTick);
        VLOG("TIMA: 0x%04x\n", mTIMA);
        return;
    }

    while(cycles > 0) {
        mSysTick += 1;

        DLOG("xored: 0x%04x, mux bit: 0x%04x\n", (oldSysTick ^ mSysTick), (mSysTick & mMuxBit));

        if((((oldSysTick ^ mSysTick) & mMuxBit) != 0) && ((mSysTick & mMuxBit) == 0)) {
            ++mTIMA;

            // Check for rollover
            if(mTIMA == 0) {
                //TODO: Delay this 1 cycle
                mTIMA = mTMA;
                mIC->requestInterrupt(InterruptController::itTimerOverflow);
            }
        }

        oldSysTick = mSysTick;
        --cycles;
    }

    VLOG("SysTick: 0x%04x\n", mSysTick);
    VLOG("TIMA: 0x%04x\n", mTIMA);
    VLOG("TIMA DIV: %u\n", mSysTick % (mMuxBit << 1));
}
