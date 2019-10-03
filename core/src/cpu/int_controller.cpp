#include "int_controller.h"
#include "io_regs.h"

#define IRQ_TYPE_TO_BIT(_type) (1 << _type)

InterruptController::InterruptController() {
    mEnabled = false;
    mIE = 0;
    mIF = 0;
}

InterruptController::~InterruptController() {
}

void InterruptController::writeAddr(uint16_t addr, uint8_t val) {
    if(addr == IOREG_IE)
        mIE = val;
    else if(addr == IOREG_IF)
        mIF = val;
}

uint8_t InterruptController::readAddr(uint16_t addr) {
    if(addr == IOREG_IE)
        return mIE;
    if(addr == IOREG_IF)
        return mIF;

    return 0;
}

void InterruptController::setEnabled(bool enabled) {
    mEnabled = enabled;
}

bool InterruptController::isEnabled(void) {
    return mEnabled;
}

void InterruptController::requestInterrupt(InterruptType type) {
    if(type == itNone)
        return;

    mIF |= IRQ_TYPE_TO_BIT(type);
}

void InterruptController::clearInterrupt(InterruptType type) {
    if(type == itNone)
        return;

    mIF &= ~IRQ_TYPE_TO_BIT(type);
}

bool InterruptController::isPending(void) {
    return (mIF & mIE);
}

InterruptController::InterruptType InterruptController::getPending() {
    int i;

    if(!mEnabled)
        return itNone;

    for(i = 0; i < itNone; ++i) {
        if(IRQ_TYPE_TO_BIT(i) & mIF)
            return (InterruptType)i;
    }

    return itNone;
}

const uint16_t InterruptController::sISRTable[InterruptController::itNone] = {
    ISR_VBLANK,
    ISR_LCDC_STATUS,
    ISR_TIMER,
    ISR_SERIAL_COMPLETE,
    ISR_PIO
};
