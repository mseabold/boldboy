#ifndef __IO_SERIAL_H__
#define __IO_SERIAL_H__

#include "memregion.h"

#define IOREG_SC_XFER_START_MASK 0x80
#define IOREG_SC_START_XFER_VAL  0x80
#define IOREG_SC_NON_XFER_VAL    0x00

#define IOREG_SC_SHIFT_CLK_MASK   0x01
#define IOREG_SC_EXTERNAL_CLK_VAL 0x00
#define IOREG_SC_INTERNAL_CLK_VAL 0x01

#define IOREG_SC_CARE_MASK (IOREG_SC_XFER_START_MASK | IOREG_SC_SHIFT_CLK_MASK)

class IoSerial : public MemRegion {
    public:
        IoSerial();
        ~IoSerial();

        uint8_t readAddr(uint16_t addr);
        void writeAddr(uint16_t addr, uint8_t val);
        void tick(uint8_t cycles);

        virtual void doXfer() = 0;

    protected:
        /* TODO: This probably won't really be used, since what we actually need is a tick-based
         * timed state that fires when a transfer is completed. */
        uint8_t mSC;
        uint8_t mSB;

        uint8_t cpuTicks;
        uint8_t bitTicks;
};

#endif /* __IO_SERIAL_H__ */
