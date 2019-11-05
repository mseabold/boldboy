#include <catch2/catch.hpp>
#include "cart_rom.h"
#include "cart_mbc1.h"
#include "cart_mbc2.h"

#define CALC_ROM_SZ(_banks) (_banks * 0x4000)
#define READ_U16(_readable, _addr) ((_readable->readAddr(_addr)) + (_readable->readAddr(_addr+1) << 8))
#define BANK_START(_bank) (_bank * 0x4000)

static uint8_t *createRom(uint16_t banks, uint8_t type, uint8_t romSz, uint8_t ramSz) {
    if(banks < 2)
        return NULL;

    uint32_t sz = CALC_ROM_SZ(banks);
    uint8_t *rom = new uint8_t[sz];

    for(int i = 0; i < banks; i++) {
        // Put the bank number in the first 2 bytes
        rom[i*0x4000] =  i & 0xff;
        rom[i*0x4000+1] =  i >> 8;

        // Fill the rest of the bank with just an increasing byte sequence
        for(int j = 2; j < 0x4000; j++) {
            rom[i*0x4000+j] = j % 256;
        }
    }

    rom[CART_HDR_TYPE] = type;
    rom[CART_HDR_ROMSZ] = romSz;
    rom[CART_HDR_RAMSZ] = ramSz;

    return rom;
}

TEST_CASE("Rom Only Bank Test", "[cart]") {
    uint8_t *rom = createRom(2, CART_HDR_TYPE_ROM_ONLY, CART_HDR_ROMSZ_256KBIT, CART_HDR_RAMSZ_NONE);

    Cartridge *cart = new RomOnly(rom, CALC_ROM_SZ(2));

    SECTION("Bank Test") {
        REQUIRE(READ_U16(cart, BANK_START(0)) == 0x0000);
        REQUIRE(READ_U16(cart, BANK_START(1)) == 0x0001);
    }

    delete rom;
    delete cart;

}

TEST_CASE("Rom Only Write Test", "[cart]") {
    uint8_t *rom = createRom(2, CART_HDR_TYPE_ROM_ONLY, CART_HDR_ROMSZ_256KBIT, CART_HDR_RAMSZ_NONE);

    Cartridge *cart = new RomOnly(rom, CALC_ROM_SZ(2));

    SECTION("Write Test") {
        uint8_t val = cart->readAddr(0x1111);
        cart->writeAddr(0x1111, val+1);
        REQUIRE(cart->readAddr(0x1111) == val);
    }

    delete rom;
    delete cart;
}

TEST_CASE("Ram Only hasRam False", "[cart]") {
    uint8_t *rom = createRom(2, CART_HDR_TYPE_ROM_ONLY, CART_HDR_ROMSZ_256KBIT, CART_HDR_RAMSZ_NONE);

    Cartridge *cart = new RomOnly(rom, CALC_ROM_SZ(2));

    SECTION("Has Ram") {
        REQUIRE(cart->hasRam() == false);
    }

    delete rom;
    delete cart;
}

TEST_CASE("Rom Cart w/ RAM hasRam", "[cart]") {
    uint8_t *rom = createRom(2, CART_HDR_TYPE_ROM_RAM, CART_HDR_ROMSZ_256KBIT, CART_HDR_RAMSZ_64KBIT);

    Cartridge *cart = new RomOnly(rom, CALC_ROM_SZ(2));

    SECTION("Has Ram") {
        REQUIRE(cart->hasRam() == true);
    }

    delete rom;
    delete cart;
}

TEST_CASE("Rom Cart w/ RAM Read/Write", "[cart]") {
    uint8_t *rom = createRom(2, CART_HDR_TYPE_ROM_RAM, CART_HDR_ROMSZ_256KBIT, CART_HDR_RAMSZ_64KBIT);

    Cartridge *cart = new RomOnly(rom, CALC_ROM_SZ(2));

    SECTION("Read/Write") {
        cart->writeAddr(0xA000, 0xaa);
        REQUIRE(cart->readAddr(0xA000) == 0xaa);
        cart->writeAddr(0xA000, 0xbb);
        REQUIRE(cart->readAddr(0xA000) == 0xbb);
    }

    delete rom;
    delete cart;
}

TEST_CASE("MBC1 ROM Bank Test 2M/8K", "[cart]") {
    uint8_t *rom = createRom(64, CART_HDR_TYPE_MBC1, CART_HDR_ROMSZ_8MBIT, CART_HDR_RAMSZ_NONE);

    Cartridge *cart = new MBC1(rom, CALC_ROM_SZ(64));

    SECTION("Bank 0") {
        // Select bank 0, which should actually select bank 1
        cart->writeAddr(0x2000, 0x00);
        // Check our bank ID for bank 1
        REQUIRE(READ_U16(cart, 0x4000) == 0x0001);
    }

    SECTION("Bank 3") {
        cart->writeAddr(0x2000, 0x03);
        REQUIRE(READ_U16(cart, 0x4000) == 0x0003);
    }

    SECTION("Bank 8") {
        //Should go to bank 9 since bit0 is 0
        cart->writeAddr(0x2000, 0x08);
        REQUIRE(READ_U16(cart, 0x4000) == 0x0008);
    }

    SECTION("Bank 33") {
        // Write 1 to the lower 5 bits
        cart->writeAddr(0x2000, 0x01);
        // Write 1 (32) to the upper 2 bits
        cart->writeAddr(0x4000, 0x01);
        REQUIRE(READ_U16(cart, 0x4000) == 33);
    }

    SECTION("Bank 32") {
        // Since 0x00 in the lower bits will always go to 0x01,
        // this should also go to bank 33
        cart->writeAddr(0x2000, 0x00);
        cart->writeAddr(0x4000, 0x01);
        REQUIRE(READ_U16(cart, 0x4000) == 33);
    }

    SECTION("Bank 65") {
        // We only support 64 banks in this ROM, so this should return 0 always
        cart->writeAddr(0x2000, 0x01);
        cart->writeAddr(0x4000, 0x02);
        REQUIRE(READ_U16(cart, 0x4000) == 0);
    }

    delete rom;
    delete cart;
}

TEST_CASE("MBC1 RAM Bank Test 2M/8K", "[cart][mbc1]") {
    uint8_t *rom = createRom(64, CART_HDR_TYPE_MBC1_RAM, CART_HDR_ROMSZ_8MBIT, CART_HDR_RAMSZ_64KBIT);

    Cartridge *cart = new MBC1(rom, CALC_ROM_SZ(64));

    SECTION("RAM Off on Power Up") {
        // A write should do nothing, and 0 should be returned
        cart->writeAddr(0xA000, 0xaa);
        REQUIRE(cart->readAddr(0xA000) == 0);
    }

    SECTION("RAM On, R/W") {
        cart->writeAddr(0x0000, 0x0A);
        cart->writeAddr(0xA000, 0xaa);
        REQUIRE(cart->readAddr(0xA000) == 0xaa);
    }

    delete rom;
    delete cart;
}

TEST_CASE("MBC1 ROM Bank Test 512K/32K", "[cart]") {
    uint8_t *rom = createRom(64, CART_HDR_TYPE_MBC1, CART_HDR_ROMSZ_8MBIT, CART_HDR_RAMSZ_NONE);

    Cartridge *cart = new MBC1(rom, CALC_ROM_SZ(64));
    cart->writeAddr(0x6000, 0x01);

    SECTION("Bank 0") {
        // Select bank 0, which should actually select bank 1
        cart->writeAddr(0x2000, 0x00);
        // Check our bank ID for bank 1
        REQUIRE(READ_U16(cart, 0x4000) == 0x0001);
    }

    SECTION("Bank 34 (2)") {
        // Should be bank 2, since upper bits control RAM bank now
        cart->writeAddr(0x2000, 0x02);
        cart->writeAddr(0x4000, 0x01);
        REQUIRE(READ_U16(cart, 0x4000) == 2);
    }

    delete rom;
    delete cart;
}

TEST_CASE("MBC1 RAM Bank Test 512K/32K", "[cart][mbc1][ram]") {
    uint8_t *rom = createRom(64, CART_HDR_TYPE_MBC1_RAM, CART_HDR_ROMSZ_8MBIT, CART_HDR_RAMSZ_256KBIT);

    Cartridge *cart = new MBC1(rom, CALC_ROM_SZ(64));
    cart->writeAddr(0x6000, 0x01);

    SECTION("RAM Off on Power Up") {
        // A write should do nothing, and 0 should be returned
        cart->writeAddr(0xA000, 0xaa);
        REQUIRE(cart->readAddr(0xA000) == 0);
    }

    SECTION("RAM On, R/W") {
        cart->writeAddr(0x0000, 0x0A);
        cart->writeAddr(0xA000, 0xaa);
        REQUIRE(cart->readAddr(0xA000) == 0xaa);
    }

    SECTION("R/W 2 banks") {
        // Switch to bank 1
        cart->writeAddr(0x0000, 0x0A);
        cart->writeAddr(0x4000, 0x01);
        cart->writeAddr(0xA000, 0x01);

        //Switch to bank 2
        cart->writeAddr(0x4000, 0x02);
        cart->writeAddr(0xA000, 0x02);

        // Switch bank to 1 and make sure the value matches
        cart->writeAddr(0x4000, 0x01);
        REQUIRE(cart->readAddr(0xA000) == 0x01);

        // Switch bank to 2 and make sure the value matches
        cart->writeAddr(0x4000, 0x02);
        REQUIRE(cart->readAddr(0xA000) == 0x02);
    }

    delete rom;
    delete cart;
}

TEST_CASE("MBC2 ROM Bank Test", "[cart][mbc2]") {
    uint8_t *rom = createRom(16, CART_HDR_TYPE_MBC2, CART_HDR_ROMSZ_2MBIT, CART_HDR_RAMSZ_NONE);

    Cartridge *cart = new MBC2(rom, CALC_ROM_SZ(16));

    SECTION("Read Bank 0") {
        REQUIRE(READ_U16(cart, 0x0000) == 0x0000);
    }

    SECTION("Select Bank 0") {
        // Select bank 0, which should actually select bank 1
        cart->writeAddr(0x2100, 0x00);
        // Check our bank ID for bank 1
        REQUIRE(READ_U16(cart, 0x4000) == 0x0001);
    }

    SECTION("Select Bank 2") {
        cart->writeAddr(0x2100, 0x02);
        // Check our bank ID for bank 1
        REQUIRE(READ_U16(cart, 0x4000) == 0x0002);
    }

    SECTION("Select Bank 17") {
        cart->writeAddr(0x2100, 0x11);
        // Check our bank ID for bank 1
        REQUIRE(READ_U16(cart, 0x4000) == 0x0001);
    }

    SECTION("Write to ROM Bank without 0x100 set") {
        cart->writeAddr(0x2100, 0x02);

        REQUIRE(READ_U16(cart, 0x4000) == 0x0002);

        cart->writeAddr(0x2000, 0x03);
        REQUIRE(READ_U16(cart, 0x4000) == 0x0002);
    }

    delete rom;
    delete cart;
}

TEST_CASE("MBC2 RAM Tests", "[cart][mbc2]") {
    uint8_t *rom = createRom(16, CART_HDR_TYPE_MBC2, CART_HDR_ROMSZ_2MBIT, CART_HDR_RAMSZ_NONE);

    Cartridge *cart = new MBC2(rom, CALC_ROM_SZ(16));

    // Enable RAM
    cart->writeAddr(0x0000, 0x0a);

    SECTION("Disable RAM with 0x100 set") {
        // Set a value
        cart->writeAddr(0xA000, 0x0b);

        // Disable RAM with an invalid value
        cart->writeAddr(0x0100, 0x00);

        // RAM should still be enabled
        REQUIRE(cart->readAddr(0xA000) == 0x0b);
    }

    SECTION("Disable RAM") {
        // Set a value
        cart->writeAddr(0xA000, 0x0b);

        // Disable RAM
        cart->writeAddr(0x0000, 0x00);

        // RAM should be disabled
        REQUIRE(cart->readAddr(0xA000) == 0x00);
    }

    SECTION("Lower 4 bits only") {
        cart->writeAddr(0xA000, 0x1b);
        REQUIRE(cart->readAddr(0xA000) == 0x0b);
    }
    delete rom;
    delete cart;
}
