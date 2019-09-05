#include "ioregion.h"
#include "io_regs.h"

IoRegion::IoRegion() {
    mSerial = NULL;
}

IoRegion::~IoRegion() {
}

uint8_t IoRegion::readAddr(uint16_t addr) {
    MemRegion *handler = findHandler(addr);

    if(handler)
        return handler->readAddr(addr);

    return 0;
}

void IoRegion::writeAddr(uint16_t addr, uint8_t val) {
    MemRegion *handler = findHandler(addr);

    if(handler)
        handler->writeAddr(addr, val);
}

void IoRegion::setSerialHandler(IoSerial *handler) {
    mSerial = handler;
}

MemRegion *IoRegion::findHandler(uint16_t addr) {
    //TODO: Eventually this should have the ability to do lookup
    //      on blocks of addressed, but for now we'll explicitly
    //      check for serial
    switch(addr) {
        case IOREG_SB:
        case IOREG_SC:
            return mSerial;
    }

    return NULL;
}
