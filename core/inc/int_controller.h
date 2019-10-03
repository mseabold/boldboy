#ifndef __INT_CONTROLLER_H__
#define __INT_CONTROLLER_H__

#include "memregion.h"

#define ISR_VBLANK          0x0040
#define ISR_LCDC_STATUS     0x0048
#define ISR_TIMER           0x0050
#define ISR_SERIAL_COMPLETE 0x0058
#define ISR_PIO             0x0060

class InterruptController : public MemRegion {
    public:
        enum InterruptType {
            itVBlank,
            itLCDCStatus,
            itTimerOverflow,
            itSerialComplete,
            itPIO,
            itNone
        };

        static const uint16_t sISRTable[itNone];

        InterruptController();
        ~InterruptController();

        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);

        void setEnabled(bool enabled);
        bool isEnabled(void);
        void requestInterrupt(InterruptType type);
        void clearInterrupt(InterruptType type);
        bool isPending(void);
        InterruptType getPending(void);


    private:
        bool mEnabled;
        uint8_t mIF;
        uint8_t mIE;
};

#endif /* __INT_CONTROLLER_H__ */
