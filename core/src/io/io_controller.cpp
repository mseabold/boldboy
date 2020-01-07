#include "io_controller.h"
#include "io_regs.h"


#define TICKS_PER_DIV 256

IoController::IoController(InterruptController *ic) {
    mSerial = NULL;
    mTimer = NULL;
    mIF = 0;
    mDIV = 0;
    mDIVticks = 0;
    mIC = ic;
}

IoController::~IoController() {
}

uint8_t IoController::readAddr(uint16_t addr) {
    MemRegion *handler = findHandler(addr);

    if(handler)
        return handler->readAddr(addr);

    return 0;
}

void IoController::writeAddr(uint16_t addr, uint8_t val) {
    MemRegion *handler = findHandler(addr);

    if(handler)
        handler->writeAddr(addr, val);
}

void IoController::setSerialHandler(IoSerial *handler) {
    mSerial = handler;
}

void IoController::tick(uint8_t cycles) {
    if(mSerial != NULL)
        mSerial->tick(cycles);

    if(mTimer != NULL)
        mTimer->tick(cycles);
}

void IoController::setTimer(IoTimer *timer) {
    mTimer = timer;
}

MemRegion *IoController::findHandler(uint16_t addr) {
    //TODO: Eventually this should have the ability to do lookup
    //      on blocks of addressed, but for now we'll explicitly
    //      check for serial
    switch(addr) {
        case IOREG_SB:
        case IOREG_SC:
            return mSerial;
        case IOREG_IF:
        case IOREG_IE:
            return mIC;
        case IOREG_DIV:
        case IOREG_TIMA:
        case IOREG_TMA:
        case IOREG_TAC:
            return mTimer;
    }

    return NULL;
}

