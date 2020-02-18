#ifndef IOREGION_H
#define IOREGION_H

#include <stddef.h>
#include "memregion.h"
#include "io_serial.h"
#include "int_controller.h"
#include "io_timer.h"
#include "ppu.h"
#include "io_oam_dma.h"

class IoController : public MemRegion
{
private:
    InterruptController *mIC;
    IoSerial *mSerial;
    IoTimer *mTimer;
    uint8_t mIF;
    uint8_t mDIV;
    uint16_t mDIVticks;
    Ppu *mPpu;
    OAMDMA *mDMA;

    MemRegion *findHandler(uint16_t addr);

public:
    IoController(InterruptController *ic, Ppu *ppu, OAMDMA *dma);
    virtual ~IoController();
    uint8_t readAddr(uint16_t addr);
    void writeAddr(uint16_t addr, uint8_t val);
    void tick(uint8_t cycles);

    void setSerialHandler(IoSerial *handler);
    void setTimer(IoTimer *timer);
};

#endif /* IOREGION_H */
