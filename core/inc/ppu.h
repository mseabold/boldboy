#ifndef __PPU_H__
#define __PPU_H__

#include "memregion.h"
#include "int_controller.h"

class Ppu : public MemRegion {
    public:
        Ppu(InterruptController *ic);
        ~Ppu();

        void writeAddr(uint16_t addr, uint8_t val);
        uint8_t readAddr(uint16_t addr);
        void tick(uint8_t cycles);
    private:
        uint8_t mVRAM[0x2000];
        InterruptController *mIC;
};

#endif /* __PPU_H__ */
