#ifndef CPU_H
#define CPU_H

#include "reg16.h"
#include "reg8.h"
#include "mmu.h"
#include "int_controller.h"

#define NUM_R8  8
#define NUM_R16 6

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

#define SET_FLAG(f)   mrF->write(mrF->read() | (f))
#define CLEAR_FLAG(f) mrF->write(mrF->read() & ~(f))
#define TOGGLE_FLAG(f) mrF->write(mrF->read() ^ (f))
#define TEST_FLAG(f)  (mrF->read() & (f))
#define CLEAR_FLAGS   mrF->write(0)
#define CHECK_ZERO(_r) (_r == 0)?SET_FLAG(FLAG_Z):CLEAR_FLAG(FLAG_Z)

#define CARRY_BITS(_a1, _a2, _r) (_a1 ^ _a2 ^ _r)
#define CARRY_BITS_3(_a1, _a2, _a3, _r) (_a1 ^ _a2 ^ _a3 ^ _r)

#define ADVANCE_STATE(_cycles) do { mCycles = _cycles; ++mCurState; } while(0)
#define RESET_STATE(_cycles) do { mCycles = _cycles; mCurState = 0; } while(0)

#define CPU_STATE_FLAGS_IME_ENABLE_PENDING 0x00000001
#define CPU_STATE_FLAGS_IME_ENABLE_DELAY   0x00000002

class Cpu
{
public:
    enum reg8_name {
        rA,
        rF,
        rB,
        rC,
        rD,
        rE,
        rH,
        rL,
    };
    enum reg16_name {
        rAF,
        rBC,
        rDE,
        rHL,
        rSP,
        rPC,
    };

    Cpu(Mmu *mmu, InterruptController *ic);
    ~Cpu();

    typedef void (Cpu::*OpHandler)(uint16_t p1, uint16_t p2);

    inline Reg8 *getReg8(reg8_name name) { return mReg8s[name]; };
    inline Reg16 *getReg16(reg16_name name) { return mReg16s[name]; };

    uint8_t tick();
    void disassemble(char *buffer, uint32_t bufLen);

#ifdef ENABLE_TEST_HARNESS
    void setMagicOpcode(uint8_t opcode);
    bool magicOpcodeRun();
#endif

    class Opcode {
        public:
            Opcode();
            OpHandler handler;
            uint8_t cycles;
            uint8_t branchCycles;
            const char *mnemonic;
            uint16_t p1;
            uint16_t p2;

            void init(OpHandler _handler, uint8_t _cycles, const char *_mnemonic);
            void init(OpHandler _handler, uint8_t _cycles, uint8_t _branchCycles, const char *_mnemonic);
            void init(OpHandler _handler, uint8_t _cycles, const char *_mnemonic, uint16_t _p1);
            void init(OpHandler _handler, uint8_t _cycles, const char *_mnemonic, uint16_t _p1, uint16_t _p2);
            void init(OpHandler _handler, uint8_t _cycles, uint8_t _branchCycles, const char *_mnemonic, uint16_t _p1);
            void init(OpHandler _handler, uint8_t _cycles, uint8_t _branchCycles, const char *_mnemonic, uint16_t _p1, uint16_t _p2);
    };

private:
    Reg8 *mReg8s[NUM_R8];
    Reg16 *mReg16s[NUM_R16];
    Reg8 *mrA;
    Reg8 *mrF;
    Reg16 *mrPC;
    Reg16 *mrSP;
    Reg16 *mrHL;
    bool mBranchTaken;
    uint8_t mCurOpcode;
    bool mIsCB;
    bool mIsHalted;
    bool mHaltSkipPC;
    uint8_t mCurState;
    uint8_t mCycles;
    uint8_t mBrCycles;
    uint16_t mCache;
    uint32_t mStateFlags;

    Mmu *mMmu;
    InterruptController *mIC;

    Opcode mOpTable[256];
    Opcode mExtOpTable[256];

    void initOps(void);

    /* Helpers. */
    uint8_t add(uint8_t p1, uint8_t p2, bool checkC);
    uint8_t sub(uint8_t p1, uint8_t p2, bool checkC);
    uint16_t popStack_16(void);
    void pushStack_16(uint16_t val);
    uint8_t popStack8(void);
    void pushStack8(uint8_t val);
    void and_A(uint8_t param);
    void xor_A(uint8_t param);
    void or_A(uint8_t param);
    void procCallSubstate(bool branch);

    uint8_t rlc(uint8_t p1);
    uint8_t rrc(uint8_t p1);
    uint8_t rl(uint8_t p1);
    uint8_t rr(uint8_t p1);
    uint8_t sla(uint8_t p1);
    uint8_t sra(uint8_t p1);
    uint8_t swap(uint8_t p1);
    uint8_t srl(uint8_t p1);
    void bit(uint8_t p1);
    uint8_t res(uint8_t p1);
    uint8_t set(uint8_t p1);

    /* Opcode Handlers */
    void oph_Nop(uint16_t p1, uint16_t p2);
    void oph_Invalid(uint16_t p1, uint16_t p2);
    void oph_Stop(uint16_t p1, uint16_t p2);
    void oph_Halt(uint16_t p1, uint16_t p2);
    void oph_EI(uint16_t p1, uint16_t p2);
    void oph_DI(uint16_t p1, uint16_t p2);
    void oph_Prefix(uint16_t p1, uint16_t p2);
    void oph_LD_r16_d16(uint16_t p1, uint16_t p2);
    void oph_LD_ar16_A(uint16_t p1, uint16_t p2);
    void oph_Inc_r16(uint16_t p1, uint16_t p2);
    void oph_Dec_r16(uint16_t p1, uint16_t p2);
    void oph_Inc_r8(uint16_t p1, uint16_t p2);
    void oph_Dec_r8(uint16_t p1, uint16_t p2);
    void oph_Inc_arHL(uint16_t p1, uint16_t p2);
    void oph_Dec_arHL(uint16_t p1, uint16_t p2);
    void oph_LD_r8_d8(uint16_t p1, uint16_t p2);
    void oph_LD_arHL_d8(uint16_t p1, uint16_t p2);
    void oph_LD_a16_SP(uint16_t p1, uint16_t p2);
    void oph_ADD_HL_r16(uint16_t p1, uint16_t p2);
    void oph_LD_A_ar16(uint16_t p1, uint16_t p2);
    void oph_JR(uint16_t p1, uint16_t p2);
    void oph_JR_Z_NZ(uint16_t p1, uint16_t p2);
    void oph_JR_C_NC(uint16_t p1, uint16_t p2);
    void oph_DAA(uint16_t p1, uint16_t p2);
    void oph_CPL(uint16_t p1, uint16_t p2);
    void oph_SCF(uint16_t p1, uint16_t p2);
    void oph_CCF(uint16_t p1, uint16_t p2);
    void oph_LD_r8_r8(uint16_t p1, uint16_t p2);
    void oph_LD_r8_arHL(uint16_t p1, uint16_t p2);
    void oph_LD_arHL_r8(uint16_t p1, uint16_t p2);
    void oph_ADD_A_r8(uint16_t p1, uint16_t p2);
    void oph_ADD_A_arHL(uint16_t p1, uint16_t p2);
    void oph_ADD_A_d8(uint16_t p1, uint16_t p2);
    void oph_ADC_A_r8(uint16_t p1, uint16_t p2);
    void oph_ADC_A_arHL(uint16_t p1, uint16_t p2);
    void oph_ADC_A_d8(uint16_t p1, uint16_t p2);
    void oph_SUB_A_r8(uint16_t p1, uint16_t p2);
    void oph_SUB_A_arHL(uint16_t p1, uint16_t p2);
    void oph_SUB_A_d8(uint16_t p1, uint16_t p2);
    void oph_SBC_A_r8(uint16_t p1, uint16_t p2);
    void oph_SBC_A_arHL(uint16_t p1, uint16_t p2);
    void oph_SBC_A_d8(uint16_t p1, uint16_t p2);
    void oph_AND_r8(uint16_t p1, uint16_t p2);
    void oph_AND_arHL(uint16_t p1, uint16_t p2);
    void oph_AND_d8(uint16_t p1, uint16_t p2);
    void oph_XOR_r8(uint16_t p1, uint16_t p2);
    void oph_XOR_arHL(uint16_t p1, uint16_t p2);
    void oph_XOR_d8(uint16_t p1, uint16_t p2);
    void oph_OR_r8(uint16_t p1, uint16_t p2);
    void oph_OR_arHL(uint16_t p1, uint16_t p2);
    void oph_OR_d8(uint16_t p1, uint16_t p2);
    void oph_CP_r8(uint16_t p1, uint16_t p2);
    void oph_CP_arHL(uint16_t p1, uint16_t p2);
    void oph_CP_d8(uint16_t p1, uint16_t p2);
    void oph_RET_flag(uint16_t p1, uint16_t p2);
    void oph_RET(uint16_t p1, uint16_t p2);
    void oph_RETI(uint16_t p1, uint16_t p2);
    void oph_POP_r16(uint16_t p1, uint16_t p2);
    void oph_PUSH_r16(uint16_t p1, uint16_t p2);
    void oph_JP_flag_a16(uint16_t p1, uint16_t p2);
    void oph_JP_a16(uint16_t p1, uint16_t p2);
    void oph_JP_arHL(uint16_t p1, uint16_t p2);
    void oph_CALL_flag_a16(uint16_t p1, uint16_t p2);
    void oph_CALL_a16(uint16_t p1, uint16_t p2);
    void oph_RST_n(uint16_t p1, uint16_t p2);
    void oph_LDH_a8_A(uint16_t p1, uint16_t p2);
    void oph_LDH_A_a8(uint16_t p1, uint16_t p2);
    void oph_LD_arC_A(uint16_t p1, uint16_t p2);
    void oph_LD_A_arC(uint16_t p1, uint16_t p2);
    void oph_ADD_SP_r8(uint16_t p1, uint16_t p2);
    void oph_LD_a16_A(uint16_t p1, uint16_t p2);
    void oph_LD_A_a16(uint16_t p1, uint16_t p2);
    void oph_LD_HL_SP_p_r8(uint16_t p1, uint16_t p2);
    void oph_LD_SP_HL(uint16_t p1, uint16_t p2);
    void oph_RLC_r8(uint16_t p1, uint16_t p2);
    void oph_RRC_r8(uint16_t p1, uint16_t p2);
    void oph_RL_r8(uint16_t p1, uint16_t p2);
    void oph_RR_r8(uint16_t p1, uint16_t p2);
    void oph_SLA_r8(uint16_t p1, uint16_t p2);
    void oph_SRA_r8(uint16_t p1, uint16_t p2);
    void oph_SWAP_r8(uint16_t p1, uint16_t p2);
    void oph_SRL_r8(uint16_t p1, uint16_t p2);
    void oph_BIT_r8(uint16_t p1, uint16_t p2);
    void oph_RES_r8(uint16_t p1, uint16_t p2);
    void oph_SET_r8(uint16_t p1, uint16_t p2);
    void oph_RLC_arHL(uint16_t p1, uint16_t p2);
    void oph_RRC_arHL(uint16_t p1, uint16_t p2);
    void oph_RL_arHL(uint16_t p1, uint16_t p2);
    void oph_RR_arHL(uint16_t p1, uint16_t p2);
    void oph_SLA_arHL(uint16_t p1, uint16_t p2);
    void oph_SRA_arHL(uint16_t p1, uint16_t p2);
    void oph_SWAP_arHL(uint16_t p1, uint16_t p2);
    void oph_SRL_arHL(uint16_t p1, uint16_t p2);
    void oph_BIT_arHL(uint16_t p1, uint16_t p2);
    void oph_RES_arHL(uint16_t p1, uint16_t p2);
    void oph_SET_arHL(uint16_t p1, uint16_t p2);

#ifdef ENABLE_TEST_HARNESS
    uint8_t mMagicOpcode;
    bool mMagicOpcodeRun;
    bool mMagicOpcodeValid;
#endif

};

#endif /* CPU_H */
