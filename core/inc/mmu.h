#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stddef.h>
#include "memregion.h"
#include "cartridge.h"
#include "upperregion.h"
#include "emptyregion.h"
#include "ppu.h"
#include "io/io_oam_dma.h"

#define MMU_NUM_MEM_REGIONS 8

#define CART_ROM_BASE     0x0000
#define VRAM_BASE         0x8000
#define CART_RAM_BASE     0xA000
#define INTERNAL_RAM_BASE 0xC000
#define RAM_ECHO_BASE     0xE000
#define OAM_BASE          0xFE00
#define IO_BASE           0xFF00
#define UPPER_RAM_BASE    0xFF80
#define IER_BASE          0xFFFF
class Mmu
{
private:
    MemRegion *mRegions[MMU_NUM_MEM_REGIONS];
    bool mBootromEnabled;
    UpperRegion *mUpper;
    EmptyRegion *mEmpty;
    OAMDMA *mDMA;

    static const uint8_t sBootROM[256];

public:
    Mmu(MemRegion *io, Ppu *ppu, OAMDMA *dma);
    uint8_t readAddr(uint16_t addr);
    void writeAddr(uint16_t addr, uint8_t val);
    void reset(void);
    void loadCart(Cartridge *cart);
    void enableBootrom(bool enabled);
    ~Mmu();
};

#endif /* MMU_H */
