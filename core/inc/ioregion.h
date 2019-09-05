#ifndef IOREGION_H
#define IOREGION_H

#include <stddef.h>
#include "memregion.h"
#include "io_serial.h"

class IoRegion : public MemRegion
{
private:
    IoSerial *mSerial;
    MemRegion *findHandler(uint16_t addr);

public:
    IoRegion();
    virtual ~IoRegion();
    uint8_t readAddr(uint16_t addr);
    void writeAddr(uint16_t addr, uint8_t val);

    void setSerialHandler(IoSerial *handler);
};

#endif /* IOREGION_H */
