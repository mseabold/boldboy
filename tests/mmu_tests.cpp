#include <catch2/catch.hpp>
#include "mmu.h"
#include "test_cart.h"

static uint8_t cartRom[] = {0xaa, 0xbb, 0xcc, 0xdd};

static inline void writeBuffer(Mmu* mmu, uint16_t base, uint16_t len, uint8_t *buffer) {
    for(uint16_t i=0; i<len; ++i)
        mmu->writeAddr(base+i, buffer[i]);
}

TEST_CASE("Read from Cart ROM", "[mmu]") {
    Cartridge *cart = new TestCart(sizeof(cartRom), cartRom);
    Mmu *mmu = new Mmu(cart, NULL);

    REQUIRE(mmu->readAddr(0) == cartRom[0]);
    REQUIRE(mmu->readAddr(1) == cartRom[1]);
    REQUIRE(mmu->readAddr(2) == cartRom[2]);
    REQUIRE(mmu->readAddr(3) == cartRom[3]);
}

TEST_CASE("Write then read from Internal RAM", "[mmu]") {
    Mmu *mmu = new Mmu(NULL, NULL);
    writeBuffer(mmu, INTERNAL_RAM_BASE, sizeof(cartRom), cartRom);

    REQUIRE(mmu->readAddr(INTERNAL_RAM_BASE+0) == cartRom[0]);
    REQUIRE(mmu->readAddr(INTERNAL_RAM_BASE+1) == cartRom[1]);
    REQUIRE(mmu->readAddr(INTERNAL_RAM_BASE+2) == cartRom[2]);
    REQUIRE(mmu->readAddr(INTERNAL_RAM_BASE+3) == cartRom[3]);
}

TEST_CASE("Write then read from Video RAM", "[mmu]") {
    Mmu *mmu = new Mmu(NULL, NULL);
    writeBuffer(mmu, VRAM_BASE, sizeof(cartRom), cartRom);

    REQUIRE(mmu->readAddr(VRAM_BASE+0) == cartRom[0]);
    REQUIRE(mmu->readAddr(VRAM_BASE+1) == cartRom[1]);
    REQUIRE(mmu->readAddr(VRAM_BASE+2) == cartRom[2]);
    REQUIRE(mmu->readAddr(VRAM_BASE+3) == cartRom[3]);
}

TEST_CASE("Write to echoed RAM then read from Internal RAM", "[mmu]") {
    Mmu *mmu = new Mmu(NULL, NULL);
    writeBuffer(mmu, RAM_ECHO_BASE, sizeof(cartRom), cartRom);

    REQUIRE(mmu->readAddr(INTERNAL_RAM_BASE+0) == cartRom[0]);
    REQUIRE(mmu->readAddr(INTERNAL_RAM_BASE+1) == cartRom[1]);
    REQUIRE(mmu->readAddr(INTERNAL_RAM_BASE+2) == cartRom[2]);
    REQUIRE(mmu->readAddr(INTERNAL_RAM_BASE+3) == cartRom[3]);
}

TEST_CASE("Write to internal RAM then read from echoed RAM", "[mmu]") {
    Mmu *mmu = new Mmu(NULL, NULL);
    writeBuffer(mmu, INTERNAL_RAM_BASE, sizeof(cartRom), cartRom);

    REQUIRE(mmu->readAddr(RAM_ECHO_BASE+0) == cartRom[0]);
    REQUIRE(mmu->readAddr(RAM_ECHO_BASE+1) == cartRom[1]);
    REQUIRE(mmu->readAddr(RAM_ECHO_BASE+2) == cartRom[2]);
    REQUIRE(mmu->readAddr(RAM_ECHO_BASE+3) == cartRom[3]);
}
