#include "cpu.h"

Cpu::Opcode::Opcode() {
    handler = &Cpu::oph_Invalid;
    cycles = 4;
    branchCycles = 0;
    mnemonic = "Invalid";
    p1 = 0;
    p2 = 0;
}

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, const char *_mnemonic) {
    init(_handler, _cycles, _mnemonic, 0, 0);
}

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, uint8_t _branchCycles, const char *_mnemonic) {
    init(_handler, _cycles, _branchCycles, _mnemonic, 0, 0);
}

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, const char *_mnemonic, uint16_t _p1) {
    init(_handler, _cycles, _mnemonic, _p1, 0);
}

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, const char *_mnemonic, uint16_t _p1, uint16_t _p2) {
    init(_handler, _cycles, 0, _mnemonic, _p1, _p2);
}

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, uint8_t _branchCycles, const char *_mnemonic, uint16_t _p1, uint16_t _p2) {
    handler = _handler;
    cycles = _cycles;
    branchCycles = _branchCycles;
    mnemonic = _mnemonic;
    p1 = _p1;
    p2 = _p2;
}

void Cpu::oph_Nop(uint16_t p1, uint16_t p2) {
}

void Cpu::oph_Invalid(uint16_t p1, uint16_t p2) {
}

void Cpu::oph_Stop(uint16_t p1, uint16_t p2) {
}

void Cpu::oph_Halt(uint16_t p1, uint16_t p2) {
}

void Cpu::oph_EI(uint16_t p1, uint16_t p2) {
}

void Cpu::oph_DI(uint16_t p1, uint16_t p2) {
}

void Cpu::oph_Prefix(uint16_t p1, uint16_t p2) {
}

void Cpu::oph_LD_r16_d16(uint16_t p1, uint16_t p2) {
    mrPC->increment();
    mReg16s[p1]->getLow()->write(mMmu->readAddr(mrPC->read()));
    mrPC->increment();
    mReg16s[p1]->getHigh()->write(mMmu->readAddr(mrPC->read()));
}

void Cpu::oph_LD_ar16_A(uint16_t p1, uint16_t p2) {
    int16_t s_p2 = (int16_t)p2;
    mMmu->writeAddr(mReg16s[p1]->read(), mrA->read());

    if(s_p2 > 0)
        mReg16s[p1]->increment();
    else if(s_p2 < 0)
        mReg16s[p1]->decrement();
}

void Cpu::oph_Inc_r16(uint16_t p1, uint16_t p2) {
    mReg16s[p1]->increment();
}

void Cpu::oph_Inc_r8(uint16_t p1, uint16_t p2) {

    uint16_t newVal = mReg8s[p1]->increment();

    (newVal == 0)?SET_FLAG(FLAG_Z):CLEAR_FLAG(FLAG_Z);
    ((newVal & 0xf) == 0)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);

    CLEAR_FLAG(FLAG_N);
}

void Cpu::oph_Dec_r8(uint16_t p1, uint16_t p2) {
    uint16_t newVal = mReg8s[p1]->decrement();

    (newVal == 0)?SET_FLAG(FLAG_Z):CLEAR_FLAG(FLAG_Z);
    ((newVal & 0xf) == 0xf)?CLEAR_FLAG(FLAG_H):SET_FLAG(FLAG_H);

    SET_FLAG(FLAG_N);
}

void Cpu::oph_Inc_arHL(uint16_t p1, uint16_t p2) {
    uint8_t memVal = mMmu->readAddr(mrHL->read());
    ++memVal;
    mMmu->writeAddr(mrHL->read(), memVal);

    (memVal == 0)?SET_FLAG(FLAG_Z):CLEAR_FLAG(FLAG_Z);
    ((memVal & 0xf) == 0)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);

    CLEAR_FLAG(FLAG_N);
}

void Cpu::oph_Dec_arHL(uint16_t p1, uint16_t p2) {
    uint8_t memVal = mMmu->readAddr(mrHL->read());
    --memVal;
    mMmu->writeAddr(mrHL->read(), memVal);

    (memVal == 0)?SET_FLAG(FLAG_Z):CLEAR_FLAG(FLAG_Z);
    ((memVal & 0xf) == 0xf)?CLEAR_FLAG(FLAG_H):SET_FLAG(FLAG_H);

    SET_FLAG(FLAG_N);
}

void Cpu::oph_LD_r8_d8(uint16_t p1, uint16_t p2) {
    mrPC->increment();
    mReg8s[p1]->write(mMmu->readAddr(mrPC->read()));
}

void Cpu::oph_LD_arHL_d8(uint16_t p1, uint16_t p2) {
    mrPC->increment();
    mMmu->writeAddr(mrHL->read(), mMmu->readAddr(mrPC->read()));
}

void Cpu::oph_RLC(uint16_t p1, uint16_t p2) {
    uint16_t val = mReg8s[p1]->read();
    val = val << 1;

    // If carry, shift it to bit 0 and C flag
    if(val & 0x100)
    {
        val |= 1;
        SET_FLAG(FLAG_C);
    }
    else
        CLEAR_FLAG(FLAG_C);

    mReg8s[p1]->write((uint8_t)val);
}

void Cpu::oph_LD_a16_SP(uint16_t p1, uint16_t p2) {
    uint16_t addr;

    // Read in the 2 address bytes in LE
    addr = mMmu->readAddr(mrPC->increment());
    addr += mMmu->readAddr(mrPC->increment()) << 8;

    // Now write out SP into 2 bytes in LE
    mMmu->writeAddr(addr, mrSP->getLow()->read());
    mMmu->writeAddr(addr+1, mrSP->getHigh()->read());
}

void Cpu::oph_ADD_HL_r16(uint16_t p1, uint16_t p2) {
    uint32_t hlVal = mrHL->read();
    uint32_t pVal = mReg16s[p1]->read();
    uint32_t result = hlVal + pVal;

    (result & 0x10000)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);

    (CARRY_BITS(hlVal, pVal, (result & 0xffff)) & 0x1000)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);

    mrHL->write((uint16_t)result);
}
