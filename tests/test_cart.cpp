#include "test_cart.h"
#include "mmu.h"

TestCart::TestCart() {
    mRomLen = 0;
    mRom = nullptr;
}

TestCart::TestCart(uint16_t romLen, uint8_t *rom) {
    setRom(romLen, rom);
}

TestCart::~TestCart() {
}

uint8_t TestCart::readAddr(uint16_t addr) {
    if(addr < mRomLen)
        return mRom[addr];

    if(addr >= CART_RAM_BASE && ((addr - CART_RAM_BASE) < 0x2000))
        return mRam[addr-CART_RAM_BASE];

    return 0;
}

void TestCart::writeAddr(uint16_t addr, uint8_t val) {
    //ROM is ROM
    if(addr >= CART_RAM_BASE && ((addr - CART_RAM_BASE) < 0x2000))
        mRam[addr-CART_RAM_BASE] = val;
}

void TestCart::setRom(uint16_t len, uint8_t *rom) {
    mRomLen = len;
    mRom = rom;
}
