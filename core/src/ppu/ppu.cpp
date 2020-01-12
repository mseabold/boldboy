#include "ppu.h"
#include "io_regs.h"

#define VRAM_BASE         0x8000

Ppu::Ppu(InterruptController *ic) {
    mIC = ic;
}

Ppu::~Ppu() {
}

void Ppu::writeAddr(uint16_t addr, uint8_t val) {

}

uint8_t Ppu::readAddr(uint16_t addr) {
    return 0;
}

void Ppu::tick(uint8_t cycles) {
}
