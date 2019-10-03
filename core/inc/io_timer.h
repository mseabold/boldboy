#ifndef __IO_TIMER_H__
#define __IO_TIMER_H__

#include "int_controller.h"
#include "memregion.h"
#include "io_regs.h"

#define TIMA_DIVIDER_256_KHZ 16
#define TIMA_DIVIDER_64_KHZ  64
#define TIMA_DIVIDER_16_KHZ  256
#define TIMA_DIVIDER_4_KHZ   1024

class IoTimer : public MemRegion {
    public:
        IoTimer(InterruptController *ic);
        ~IoTimer();

        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);
        void tick(uint8_t cycles);

    private:
        bool mStarted;
        uint8_t mTIMA;
        uint8_t mTMA;
        uint8_t mTAC;
        uint32_t mTicks;
        uint32_t mDivider;
        InterruptController *mIC;
};

#endif /* __IO_TIMER_H__ */
