#ifndef __BOLDBOY_H__
#define __BOLDBOY_H__

#include "io_regs.h"
#include "io_controller.h"
#include "int_controller.h"
#include "io_timer.h"
#include "io_serial.h"
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

    private:
        Cpu *mCpu;
        Mmu *mMmu;
        Cartridge *mCart;
        IoTimer *mTimer;
        IoController *mIO;
        InterruptController *mIC;
        Ppu *mPpu;

};

#endif /* __BOLDBOY_H__ */
