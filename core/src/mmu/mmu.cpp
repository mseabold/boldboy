#include "mmu.h"
#include "ramregion.h"
#include "cartregion.h"
#include "upperregion.h"

#define CART_ROM_REGION_START 0
#define CART_RAM_REGION 5
#define VRAM_REGION 4
#define IRAM_REGION 6
#define UPPER_REGION 7

Mmu::Mmu(Cartridge *cart) {
    /* Initialize all of the memory regions. */
    CartRegion *cartReg = new CartRegion(cart);
    RamRegion *vRam = new RamRegion(0x8000, 0x2000); // 8kB VRAM at 0x8000
    RamRegion *iRam = new RamRegion(0xC000, 0x2000); // 8kB internal RAM at 0xC000
    mIO = new IoRegion();

    /* Upper 8kB contains multiple regions, including IO and echoed iRAM */
    UpperRegion *upper = new UpperRegion(iRam, mIO);


    /* Build out the memory map. We split the map in 8k addressable regions. */

    /* Lower 32K is cartridge fixed/switchable ROM */
    mRegions[CART_ROM_REGION_START] = cartReg;
    mRegions[CART_ROM_REGION_START+1] = cartReg;
    mRegions[CART_ROM_REGION_START+2] = cartReg;
    mRegions[CART_ROM_REGION_START+3] = cartReg;

    /* Next 8K is vRAM */
    mRegions[VRAM_REGION] = vRam;

    /* Next 8K is switchable cartridge RAM */
    mRegions[CART_RAM_REGION] = cartReg;

    /* Next 8K is internal RAM */
    mRegions[IRAM_REGION] = iRam;

    /* Upper 8K is special. */
    mRegions[UPPER_REGION] = upper;

}

Mmu::~Mmu() {
    /* Delete each allocated region. */
    delete mRegions[CART_ROM_REGION_START]; // Cartridge (this takes care of cart RAM)
    delete mRegions[VRAM_REGION];
    delete mRegions[IRAM_REGION];
    delete mRegions[UPPER_REGION];
    delete mIO;
}

uint8_t Mmu::readAddr(uint16_t addr) {
    return mRegions[addr >> 13]->readAddr(addr);
}

void Mmu::writeAddr(uint16_t addr, uint8_t val) {
    mRegions[addr >> 13]->writeAddr(addr, val);
}
