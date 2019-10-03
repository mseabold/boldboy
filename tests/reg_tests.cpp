#include <catch2/catch.hpp>
#include "cpu.h"

static inline uint8_t cpuTestWriteReadReg8(Cpu *cpu, Cpu::reg8_name reg, uint8_t val) {
    cpu->getReg8(reg)->write(val);
    return cpu->getReg8(reg)->read();
}

static inline uint16_t cpuTestWriteReadReg16(Cpu *cpu, Cpu::reg16_name reg, uint16_t val) {
    cpu->getReg16(reg)->write(val);
    return cpu->getReg16(reg)->read();
}

#define TEST_VAL_8 0xaa
#define TEST_VAL_16 0xaabb
#define TEST_VAL_8_HIGH 0xaa
#define TEST_VAL_8_LOW 0xbb


TEST_CASE("8 bit registers", "[cpu]") {
    Cpu *cpu = new Cpu(NULL, NULL);

    REQUIRE(cpu != NULL);

    SECTION("A Read/Write") {
        REQUIRE(cpuTestWriteReadReg8(cpu, Cpu::rA, TEST_VAL_8) == TEST_VAL_8);
    }
    SECTION("F Read/Write") {
        REQUIRE(cpuTestWriteReadReg8(cpu, Cpu::rF, TEST_VAL_8) == TEST_VAL_8);
    }
    SECTION("B Read/Write") {
        REQUIRE(cpuTestWriteReadReg8(cpu, Cpu::rB, TEST_VAL_8) == TEST_VAL_8);
    }
    SECTION("C Read/Write") {
        REQUIRE(cpuTestWriteReadReg8(cpu, Cpu::rC, TEST_VAL_8) == TEST_VAL_8);
    }
    SECTION("D Read/Write") {
        REQUIRE(cpuTestWriteReadReg8(cpu, Cpu::rD, TEST_VAL_8) == TEST_VAL_8);
    }
    SECTION("E Read/Write") {
        REQUIRE(cpuTestWriteReadReg8(cpu, Cpu::rE, TEST_VAL_8) == TEST_VAL_8);
    }
    SECTION("H Read/Write") {
        REQUIRE(cpuTestWriteReadReg8(cpu, Cpu::rH, TEST_VAL_8) == TEST_VAL_8);
    }
    SECTION("L Read/Write") {
        REQUIRE(cpuTestWriteReadReg8(cpu, Cpu::rL, TEST_VAL_8) == TEST_VAL_8);
    }

    delete cpu;
}

TEST_CASE("16 bit registers", "[cpu]") {
    Cpu *cpu = new Cpu(NULL, NULL);

    REQUIRE(cpu != NULL);

    SECTION("AF Read/Write") {
        REQUIRE(cpuTestWriteReadReg16(cpu, Cpu::rAF, TEST_VAL_16) == TEST_VAL_16);
    }
    SECTION("BC Read/Write") {
        REQUIRE(cpuTestWriteReadReg16(cpu, Cpu::rBC, TEST_VAL_16) == TEST_VAL_16);
    }
    SECTION("DE Read/Write") {
        REQUIRE(cpuTestWriteReadReg16(cpu, Cpu::rDE, TEST_VAL_16) == TEST_VAL_16);
    }
    SECTION("HL Read/Write") {
        REQUIRE(cpuTestWriteReadReg16(cpu, Cpu::rHL, TEST_VAL_16) == TEST_VAL_16);
    }
    SECTION("SP Read/Write") {
        REQUIRE(cpuTestWriteReadReg16(cpu, Cpu::rSP, TEST_VAL_16) == TEST_VAL_16);
    }
    SECTION("PC Read/Write") {
        REQUIRE(cpuTestWriteReadReg16(cpu, Cpu::rPC, TEST_VAL_16) == TEST_VAL_16);
    }

    delete cpu;
}

TEST_CASE("16 bit write/8 bit read direct", "[cpu]") {
    Cpu *cpu = new Cpu(NULL, NULL);

    REQUIRE(cpu != NULL);

    SECTION("AF Read/Write") {
        cpu->getReg16(Cpu::rAF)->write(TEST_VAL_16);
        REQUIRE(cpu->getReg8(Cpu::rA)->read() == TEST_VAL_8_HIGH);
        REQUIRE(cpu->getReg8(Cpu::rF)->read() == TEST_VAL_8_LOW);

    }
    SECTION("BC Read/Write") {
        cpu->getReg16(Cpu::rBC)->write(TEST_VAL_16);
        REQUIRE(cpu->getReg8(Cpu::rB)->read() == TEST_VAL_8_HIGH);
        REQUIRE(cpu->getReg8(Cpu::rC)->read() == TEST_VAL_8_LOW);
    }
    SECTION("DE Read/Write") {
        cpu->getReg16(Cpu::rDE)->write(TEST_VAL_16);
        REQUIRE(cpu->getReg8(Cpu::rD)->read() == TEST_VAL_8_HIGH);
        REQUIRE(cpu->getReg8(Cpu::rE)->read() == TEST_VAL_8_LOW);
    }
    SECTION("HL Read/Write") {
        cpu->getReg16(Cpu::rHL)->write(TEST_VAL_16);
        REQUIRE(cpu->getReg8(Cpu::rH)->read() == TEST_VAL_8_HIGH);
        REQUIRE(cpu->getReg8(Cpu::rL)->read() == TEST_VAL_8_LOW);
    }

    delete cpu;
}

TEST_CASE("16 bit write/8 bit read", "[cpu]") {
    Cpu *cpu = new Cpu(NULL, NULL);

    REQUIRE(cpu != NULL);

    SECTION("AF Read/Write") {
        Reg16 *r = cpu->getReg16(Cpu::rAF);
        r->write(TEST_VAL_16);
        REQUIRE(r->getHigh()->read() == TEST_VAL_8_HIGH);
        REQUIRE(r->getLow()->read() == TEST_VAL_8_LOW);

    }
    SECTION("BC Read/Write") {
        Reg16 *r = cpu->getReg16(Cpu::rBC);
        r->write(TEST_VAL_16);
        REQUIRE(r->getHigh()->read() == TEST_VAL_8_HIGH);
        REQUIRE(r->getLow()->read() == TEST_VAL_8_LOW);
    }
    SECTION("DE Read/Write") {
        Reg16 *r = cpu->getReg16(Cpu::rDE);
        r->write(TEST_VAL_16);
        REQUIRE(r->getHigh()->read() == TEST_VAL_8_HIGH);
        REQUIRE(r->getLow()->read() == TEST_VAL_8_LOW);
    }
    SECTION("HL Read/Write") {
        Reg16 *r = cpu->getReg16(Cpu::rHL);
        r->write(TEST_VAL_16);
        REQUIRE(r->getHigh()->read() == TEST_VAL_8_HIGH);
        REQUIRE(r->getLow()->read() == TEST_VAL_8_LOW);
    }
    SECTION("SP Read/Write") {
        Reg16 *r = cpu->getReg16(Cpu::rSP);
        r->write(TEST_VAL_16);
        REQUIRE(r->getHigh()->read() == TEST_VAL_8_HIGH);
        REQUIRE(r->getLow()->read() == TEST_VAL_8_LOW);
    }
    SECTION("PC Read/Write") {
        Reg16 *r = cpu->getReg16(Cpu::rPC);
        r->write(TEST_VAL_16);
        REQUIRE(r->getHigh()->read() == TEST_VAL_8_HIGH);
        REQUIRE(r->getLow()->read() == TEST_VAL_8_LOW);
    }

    delete cpu;
}

TEST_CASE("8 bit register increment/decrement", "[cpu]") {
    Cpu *cpu = new Cpu(NULL, NULL);

    REQUIRE(cpu != NULL);

    SECTION("A Inc/Dec") {
        Reg8 *r = cpu->getReg8(Cpu::rA);
        r->write(TEST_VAL_8);
        REQUIRE(r->increment() == TEST_VAL_8 + 1);
        REQUIRE(r->decrement() == TEST_VAL_8);
    }
    SECTION("F Inc/Dec") {
        Reg8 *r = cpu->getReg8(Cpu::rF);
        r->write(TEST_VAL_8);
        REQUIRE(r->increment() == TEST_VAL_8 + 1);
        REQUIRE(r->decrement() == TEST_VAL_8);
    }
    SECTION("B Inc/Dec") {
        Reg8 *r = cpu->getReg8(Cpu::rB);
        r->write(TEST_VAL_8);
        REQUIRE(r->increment() == TEST_VAL_8 + 1);
        REQUIRE(r->decrement() == TEST_VAL_8);
    }
    SECTION("C Inc/Dec") {
        Reg8 *r = cpu->getReg8(Cpu::rC);
        r->write(TEST_VAL_8);
        REQUIRE(r->increment() == TEST_VAL_8 + 1);
        REQUIRE(r->decrement() == TEST_VAL_8);
    }
    SECTION("D Inc/Dec") {
        Reg8 *r = cpu->getReg8(Cpu::rD);
        r->write(TEST_VAL_8);
        REQUIRE(r->increment() == TEST_VAL_8 + 1);
        REQUIRE(r->decrement() == TEST_VAL_8);
    }
    SECTION("E Inc/Dec") {
        Reg8 *r = cpu->getReg8(Cpu::rE);
        r->write(TEST_VAL_8);
        REQUIRE(r->increment() == TEST_VAL_8 + 1);
        REQUIRE(r->decrement() == TEST_VAL_8);
    }
    SECTION("H Inc/Dec") {
        Reg8 *r = cpu->getReg8(Cpu::rH);
        r->write(TEST_VAL_8);
        REQUIRE(r->increment() == TEST_VAL_8 + 1);
        REQUIRE(r->decrement() == TEST_VAL_8);
    }
    SECTION("L Inc/Dec") {
        Reg8 *r = cpu->getReg8(Cpu::rL);
        r->write(TEST_VAL_8);
        REQUIRE(r->increment() == TEST_VAL_8 + 1);
        REQUIRE(r->decrement() == TEST_VAL_8);
    }

    delete cpu;
}

TEST_CASE("16 bit increment/decrement", "[cpu]") {
    Cpu *cpu = new Cpu(NULL, NULL);

    REQUIRE(cpu != NULL);

    SECTION("AF Inc/Dec") {
        Reg16 *r = cpu->getReg16(Cpu::rAF);
        r->write(TEST_VAL_16);
        REQUIRE(r->increment() == TEST_VAL_16 + 1);
        REQUIRE(r->decrement() == TEST_VAL_16);

    }
    SECTION("BC Inc/Dec") {
        Reg16 *r = cpu->getReg16(Cpu::rBC);
        r->write(TEST_VAL_16);
        REQUIRE(r->increment() == TEST_VAL_16 + 1);
        REQUIRE(r->decrement() == TEST_VAL_16);
    }
    SECTION("DE Inc/Dec") {
        Reg16 *r = cpu->getReg16(Cpu::rDE);
        r->write(TEST_VAL_16);
        REQUIRE(r->increment() == TEST_VAL_16 + 1);
        REQUIRE(r->decrement() == TEST_VAL_16);
    }
    SECTION("HL Inc/Dec") {
        Reg16 *r = cpu->getReg16(Cpu::rHL);
        r->write(TEST_VAL_16);
        REQUIRE(r->increment() == TEST_VAL_16 + 1);
        REQUIRE(r->decrement() == TEST_VAL_16);
    }
    SECTION("SP Inc/Dec") {
        Reg16 *r = cpu->getReg16(Cpu::rSP);
        r->write(TEST_VAL_16);
        REQUIRE(r->increment() == TEST_VAL_16 + 1);
        REQUIRE(r->decrement() == TEST_VAL_16);
    }
    SECTION("PC Inc/Dec") {
        Reg16 *r = cpu->getReg16(Cpu::rPC);
        r->write(TEST_VAL_16);
        REQUIRE(r->increment() == TEST_VAL_16 + 1);
        REQUIRE(r->decrement() == TEST_VAL_16);
    }

    delete cpu;
}
