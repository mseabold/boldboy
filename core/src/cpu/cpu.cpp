#include "cpu.h"

Cpu::Cpu(Mmu *mmu) : mMmu(mmu)
{
    for(int i=0;i<NUM_R8;i++) {
        // Create a new R8 for every 8 bit register
        mReg8s[i] = new Reg8();
    }

    for(int i=0;i<=rHL;i++) {
        // Create a new R16 for all R16s made of 2 R8s
        mReg16s[i] = new Reg16(mReg8s[i*2], mReg8s[i*2+1]);
    }

    // Now create 2 more R16s for SP and PC, which don't have 8 bit components
    Reg8 *r1 = new Reg8();
    Reg8 *r2 = new Reg8();

    mReg16s[rSP] = new Reg16(r1, r2);

    r1 = new Reg8();
    r2 = new Reg8();

    mReg16s[rPC] = new Reg16(r1, r2);
    mrA = mReg8s[rA];
    mrF = mReg8s[rF];
    mrPC = mReg16s[rPC];
    mrSP = mReg16s[rSP];
    mrHL = mReg16s[rHL];
    mBranchTaken = false;

    initOps();
}

Cpu::~Cpu()
{
    for(int i=0;i<NUM_R8;i++)
        delete mReg8s[i];

    delete mReg16s[rPC]->getLow();
    delete mReg16s[rPC]->getHigh();

    delete mReg16s[rSP]->getLow();
    delete mReg16s[rSP]->getHigh();

    for(int i=0;i<NUM_R16;i++)
        delete mReg16s[i];
}

uint8_t Cpu::add_3u8(uint8_t p1, uint8_t p2, uint8_t p3) {
    uint16_t result = p1 + p2 + p3;
    uint16_t carry = CARRY_BITS_3(p1, p2, p3, result);

    (result == 0)?SET_FLAG(FLAG_Z):CLEAR_FLAG(FLAG_Z);
    (carry & 0x100)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);
    (carry & 0x10)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);

    return result;
}

uint16_t Cpu::popStack_16(void) {
    uint16_t val;

    val = mMmu->readAddr(mrSP->read());
    mrSP->increment();
    val |= (mMmu->readAddr(mrSP->read()) << 8);
    mrSP->increment();

    return val;
}

void Cpu::pushStack_16(uint16_t val) {
    mrSP->decrement();
    mMmu->writeAddr(mrSP->read(), (uint8_t)(val >> 8));
    mrSP->decrement();
    mMmu->writeAddr(mrSP->read(), (uint8_t)(val & 0xFF));
}
