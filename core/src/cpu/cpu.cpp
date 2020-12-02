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
    mCurState = 0;
    mIsCB = false;
    mIsHalted = false;

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

    //TODO: Eventually some/all opcodes will need to be handled
    //      in multiple ticks with a stored state. But the old adage:
    //      Premature optimization is the root of evil

    // If there is an interrupt pending, switch to the ISR
    // handler, then let the next tick began handling it
    if(mCurState == 0 && mIC->isPending()) {
        mIsHalted = false;

        // Only service the interrupt if IME
        if(mIC->isEnabled()) {
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
    }

    // Do nothing if halted
    if(mIsHalted)
        return 4;

    if(mCurState == 0) {
        op = mMmu->readAddr(mrPC->read());

        if(op == 0xcb) {
            op = mMmu->readAddr(mrPC->increment());
            opcode = &mExtOpTable[op];
            mIsCB = true;
        } else {
            opcode = &mOpTable[op];
            mIsCB = false;
        }

        mCurOpcode = op;

        // Populate the intstruction cycles with full number. Some opcodes
        // may override this if the the opcode should be handled in multiple
        // states.
        mCycles = opcode->cycles;
        mBrCycles = opcode->branchCycles;

        mrPC->increment();
    } else {
        if(mIsCB)
            opcode = &mExtOpTable[mCurOpcode];
        else
            opcode = &mOpTable[mCurOpcode];

        // We know if we're in a sub-state that the opcode MUST handle
        // consuming its own cycles
        mCycles = 0;
        mBrCycles = 0;
    }

    (this->*opcode->handler)(opcode->p1, opcode->p2);

    if(mBranchTaken)
        return mBrCycles;
    else
        return mCycles;
}

void Cpu::disassemble(char *buffer, uint32_t bufLen) {
    uint8_t op;
    Opcode *opcode;

    op = mMmu->readAddr(mrPC->read());

    if(mCurState == 0) {
        if(op == 0xcb) {
            op = mMmu->readAddr(mrPC->read()+1);
            opcode = &mExtOpTable[op];
        } else
            opcode = &mOpTable[op];
    } else {
        if(mIsCB) {
            op = mMmu->readAddr(mrPC->read()+1);
            opcode = &mExtOpTable[mCurOpcode];
        } else
            opcode = &mOpTable[mCurOpcode];
    }

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
    uint16_t val = popStack8();
    val |= (popStack8() << 8);

    return val;
}

void Cpu::pushStack_16(uint16_t val) {
    pushStack8((uint8_t)(val >> 8));
    pushStack8((uint8_t)(val & 0xff));
}

uint8_t Cpu::popStack8(void) {
    return mMmu->readAddr(mrSP->postIncrement());
}

void Cpu::pushStack8(uint8_t val) {
    mMmu->writeAddr(mrSP->decrement(), val);
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

uint8_t Cpu::rlc(uint8_t p1) {
    uint16_t result = (uint16_t)p1;
    result = result << 1;

    CLEAR_FLAGS;

    // If carry, shift it to bit 0 and C flag
    if(result & 0x100)
    {
        result |= 1;
        SET_FLAG(FLAG_C);
    }

    return (uint8_t)result;
}

uint8_t Cpu::rrc(uint8_t p1) {
    uint8_t cBit = p1 & 0x01;

    p1 >>= 1;

    CLEAR_FLAGS;

    if(cBit) {
        p1 |= 0x80;
        SET_FLAG(FLAG_C);
    }

    return p1;
}

uint8_t Cpu::rl(uint8_t p1) {
    uint16_t result = (uint16_t)p1;

    result <<= 1;

    if(TEST_FLAG(FLAG_C))
        result |= 0x01;

    CLEAR_FLAGS;

    if(result & 0x100)
        SET_FLAG(FLAG_C);

    return (uint8_t)result;
}

uint8_t Cpu::rr(uint8_t p1) {
    uint8_t cBit = p1 & 0x01;

    p1 >>= 1;

    if(TEST_FLAG(FLAG_C))
        p1 |= 0x80;

    CLEAR_FLAGS;

    if(cBit)
        SET_FLAG(FLAG_C);

    return p1;
}

uint8_t Cpu::sla(uint8_t p1) {
    CLEAR_FLAGS;
    (p1 & 0x80)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);

    p1 <<= 1;

    CHECK_ZERO(p1);

    return p1;
}

uint8_t Cpu::sra(uint8_t p1) {
    CLEAR_FLAGS;
    (p1 & 0x01)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);

    p1 >>= 1;

    if(p1 & 0x40)
        p1 |= 0x80;

    CHECK_ZERO(p1);

    return p1;
}

uint8_t Cpu::swap(uint8_t p1) {
    p1 = (((p1 & 0xF) << 4) | (p1 >> 4));
    CLEAR_FLAGS;
    CHECK_ZERO(p1);

    return p1;
}

uint8_t Cpu::srl(uint8_t p1) {
    CLEAR_FLAGS;
    (p1 & 0x01)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);

    p1 >>= 1;

    CHECK_ZERO(p1);

    return p1;
}

#define GET_BIT_FROM_OP(_base) ((mCurOpcode - _base) / 8)

void Cpu::bit(uint8_t p1) {
    CLEAR_FLAG(FLAG_N);
    SET_FLAG(FLAG_H);

    CHECK_ZERO((p1 & (1 << GET_BIT_FROM_OP(0x40))));
}

uint8_t Cpu::res(uint8_t p1) {
    p1 &= ~(1 << GET_BIT_FROM_OP(0x80));

    return p1;
}

uint8_t Cpu::set(uint8_t p1) {
    p1 |= (1 << GET_BIT_FROM_OP(0xc0));

    return p1;
}

void Cpu::procCallSubstate(bool branch) {
    uint16_t tmp;

    switch(mCurState) {
        case 0:
            ADVANCE_STATE(4);
            break;
        case 1:
            mCache = mMmu->readAddr(mrPC->postIncrement());
            ADVANCE_STATE(4);
            break;
        case 2:
            mCache |= (mMmu->readAddr(mrPC->postIncrement()) << 8);

            if(branch)
                ADVANCE_STATE(8);
            else
                RESET_STATE(4);

            break;
        case 3:
            tmp = mCache;
            mCache = mrPC->read();
            mrPC->write(tmp);

            pushStack8((uint8_t)(mCache >> 8));
            ADVANCE_STATE(4);
            break;
        case 4:
            pushStack8((uint8_t)(mCache & 0xff));
            RESET_STATE(4);
            break;
    }
}
