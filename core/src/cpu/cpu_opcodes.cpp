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
    mReg8s[p1]->increment();
}
