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
    mIsHalted = true;
}

void Cpu::oph_EI(uint16_t p1, uint16_t p2) {
    if(!(mStateFlags & CPU_STATE_FLAGS_IME_ENABLE_PENDING))
        mStateFlags |= (CPU_STATE_FLAGS_IME_ENABLE_PENDING | CPU_STATE_FLAGS_IME_ENABLE_DELAY);
}

void Cpu::oph_DI(uint16_t p1, uint16_t p2) {
    mIC->setEnabled(false);
}

void Cpu::oph_Prefix(uint16_t p1, uint16_t p2) {
}

void Cpu::oph_LD_r16_d16(uint16_t p1, uint16_t p2) {
    mReg16s[p1]->getLow()->write(mMmu->readAddr(mrPC->read()));
    mrPC->increment();
    mReg16s[p1]->getHigh()->write(mMmu->readAddr(mrPC->read()));
    mrPC->increment();
}

void Cpu::oph_LD_ar16_A(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

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
    ((newVal & 0xf) == 0xf)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);

    SET_FLAG(FLAG_N);
}

void Cpu::oph_Inc_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 4;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            mCache = mMmu->readAddr(mrHL->read());
            ++mCurState;
            break;
        case 2:
            ++mCache;

            mMmu->writeAddr(mrHL->read(), mCache);

            CHECK_ZERO(mCache);
            ((mCache & 0xf) == 0)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);
            CLEAR_FLAG(FLAG_N);

            mCycles = 4;
            mCurState = 0;
            break;
    }
}

void Cpu::oph_Dec_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 4;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            mCache = mMmu->readAddr(mrHL->read());
            ++mCurState;
            break;
        case 2:
            --mCache;

            mMmu->writeAddr(mrHL->read(), mCache);

            CHECK_ZERO(mCache);
            ((mCache & 0xf) == 0xf)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);
            SET_FLAG(FLAG_N);

            mCycles = 4;
            mCurState = 0;
            break;
    }
}

void Cpu::oph_LD_r8_d8(uint16_t p1, uint16_t p2) {
    mReg8s[p1]->write(mMmu->readAddr(mrPC->read()));
    mrPC->increment();
}

void Cpu::oph_LD_arHL_d8(uint16_t p1, uint16_t p2) {
    if(mCurState == 0) {
        mCycles = 8;
        mCurState = 1;
        return;
    }

    mCycles = 4;
    mCurState = 0;

    mMmu->writeAddr(mrHL->read(), mMmu->readAddr(mrPC->read()));
    mrPC->increment();
}

void Cpu::oph_LD_a16_SP(uint16_t p1, uint16_t p2) {
    uint16_t addr;

    // Read in the 2 address bytes in LE
    addr = mMmu->readAddr(mrPC->postIncrement());
    addr += mMmu->readAddr(mrPC->postIncrement()) << 8;

    // Now write out SP into 2 bytes in LE
    mMmu->writeAddr(addr, mrSP->getLow()->read());
    mMmu->writeAddr(addr+1, mrSP->getHigh()->read());
}

void Cpu::oph_ADD_HL_r16(uint16_t p1, uint16_t p2) {
    uint32_t hlVal = mrHL->read();
    uint32_t pVal = mReg16s[p1]->read();
    uint32_t result = hlVal + pVal;

    CLEAR_FLAG(FLAG_N);
    (result & 0x10000)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);

    (CARRY_BITS(hlVal, pVal, (result & 0xffff)) & 0x1000)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);

    mrHL->write((uint16_t)result);
}

void Cpu::oph_LD_A_ar16(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

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
    uint16_t pcVal = mrPC->postIncrement();
    int8_t rVal = (int8_t)mMmu->readAddr(pcVal);

    // Account for the fact that we did a postincrement
    mrPC->write(pcVal + rVal + 1);
}

void Cpu::oph_JR_Z_NZ(uint16_t p1, uint16_t p2) {
    if(TEST_FLAG(FLAG_Z) == p1) {
        uint16_t pcVal = mrPC->postIncrement();
        int8_t rVal = (int8_t)mMmu->readAddr(pcVal);
        mrPC->write(pcVal + rVal + 1);
        mBranchTaken = true;
    } else {
        mrPC->increment();
        mBranchTaken = false;
    }
}

void Cpu::oph_JR_C_NC(uint16_t p1, uint16_t p2) {
    if(TEST_FLAG(FLAG_C) == p1) {
        uint16_t pcVal = mrPC->postIncrement();
        int8_t rVal = (int8_t)mMmu->readAddr(pcVal);
        mrPC->write(pcVal + rVal + 1);
        mBranchTaken = true;
    } else {
        mrPC->increment();
        mBranchTaken = false;
    }
}
void Cpu::oph_DAA(uint16_t p1, uint16_t p2) {
    int aVal = mrA->read();
    if(TEST_FLAG(FLAG_N)) {
        // The substraction case only cares about checking the flags
        if(TEST_FLAG(FLAG_H))
            aVal = (aVal - 0x06) & 0xff;
        if(TEST_FLAG(FLAG_C))
            aVal -= 0x60;
    } else {
        // If the previous operation carried, or the result caused/will
        // cause the top nibble to wrap, update it.
        // Note that >x99 check accounts for if the top nibble
        // already needs adjusting OR the bottom nibble will
        // be adjusted, then in turn cause the top nibble to need
        // adjusting.
        if(TEST_FLAG(FLAG_C) || aVal > 0x99) {
            aVal += 0x60;
        }

        // Now handle the lower nibble if it wrapped or had a half carry
        if(TEST_FLAG(FLAG_H) || (aVal & 0x0f) > 0x09) {
            aVal += 0x06;
        }

    }


    if(aVal & 0x100)
        SET_FLAG(FLAG_C);
    aVal = aVal & 0xff;
    CLEAR_FLAG(FLAG_H);
    CHECK_ZERO(aVal);

    mrA->write(aVal);
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
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    mReg8s[p1]->write(mMmu->readAddr(mrHL->read()));
}

void Cpu::oph_LD_arHL_r8(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    mMmu->writeAddr(mrHL->read(), mReg8s[p1]->read());
}

void Cpu::oph_ADD_A_r8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mReg8s[p1]->read();

    CLEAR_FLAG(FLAG_N);

    mrA->write(add(aVal, rVal, false));
}

void Cpu::oph_ADD_A_arHL(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    uint8_t aVal = mrA->read();
    uint8_t rVal = mMmu->readAddr(mrHL->read());

    CLEAR_FLAG(FLAG_N);
    mrA->write(add(aVal, rVal, false));
}

void Cpu::oph_ADD_A_d8(uint16_t p1, uint16_t p2) {
    uint8_t dVal = mMmu->readAddr(mrPC->postIncrement());
    uint8_t aVal = mrA->read();

    CLEAR_FLAG(FLAG_N);
    mrA->write(add(dVal, aVal, false));
}

void Cpu::oph_ADC_A_r8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mReg8s[p1]->read();

    CLEAR_FLAG(FLAG_N);
    mrA->write(add(aVal, rVal, true));
}

void Cpu::oph_ADC_A_arHL(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    uint8_t aVal = mrA->read();
    uint8_t rVal = mMmu->readAddr(mrHL->read());

    CLEAR_FLAG(FLAG_N);
    mrA->write(add(aVal, rVal, true));
}

void Cpu::oph_ADC_A_d8(uint16_t p1, uint16_t p2) {
    uint8_t dVal = mMmu->readAddr(mrPC->postIncrement());
    uint8_t aVal = mrA->read();

    CLEAR_FLAG(FLAG_N);
    mrA->write(add(dVal, aVal, true));
}

void Cpu::oph_SUB_A_r8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mReg8s[p1]->read();

    SET_FLAG(FLAG_N);
    mrA->write(sub(aVal, rVal, false));
}

void Cpu::oph_SUB_A_arHL(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    uint8_t aVal = mrA->read();
    uint8_t rVal = mMmu->readAddr(mrHL->read());

    SET_FLAG(FLAG_N);
    mrA->write(sub(aVal, rVal, false));
}

void Cpu::oph_SUB_A_d8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t dVal = mMmu->readAddr(mrPC->postIncrement());

    SET_FLAG(FLAG_N);
    mrA->write(sub(aVal, dVal, false));
}

void Cpu::oph_SBC_A_r8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mReg8s[p1]->read();

    SET_FLAG(FLAG_N);
    mrA->write(sub(aVal, rVal, true));
}

void Cpu::oph_SBC_A_arHL(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    uint8_t aVal = mrA->read();
    uint8_t rVal = mMmu->readAddr(mrHL->read());

    SET_FLAG(FLAG_N);
    mrA->write(sub(aVal, rVal, true));
}

void Cpu::oph_SBC_A_d8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t dVal = mMmu->readAddr(mrPC->postIncrement());

    SET_FLAG(FLAG_N);
    mrA->write(sub(aVal, dVal, true));
}

void Cpu::oph_AND_r8(uint16_t p1, uint16_t p2) {
    and_A(mReg8s[p1]->read());
}

void Cpu::oph_AND_arHL(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    and_A(mMmu->readAddr(mrHL->read()));
}

void Cpu::oph_AND_d8(uint16_t p1, uint16_t p2) {
    and_A(mMmu->readAddr(mrPC->postIncrement()));
}

void Cpu::oph_XOR_r8(uint16_t p1, uint16_t p2) {
    xor_A(mReg8s[p1]->read());
}

void Cpu::oph_XOR_arHL(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    xor_A(mMmu->readAddr(mrHL->read()));
}

void Cpu::oph_XOR_d8(uint16_t p1, uint16_t p2) {
    xor_A(mMmu->readAddr(mrPC->postIncrement()));
}

void Cpu::oph_OR_r8(uint16_t p1, uint16_t p2) {
    or_A(mReg8s[p1]->read());
}

void Cpu::oph_OR_arHL(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    or_A(mMmu->readAddr(mrHL->read()));
}

void Cpu::oph_OR_d8(uint16_t p1, uint16_t p2) {
    or_A(mMmu->readAddr(mrPC->postIncrement()));
}

void Cpu::oph_CP_r8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t rVal = mReg8s[p1]->read();

    SET_FLAG(FLAG_N);

    // Simply use the subtraction subroutine, but throw away the result.
    // This will set all required flags
    sub(aVal, rVal, false);
}

void Cpu::oph_CP_arHL(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    uint8_t aVal = mrA->read();
    uint8_t rVal = mMmu->readAddr(mrHL->read());

    SET_FLAG(FLAG_N);

    // Simply use the subtraction subroutine, but throw away the result.
    // This will set all required flags
    sub(aVal, rVal, false);
}

void Cpu::oph_CP_d8(uint16_t p1, uint16_t p2) {
    uint8_t aVal = mrA->read();
    uint8_t dVal = mMmu->readAddr(mrPC->postIncrement());

    SET_FLAG(FLAG_N);

    // Simply use the subtraction subroutine, but throw away the result.
    // This will set all required flags
    sub(aVal, dVal, false);
}

void Cpu::oph_RET_flag(uint16_t p1, uint16_t p2) {
    if(p2 == TEST_FLAG(p1)) {
        mBranchTaken = true;
        oph_RET(0,0);
    }
}

void Cpu::oph_RET(uint16_t p1, uint16_t p2) {
    mrPC->write(popStack_16());
}

void Cpu::oph_RETI(uint16_t p1, uint16_t p2) {
    oph_RET(0,0);
    mIC->setEnabled(true);
}

void Cpu::oph_POP_r16(uint16_t p1, uint16_t p2) {
    // AF should mask off the lower 4 bits of F
    if(p1 == rAF)
        mReg16s[p1]->write(popStack_16() & 0xfff0);
    else
        mReg16s[p1]->write(popStack_16());
}

void Cpu::oph_PUSH_r16(uint16_t p1, uint16_t p2) {
    pushStack_16(mReg16s[p1]->read());
}

void Cpu::oph_JP_flag_a16(uint16_t p1, uint16_t p2) {
    if(p2 == TEST_FLAG(p1)) {
        mBranchTaken = true;
        oph_JP_a16(0,0);
    } else {
        mrPC->increment();
        mrPC->increment();
    }
}

void Cpu::oph_JP_a16(uint16_t p1, uint16_t p2) {
    uint16_t pcVal;

    pcVal = mMmu->readAddr(mrPC->postIncrement());
    pcVal |= (mMmu->readAddr(mrPC->postIncrement()) << 8);

    mrPC->write(pcVal);
}

void Cpu::oph_JP_arHL(uint16_t p1, uint16_t p2) {
    mrPC->write(mrHL->read());
}

void Cpu::oph_CALL_flag_a16(uint16_t p1, uint16_t p2) {
    procCallSubstate((p2 == TEST_FLAG(p1)));
}

void Cpu::oph_CALL_a16(uint16_t p1, uint16_t p2) {
    procCallSubstate(true);
}

void Cpu::oph_RST_n(uint16_t p1, uint16_t p2) {
    pushStack_16(mrPC->read());
    mrPC->write(p1);
}

void Cpu::oph_LDH_a8_A(uint16_t p1, uint16_t p2) {
    if(mCurState == 0) {
        mCycles = 8;
        mCurState = 1;
        return;
    }

    mCycles = 4;
    mCurState = 0;

    uint8_t aVal = mMmu->readAddr(mrPC->postIncrement());
    mMmu->writeAddr(0xFF00 + aVal, mrA->read());
}

void Cpu::oph_LDH_A_a8(uint16_t p1, uint16_t p2) {
    if(mCurState == 0) {
        mCycles = 8;
        mCurState = 1;
        return;
    }

    mCycles = 4;
    mCurState = 0;

    uint8_t aVal = mMmu->readAddr(mrPC->postIncrement());
    mrA->write(mMmu->readAddr(0xFF00 + aVal));
}

void Cpu::oph_LD_arC_A(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    mMmu->writeAddr(0xFF00 + mReg8s[rC]->read(), mrA->read());
}

void Cpu::oph_LD_A_arC(uint16_t p1, uint16_t p2) {
    mCycles = 4;

    if(mCurState == 0) {
        mCurState = 1;
        return;
    }

    mCurState = 0;

    mrA->write(mMmu->readAddr(0xFF00 + mReg8s[rC]->read()));
}

void Cpu::oph_ADD_SP_r8(uint16_t p1, uint16_t p2) {
    if(mCurState == 0) {
        mCycles = 4;
        mCurState = 1;
        return;
    }

    mCurState = 0;
    mCycles = 12;

    uint16_t aVal = (int16_t)(int8_t)mMmu->readAddr(mrPC->postIncrement());
    uint16_t rVal = mrSP->read();

    VLOG(ZONE_CPU, "Add SP r8! (0x%02x)\n", aVal);

    uint16_t r = aVal + rVal;

    CLEAR_FLAG(FLAG_Z | FLAG_N);
    (CARRY_BITS(aVal, rVal, r) & 0x10)?SET_FLAG(FLAG_H):CLEAR_FLAG(FLAG_H);
    (CARRY_BITS(aVal, rVal, r) & 0x100)?SET_FLAG(FLAG_C):CLEAR_FLAG(FLAG_C);
    mrSP->write(r);
}

void Cpu::oph_LD_a16_A(uint16_t p1, uint16_t p2) {
    if(mCurState == 0) {
        mCycles = 12;
        mCurState = 1;
        return;
    }

    mCycles = 4;
    mCurState = 0;

    uint16_t aVal = mMmu->readAddr(mrPC->postIncrement());
    aVal |= mMmu->readAddr(mrPC->postIncrement()) << 8;

    mMmu->writeAddr(aVal, mrA->read());
}

void Cpu::oph_LD_A_a16(uint16_t p1, uint16_t p2) {
    if(mCurState == 0) {
        mCycles = 12;
        mCurState = 1;
        return;
    }

    mCycles = 4;
    mCurState = 0;

    uint16_t aVal = mMmu->readAddr(mrPC->postIncrement());
    aVal |= mMmu->readAddr(mrPC->postIncrement()) << 8;

    mrA->write(mMmu->readAddr(aVal));
}

void Cpu::oph_LD_HL_SP_p_r8(uint16_t p1, uint16_t p2) {
    uint16_t aVal = (int16_t)(int8_t)mMmu->readAddr(mrPC->postIncrement());
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

void Cpu::oph_RLC_r8(uint16_t p1, uint16_t p2) {
    uint8_t rVal = rlc(mReg8s[p1]->read());

    if(mIsCB)
        CHECK_ZERO(rVal);

    mReg8s[p1]->write(rVal);
}

void Cpu::oph_RRC_r8(uint16_t p1, uint16_t p2) {
    uint8_t rVal = rrc(mReg8s[p1]->read());

    if(mIsCB)
        CHECK_ZERO(rVal);

    mReg8s[p1]->write(rVal);
}

void Cpu::oph_RL_r8(uint16_t p1, uint16_t p2) {
    uint8_t rVal = rl(mReg8s[p1]->read());

    if(mIsCB)
        CHECK_ZERO(rVal);

    mReg8s[p1]->write(rVal);
}

void Cpu::oph_RR_r8(uint16_t p1, uint16_t p2) {
    uint8_t rVal = rr(mReg8s[p1]->read());

    if(mIsCB)
        CHECK_ZERO(rVal);

    mReg8s[p1]->write(rVal);
}

void Cpu::oph_SLA_r8(uint16_t p1, uint16_t p2) {
    mReg8s[p1]->write(sla(mReg8s[p1]->read()));
}

void Cpu::oph_SRA_r8(uint16_t p1, uint16_t p2) {
    mReg8s[p1]->write(sra(mReg8s[p1]->read()));
}

void Cpu::oph_SWAP_r8(uint16_t p1, uint16_t p2) {
    mReg8s[p1]->write(swap(mReg8s[p1]->read()));
}

void Cpu::oph_SRL_r8(uint16_t p1, uint16_t p2) {
    mReg8s[p1]->write(srl(mReg8s[p1]->read()));
}

void Cpu::oph_BIT_r8(uint16_t p1, uint16_t p2) {
    bit(mReg8s[p1]->read());
}

void Cpu::oph_RES_r8(uint16_t p1, uint16_t p2) {
    mReg8s[p1]->write(res(mReg8s[p1]->read()));
}

void Cpu::oph_SET_r8(uint16_t p1, uint16_t p2) {
    mReg8s[p1]->write(set(mReg8s[p1]->read()));
}

void Cpu::oph_RLC_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            ++mCurState;
            mCache = mMmu->readAddr(mrHL->read());
            break;
        case 2:
            mCycles = 4;
            mCurState = 0;
            mCache = rlc(mCache);
            CHECK_ZERO(mCache);
            mMmu->writeAddr(mrHL->read(), mCache);
            break;
    }
}

void Cpu::oph_RRC_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            ++mCurState;
            mCache = mMmu->readAddr(mrHL->read());
            break;
        case 2:
            mCycles = 4;
            mCurState = 0;
            mCache = rrc(mCache);
            CHECK_ZERO(mCache);
            mMmu->writeAddr(mrHL->read(), mCache);
            break;
    }
}

void Cpu::oph_RL_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            ++mCurState;
            mCache = mMmu->readAddr(mrHL->read());
            break;
        case 2:
            mCycles = 4;
            mCurState = 0;
            mCache = rl(mCache);
            CHECK_ZERO(mCache);
            mMmu->writeAddr(mrHL->read(), mCache);
            break;
    }
}

void Cpu::oph_RR_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            ++mCurState;
            mCache = mMmu->readAddr(mrHL->read());
            break;
        case 2:
            mCycles = 4;
            mCurState = 0;
            mCache = rr(mCache);
            CHECK_ZERO(mCache);
            mMmu->writeAddr(mrHL->read(), mCache);
            break;
    }
}

void Cpu::oph_SLA_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            ++mCurState;
            mCache = mMmu->readAddr(mrHL->read());
            break;
        case 2:
            mCycles = 4;
            mCurState = 0;
            mCache = sla(mCache);
            mMmu->writeAddr(mrHL->read(), mCache);
            break;
    }
}

void Cpu::oph_SRA_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            ++mCurState;
            mCache = mMmu->readAddr(mrHL->read());
            break;
        case 2:
            mCycles = 4;
            mCurState = 0;
            mCache = sra(mCache);
            mMmu->writeAddr(mrHL->read(), mCache);
            break;
    }
}

void Cpu::oph_SWAP_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            ++mCurState;
            mCache = mMmu->readAddr(mrHL->read());
            break;
        case 2:
            mCycles = 4;
            mCurState = 0;
            mCache = swap(mCache);
            mMmu->writeAddr(mrHL->read(), mCache);
            break;
    }
}

void Cpu::oph_SRL_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            ++mCurState;
            mCache = mMmu->readAddr(mrHL->read());
            break;
        case 2:
            mCycles = 4;
            mCurState = 0;
            mCache = srl(mCache);
            mMmu->writeAddr(mrHL->read(), mCache);
            break;
    }
}

void Cpu::oph_BIT_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            mCurState = 0;
            bit(mMmu->readAddr(mrHL->read()));
            break;
    }
}

void Cpu::oph_RES_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            ++mCurState;
            mCache = mMmu->readAddr(mrHL->read());
            break;
        case 2:
            mCycles = 4;
            mCurState = 0;
            mCache = res(mCache);
            mMmu->writeAddr(mrHL->read(), mCache);
            break;
    }
}

void Cpu::oph_SET_arHL(uint16_t p1, uint16_t p2) {
    switch(mCurState) {
        case 0:
            mCycles = 8;
            ++mCurState;
            break;
        case 1:
            mCycles = 4;
            ++mCurState;
            mCache = mMmu->readAddr(mrHL->read());
            break;
        case 2:
            mCycles = 4;
            mCurState = 0;
            mCache = set(mCache);
            mMmu->writeAddr(mrHL->read(), mCache);
            break;
    }
}
