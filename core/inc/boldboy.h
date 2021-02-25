#ifndef __BOLDBOY_H__
#define __BOLDBOY_H__

#include "io_regs.h"
#include "io/io_controller.h"
#include "int_controller.h"
#include "io/io_timer.h"
#include "io/io_serial.h"
#include "cartridge.h"
#include "cpu.h"
#include "mmu.h"
#include "ppu.h"
#include "logger.h"

class Boldboy {
    public:
        Boldboy();
        Boldboy(bool useBootrom);
        ~Boldboy();

        bool loadCart(const char *filename);
        void tick();
        void run();
        void setSerialHandler(IoSerial *serial);
        void setLogger(Logger *logger);
        void waitForFrame();
        void getFrame(uint8_t frame[144][160]);
        void buttonPressed(IoJoypad::Button button);
        void buttonReleased(IoJoypad::Button button);

#ifdef ENABLE_TEST_HARNESS
        Cpu *getCpu();
#endif

    private:
        Cpu *mCpu;
        Mmu *mMmu;
        Cartridge *mCart;
        IoTimer *mTimer;
        IoController *mIO;
        InterruptController *mIC;
        Ppu *mPpu;
        OAMDMA *mDMA;
        IoJoypad *mJoypad;

};

#endif /* __BOLDBOY_H__ */
