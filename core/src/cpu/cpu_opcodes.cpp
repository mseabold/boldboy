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

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, uint8_t _branchCycles, const char *_mnemonic, uint16_t _p1) {
    init(_handler, _cycles, _branchCycles, _mnemonic, _p1, 0);
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

void Cpu::oph_Dec_r16(uint16_t p1, uint16_t p2) {
    mReg16s[p1]->decrement();
}

void Cpu::oph_Inc_r8(uint16_t p1, uint16_t p2) {

    uint16_t newVal = mReg8s[p1]->increment();

    CHECK_ZERO(newVal);
    ((newVal & 0xf) == 0)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);

    CLEAR_FLAG(FLAG_N);
}

void Cpu::oph_Dec_r8(uint16_t p1, uint16_t p2) {
    uint16_t newVal = mReg8s[p1]->decrement();

    CHECK_ZERO(newVal);
    ((newVal & 0xf) == 0xf)?CLEAR_FLAG(FLAG_H):SET_FLAG(FLAG_H);

    SET_FLAG(FLAG_N);
}

void Cpu::oph_Inc_arHL(uint16_t p1, uint16_t p2) {
    uint8_t memVal = mMmu->readAddr(mrHL->read());
    ++memVal;
    mMmu->writeAddr(mrHL->read(), memVal);

    CHECK_ZERO(memVal);
    ((memVal & 0xf) == 0)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);

    CLEAR_FLAG(FLAG_N);
}

void Cpu::oph_Dec_arHL(uint16_t p1, uint16_t p2) {
    uint8_t memVal = mMmu->readAddr(mrHL->read());
    --memVal;
    mMmu->writeAddr(mrHL->read(), memVal);

    CHECK_ZERO(memVal);
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

void Cpu::oph_LD_A_ar16(uint16_t p1, uint16_t p2) {
    int16_t s_p2 = (int16_t)p2;

    mrA->write(mMmu->readAddr(mReg16s[p1]->read()));

    if(s_p2 > 0)
        mReg16s[p1]->increment();
    else if(s_p2 < 0)
        mReg16s[p1]->decrement();
}

void Cpu::oph_JR(uint16_t p1, uint16_t p2) {
    // Note the r8 value in the code accounts for
    // the PC being incremented.
    uint16_t pcVal = mrPC->increment();
    int8_t rVal = (int8_t)mMmu->readAddr(pcVal);

    mrPC->write(pcVal + rVal);
}

void Cpu::oph_JR_Z_NZ(uint16_t p1, uint16_t p2) {
    if(TEST_FLAG(FLAG_Z) == p1) {
        uint16_t pcVal = mrPC->increment();
        int8_t rVal = (int8_t)mMmu->readAddr(pcVal);
        mrPC->write(pcVal + rVal);
        mBranchTaken = true;
    } else {
        mrPC->increment();
        mBranchTaken = false;
    }
}

void Cpu::oph_JR_C_NC(uint16_t p1, uint16_t p2) {
    if(TEST_FLAG(FLAG_C) == p1) {
        uint16_t pcVal = mrPC->increment();
        int8_t rVal = (int8_t)mMmu->readAddr(pcVal);
        mrPC->write(pcVal + rVal);
        mBranchTaken = true;
    } else {
        mrPC->increment();
        mBranchTaken = false;
    }
}
void Cpu::oph_DAA(uint16_t p1, uint16_t p2) {
    //TODO
}

void Cpu::oph_CPL(uint16_t p1, uint16_t p2) {
    mrA->write(mrA->read() ^ 0xFF);
    SET_FLAG(FLAG_N|FLAG_H);
}

void Cpu::oph_SCF(uint16_t p1, uint16_t p2) {
    CLEAR_FLAG(FLAG_N|FLAG_H);
    SET_FLAG(FLAG_C);
}

void Cpu::oph_CCF(uint16_t p1, uint16_t p2) {
    CLEAR_FLAG(FLAG_N|FLAG_H);
    TOGGLE_FLAG(FLAG_C);
}

void Cpu::oph_LD_r8_r8(uint16_t p1, uint16_t p2) {
    mReg8s[p1]->write(mReg8s[p2]->read());
}

void Cpu::oph_LD_r8_arHL(uint16_t p1, uint16_t p2) {
    mReg8s[p1]->write(mMmu->readAddr(mrHL->read()));
}

void Cpu::oph_LD_arHL_r8(uint16_t p1, uint16_t p2) {
    mMmu->writeAddr(mrHL->read(), mReg8s[p1]->read());
}

void Cpu::oph_ADD_A_r8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mReg8s[p1]->read();

    CLEAR_FLAG(FLAG_N);

    mrA->write(add_3u8(aVal, rVal, 0));
}

void Cpu::oph_ADD_A_arHL(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mMmu->readAddr(mrHL->read());

    CLEAR_FLAG(FLAG_N);
    mrA->write(add_3u8(aVal, rVal, 0));
}

void Cpu::oph_ADD_A_d8(uint16_t p1, uint16_t p2) {
    uint8_t dVal = mMmu->readAddr(mrPC->increment());
    uint8_t aVal = mrA->read();

    CLEAR_FLAG(FLAG_N);
    mrA->write(add_3u8(dVal, aVal, 0));
}

void Cpu::oph_ADC_A_r8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mReg8s[p1]->read();

    CLEAR_FLAG(FLAG_N);
    mrA->write(add_3u8(aVal, rVal, TEST_FLAG(FLAG_C)?1:0));
}

void Cpu::oph_ADC_A_arHL(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mMmu->readAddr(mrHL->read());

    CLEAR_FLAG(FLAG_N);
    mrA->write(add_3u8(aVal, rVal, TEST_FLAG(FLAG_C)?1:0));
}

void Cpu::oph_ADC_A_d8(uint16_t p1, uint16_t p2) {
    uint8_t dVal = mMmu->readAddr(mrPC->increment());
    uint8_t aVal = mrA->read();

    CLEAR_FLAG(FLAG_N);
    mrA->write(add_3u8(dVal, aVal, TEST_FLAG(FLAG_C)?1:0));
}

void Cpu::oph_SUB_A_r8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mReg8s[p1]->read();

    SET_FLAG(FLAG_N);
    mrA->write(add_3u8(aVal, ~rVal+1, 0));
}

void Cpu::oph_SUB_A_arHL(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mMmu->readAddr(mrHL->read());

    SET_FLAG(FLAG_N);
    mrA->write(add_3u8(aVal, ~rVal+1, 0));
}

void Cpu::oph_SUB_A_d8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t dVal = mMmu->readAddr(mrPC->increment());

    SET_FLAG(FLAG_N);
    mrA->write(add_3u8(aVal, ~dVal+1, 0));
}

void Cpu::oph_SBC_A_r8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mReg8s[p1]->read();

    SET_FLAG(FLAG_N);
    mrA->write(add_3u8(aVal, ~rVal+1, TEST_FLAG(FLAG_C)?-1:0));
}

void Cpu::oph_SBC_A_arHL(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mMmu->readAddr(mrHL->read());

    SET_FLAG(FLAG_N);
    mrA->write(add_3u8(aVal, ~rVal+1, TEST_FLAG(FLAG_C)?-1:0));
}

void Cpu::oph_SBC_A_d8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t dVal = mMmu->readAddr(mrPC->increment());

    SET_FLAG(FLAG_N);
    mrA->write(add_3u8(aVal, ~dVal+1, TEST_FLAG(FLAG_C)?-1:0));
}

void Cpu::oph_AND_r8(uint16_t p1, uint16_t p2) {
    and_A(mReg8s[p1]->read());
}

void Cpu::oph_AND_arHL(uint16_t p1, uint16_t p2) {
    and_A(mMmu->readAddr(mrHL->read()));
}

void Cpu::oph_AND_d8(uint16_t p1, uint16_t p2) {
    and_A(mMmu->readAddr(mrPC->increment()));
}

void Cpu::oph_XOR_r8(uint16_t p1, uint16_t p2) {
    xor_A(mReg8s[p1]->read());
}

void Cpu::oph_XOR_arHL(uint16_t p1, uint16_t p2) {
    xor_A(mMmu->readAddr(mrHL->read()));
}

void Cpu::oph_XOR_d8(uint16_t p1, uint16_t p2) {
    xor_A(mMmu->readAddr(mrPC->increment()));
}

void Cpu::oph_OR_r8(uint16_t p1, uint16_t p2) {
    or_A(mReg8s[p1]->read());
}

void Cpu::oph_OR_arHL(uint16_t p1, uint16_t p2) {
    or_A(mMmu->readAddr(mrHL->read()));
}

void Cpu::oph_OR_d8(uint16_t p1, uint16_t p2) {
    or_A(mMmu->readAddr(mrPC->increment()));
}

void Cpu::oph_CP_r8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mReg8s[p1]->read();

    SET_FLAG(FLAG_N);

    // Simply use the subtraction subroutine, but throw away the result.
    // This will set all required flags
    add_3u8(aVal, ~rVal+1, 0);
}

void Cpu::oph_CP_arHL(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mMmu->readAddr(mrHL->read());

    SET_FLAG(FLAG_N);

    // Simply use the subtraction subroutine, but throw away the result.
    // This will set all required flags
    add_3u8(aVal, ~rVal+1, 0);
}

void Cpu::oph_CP_d8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t dVal = mMmu->readAddr(mrPC->increment());

    SET_FLAG(FLAG_N);

    // Simply use the subtraction subroutine, but throw away the result.
    // This will set all required flags
    add_3u8(aVal, ~dVal+1, 0);
}

void Cpu::oph_RET_flag(uint16_t p1, uint16_t p2) {
    if(!(p2 ^ TEST_FLAG(p1))) {
        mBranchTaken = true;
        oph_RET(0,0);
    }
}

void Cpu::oph_RET(uint16_t p1, uint16_t p2) {
    mrPC->write(popStack_16());
}

void Cpu::oph_RETI(uint16_t p1, uint16_t p2) {
    oph_RET(0,0);
    //TODO: Enable Interrupts
}

void Cpu::oph_POP_r16(uint16_t p1, uint16_t p2) {
    mReg16s[p1]->write(popStack_16());
}

void Cpu::oph_PUSH_r16(uint16_t p1, uint16_t p2) {
    pushStack_16(mReg16s[p1]->read());
}

void Cpu::oph_JP_flag_a16(uint16_t p1, uint16_t p2) {
    if(!(p2 ^ TEST_FLAG(p1))) {
        mBranchTaken = true;
        oph_JP_a16(0,0);
    }
}

void Cpu::oph_JP_a16(uint16_t p1, uint16_t p2) {
    uint16_t pcVal;

    pcVal = mMmu->readAddr(mrPC->increment());
    pcVal |= (mMmu->readAddr(mrPC->increment()) << 8);

    mrPC->write(pcVal);
}

void Cpu::oph_JP_arHL(uint16_t p1, uint16_t p2) {
    mrPC->write(mrHL->read());
}

void Cpu::oph_CALL_flag_a16(uint16_t p1, uint16_t p2) {
    if(!(p2 ^ TEST_FLAG(p1))) {
        mBranchTaken = true;
        oph_CALL_a16(0,0);
    }
}

void Cpu::oph_CALL_a16(uint16_t p1, uint16_t p2) {
    uint16_t aVal;
    aVal = mMmu->readAddr(mrPC->increment());
    aVal |= (mMmu->readAddr(mrPC->increment()) << 8);
    pushStack_16(mrPC->read());
    mrPC->write(aVal);
}

void Cpu::oph_RST_n(uint16_t p1, uint16_t p2) {
    pushStack_16(mrPC->read());
    mrPC->write(p1);
}

void Cpu::oph_LDH_a8_A(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mMmu->readAddr(mrPC->increment());
    mMmu->writeAddr(0xFF00 + aVal, mrA->read());
}

void Cpu::oph_LDH_A_a8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mMmu->readAddr(mrPC->increment());
    mrA->write(mMmu->readAddr(0xFF00 + aVal));
}

void Cpu::oph_LD_arC_A(uint16_t p1, uint16_t p2) {
    mMmu->writeAddr(0xFF00 + mReg8s[rC]->read(), mrA->read());
}

void Cpu::oph_LD_A_arC(uint16_t p1, uint16_t p2) {
    mrA->write(mMmu->readAddr(0xFF00 + mReg8s[rC]->read()));
}

void Cpu::oph_ADD_SP_r8(uint16_t p1, uint16_t p2) {
    uint16_t aVal = mMmu->readAddr(mrPC->increment());
    uint16_t rVal = mrSP->read();

    uint16_t r = aVal + rVal;

    CLEAR_FLAG(FLAG_Z | FLAG_N);
    (CARRY_BITS(aVal, rVal, r) & 0x10)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);
    (CARRY_BITS(aVal, rVal, r) & 0x100)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);
    mrSP->write(r);
}

void Cpu::oph_LD_a16_A(uint16_t p1, uint16_t p2) {
    uint16_t aVal = mMmu->readAddr(mrPC->increment());
    aVal |= mMmu->readAddr(mrPC->increment()) << 8;

    mMmu->writeAddr(aVal, mrA->read());
}

void Cpu::oph_LD_A_a16(uint16_t p1, uint16_t p2) {
    uint16_t aVal = mMmu->readAddr(mrPC->increment());
    aVal |= mMmu->readAddr(mrPC->increment()) << 8;

    mrA->write(mMmu->readAddr(aVal));
}

void Cpu::oph_LD_HL_SP_p_r8(uint16_t p1, uint16_t p2) {
    uint16_t aVal = mMmu->readAddr(mrPC->increment());
    uint16_t rVal = mrSP->read();

    uint16_t r = aVal + rVal;

    CLEAR_FLAG(FLAG_Z | FLAG_N);
    (CARRY_BITS(aVal, rVal, r) & 0x10)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);
    (CARRY_BITS(aVal, rVal, r) & 0x100)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);
    mrHL->write(r);
}

void Cpu::oph_LD_SP_HL(uint16_t p1, uint16_t p2) {
    mrSP->write(mrHL->read());
}

/*************************************************
 * Note: all of the bit operations take an initial
 *       parameter that indicates whether the
 *       operation is being done on a register or
 *       to memory via (HL)
 ************************************************/
#define GET_VAL(_isHL, _reg) (_isHL)?mMmu->readAddr(mrHL->read()):mReg8s[_reg]->read()
#define SET_VAL(_isHL, _reg, _val) (_isHL)?mMmu->writeAddr(mrHL->read(), _val):mReg8s[_reg]->write(_val)

void Cpu::oph_RLC(uint16_t p1, uint16_t p2) {
    uint16_t rVal = (uint16_t)GET_VAL(p1, p2);
    rVal = rVal << 1;

    CLEAR_FLAGS;

    // If carry, shift it to bit 0 and C flag
    if(rVal & 0x100)
    {
        rVal |= 1;
        SET_FLAG(FLAG_C);
    }

    SET_VAL(p1, p2, (uint8_t )rVal);
}

void Cpu::oph_RRC(uint16_t p1, uint16_t p2) {
    uint8_t rVal = GET_VAL(p1, p2);
    uint8_t cBit = rVal & 0x01;

    rVal >>= 1;

    CLEAR_FLAGS;

    if(cBit) {
        rVal |= 0x80;
        SET_FLAG(FLAG_C);
    }

    SET_VAL(p1, p2, rVal);
}

void Cpu::oph_RL(uint16_t p1, uint16_t p2) {
    uint16_t rVal = (uint16_t)GET_VAL(p1, p2);
    rVal <<= 1;

    if(TEST_FLAG(FLAG_C))
        rVal |= 0x01;

    CLEAR_FLAGS;

    if(rVal & 0x100)
        SET_FLAG(FLAG_C);

    SET_VAL(p1, p2, (uint8_t )rVal);
}

void Cpu::oph_RR(uint16_t p1, uint16_t p2) {
    uint8_t rVal = GET_VAL(p1, p2);
    uint8_t cBit = rVal & 0x01;

    rVal >>= 1;

    if(TEST_FLAG(FLAG_C))
        rVal |= 0x80;

    CLEAR_FLAGS;

    if(cBit)
        SET_FLAG(FLAG_C);

    SET_VAL(p1, p2, rVal);
}

void Cpu::oph_SLA(uint16_t p1, uint16_t p2) {
    uint8_t rVal = GET_VAL(p1, p2);

    CLEAR_FLAGS;
    (rVal & 0x80)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);

    rVal <<= 1;

    CHECK_ZERO(rVal);

    SET_VAL(p1, p2, rVal);
}

void Cpu::oph_SRA(uint16_t p1, uint16_t p2) {
    uint8_t rVal = GET_VAL(p1, p2);

    CLEAR_FLAGS;
    (rVal & 0x01)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);

    rVal >>= 1;

    if(rVal & 0x40)
        rVal |= 0x80;

    CHECK_ZERO(rVal);

    SET_VAL(p1, p2, rVal);
}

void Cpu::oph_SWAP(uint16_t p1, uint16_t p2) {
    uint8_t rVal = GET_VAL(p1, p2);

    rVal = (((rVal & 0xF) << 4) | (rVal >> 4));
    CLEAR_FLAGS;
    CHECK_ZERO(rVal);
}

void Cpu::oph_SRL(uint16_t p1, uint16_t p2) {
    uint8_t rVal = GET_VAL(p1, p2);

    CLEAR_FLAGS;
    (rVal & 0x01)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);

    rVal >>= 1;

    CHECK_ZERO(rVal);

    SET_VAL(p1, p2, rVal);
}

#define GET_BIT_FROM_OP(_base) ((mCurOpcode - _base) / 8)

void Cpu::oph_BIT(uint16_t p1, uint16_t p2) {
    uint8_t rVal = GET_VAL(p1, p2);
    CLEAR_FLAG(FLAG_N);
    SET_FLAG(FLAG_H);

    CHECK_ZERO((rVal & (1 << GET_BIT_FROM_OP(0x40))));
}

void Cpu::oph_RES(uint16_t p1, uint16_t p2) {
    uint8_t rVal = GET_VAL(p1, p2);

    rVal &= ~(1 << GET_BIT_FROM_OP(0x80));

    SET_VAL(p1, p2, rVal);
}

void Cpu::oph_SET(uint16_t p1, uint16_t p2) {
    uint8_t rVal = GET_VAL(p1, p2);

    rVal |= (1 << GET_BIT_FROM_OP(0xc0));

    SET_VAL(p1, p2, rVal);
}
