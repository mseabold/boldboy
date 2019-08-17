#ifndef CPU_H
#define CPU_H

#include "reg16.h"
#include "reg8.h"
#include "mmu.h"

#define NUM_R8  8
#define NUM_R16 6

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

    Cpu(Mmu *mmu);
    ~Cpu();

    typedef void (Cpu::*OpHandler)(uint16_t p1, uint16_t p2);

    inline Reg8 *getReg8(reg8_name name) { return mReg8s[name]; };
    inline Reg16 *getReg16(reg16_name name) { return mReg16s[name]; };

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

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

#define SET_FLAG(f)   mrF->write(mrF->read() | f)
#define CLEAR_FLAG(f) mrF->write(mrF->read() & ~f)
#define TOGGLE_FLAG(f) mrF->write(mrF->read() ^ f)
#define TEST_FLAG(f)  (mrF->read() & f)

#define CARRY_BITS(_a1, _a2, _r) (_a1 ^ _a2 ^ _r)
#define CARRY_BITS_3(_a1, _a2, _a3, _r) (_a1 ^ _a2 ^ _a3 ^ _r)

    Mmu *mMmu;

    Opcode mOpTable[256];
    Opcode mExtOpTable[256];

    void initOps(void);

    /* Helpers. */
    uint8_t add_3u8(uint8_t p1, uint8_t p2, uint8_t p3);
    void ret(bool enable_interrupts);

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
    void oph_RLC(uint16_t p1, uint16_t p2);
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
    void oph_ADC_A_r8(uint16_t p1, uint16_t p2);
    void oph_ADC_A_arHL(uint16_t p1, uint16_t p2);
    void oph_SUB_A_r8(uint16_t p1, uint16_t p2);
    void oph_SUB_A_arHL(uint16_t p1, uint16_t p2);
    void oph_SBC_A_r8(uint16_t p1, uint16_t p2);
    void oph_SBC_A_arHL(uint16_t p1, uint16_t p2);
    void oph_AND_r8(uint16_t p1, uint16_t p2);
    void oph_AND_arHL(uint16_t p1, uint16_t p2);
    void oph_XOR_r8(uint16_t p1, uint16_t p2);
    void oph_XOR_arHL(uint16_t p1, uint16_t p2);
    void oph_OR_r8(uint16_t p1, uint16_t p2);
    void oph_OR_arHL(uint16_t p1, uint16_t p2);
    void oph_CP_r8(uint16_t p1, uint16_t p2);
    void oph_CP_arHL(uint16_t p1, uint16_t p2);
    void oph_RET_NZ(uint16_t p1, uint16_t p2);
    void oph_RET_NC(uint16_t p1, uint16_t p2);
    void oph_RET_Z(uint16_t p1, uint16_t p2);
    void oph_RET_C(uint16_t p1, uint16_t p2);
    void oph_RET(uint16_t p1, uint16_t p2);
    void oph_RETI(uint16_t p1, uint16_t p2);

};

#endif /* CPU_H */
