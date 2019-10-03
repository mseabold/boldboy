#include "cpu.h"
#include <stdio.h>

Cpu::Cpu(Mmu *mmu, InterruptController *ic) : mMmu(mmu), mIC(ic)
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

uint8_t Cpu::tick() {
    uint8_t op;
    Opcode *opcode;
    InterruptController::InterruptType irqType;

    mBranchTaken = false;
    mIsCB = false;

    //TODO: Eventually some/all opcodes will need to be handled
    //      in multiple ticks with a stored state. But the old adage:
    //      Premature optimization is the root of evil

    // If there is an interrupt pending, switch to the ISR
    // handler, then let the next tick began handling it
    if(mIC->isPending()) {
        irqType = mIC->getPending();

        if(irqType != InterruptController::itNone) {
            mIC->setEnabled(false);
            mIC->clearInterrupt(irqType);
            pushStack_16(mrPC->read());
            mrPC->write(mIC->sISRTable[irqType]);

            //Switching to the ISR takes 20 cycles
            return 20;
        }
    }

    op = mMmu->readAddr(mrPC->read());

    if(op == 0xcb) {
        op = mMmu->readAddr(mrPC->increment());
        opcode = &mExtOpTable[op];
        mIsCB = true;
    } else
        opcode = &mOpTable[op];

    mCurOpcode = op;

    mrPC->increment();

    (this->*opcode->handler)(opcode->p1, opcode->p2);

    if(mBranchTaken)
        return opcode->branchCycles;
    else
        return opcode->cycles;
}

void Cpu::disassemble(char *buffer, uint32_t bufLen) {
    uint8_t op;
    Opcode *opcode;

    op = mMmu->readAddr(mrPC->read());

    if(op == 0xcb) {
        op = mMmu->readAddr(mrPC->read()+1);
        opcode = &mExtOpTable[op];
    } else
        opcode = &mOpTable[op];

    snprintf(buffer, bufLen, "0x%04x: %s", mrPC->read(), opcode->mnemonic);
}

//#define SIGN_EXTEND(_p) ((uint16_t)(_p) | ((_p & 0x80)?0xff00:0));
#define SIGN_EXTEND(_p) ((uint16_t)_p)


uint8_t Cpu::add(uint8_t p1, uint8_t p2, bool checkC) {
    uint16_t p1e = SIGN_EXTEND(p1);
    uint16_t p2e = SIGN_EXTEND(p2);
    uint16_t c = (checkC && TEST_FLAG(FLAG_C))?1:0;

    uint16_t result = p1e + p2e + c;
    uint16_t carry = CARRY_BITS_3(p1e, p2e, c, result);

    ((uint8_t)result == 0)?SET_FLAG(FLAG_Z):CLEAR_FLAG(FLAG_Z);
    CHECK_ZERO((uint8_t)result);
    (carry & 0x100)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);
    (carry & 0x10)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);

    return (uint8_t)result;
}

uint8_t Cpu::sub(uint8_t p1, uint8_t p2, bool checkC) {
    uint16_t p1e = SIGN_EXTEND(p1);
    uint16_t p2e = SIGN_EXTEND(p2);
    uint16_t c = (checkC && TEST_FLAG(FLAG_C))?1:0;

    uint16_t result = p1e - p2e - c;
    uint16_t carry = CARRY_BITS_3(p1e, p2e, c, result);

    CHECK_ZERO((uint8_t)result);
    (carry & 0x100)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);
    (carry & 0x10)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);

    return (uint8_t)result;
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

void Cpu::and_A(uint8_t param) {
    uint8_t result = mrA->read() & param;
    CLEAR_FLAGS;
    CHECK_ZERO(result);
    SET_FLAG(FLAG_H);
    mrA->write(result);
}

void Cpu::xor_A(uint8_t param) {
    uint8_t result = mrA->read() ^ param;
    CLEAR_FLAGS;
    CHECK_ZERO(result);
    mrA->write(result);
}

void Cpu::or_A(uint8_t param) {
    uint8_t result = mrA->read() | param;
    CLEAR_FLAGS;
    CHECK_ZERO(result);
    mrA->write(result);
}
