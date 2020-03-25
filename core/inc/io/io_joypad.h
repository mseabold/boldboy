#ifndef __IO_JOYPAD_H__
#define __IO_JOYPAD_H__

#include "memregion.h"
#include "int_controller.h"
#include "io_regs.h"

#define JOYPAD_BMASK_P15_SHIFT 4
#define JOYPAD_BMASK_P14_MASK  0x0F
#define JOYPAD_BMASK_P15_MASK  (0x0F << JOYPAD_BMASK_P15_SHIFT)

class IoJoypad : public MemRegion {
    public:
        typedef enum {
            RIGHT  = IOREG_P1_IN_RIGHT,
            LEFT   = IOREG_P1_IN_LEFT,
            UP     = IOREG_P1_IN_UP,
            DOWN   = IOREG_P1_IN_DOWN,
            A      = IOREG_P1_IN_A << JOYPAD_BMASK_P15_SHIFT,
            B      = IOREG_P1_IN_B << JOYPAD_BMASK_P15_SHIFT,
            SELECT = IOREG_P1_IN_SELECT << JOYPAD_BMASK_P15_SHIFT,
            START  = IOREG_P1_IN_START << JOYPAD_BMASK_P15_SHIFT
        } Button;

        IoJoypad(InterruptController *ic);

        void buttonPressed(Button b);
        void buttonReleased(Button b);

        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);

    private:
        InterruptController *mIC;
        uint8_t mMask;
        uint8_t mSelection;
};

#endif /* __IO_JOYPAD_H__ */
