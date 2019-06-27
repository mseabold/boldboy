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
            void init(OpHandler _handler, uint8_t _cycles, uint8_t _branchCycles, const char *_mnemonic, uint16_t _p1, uint16_t _p2);
    };

private:
    Reg8 *mReg8s[NUM_R8];
    Reg16 *mReg16s[NUM_R16];
    Reg8 *mrA;
    Reg8 *mrF;
    Reg16 *mrPC;

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

#define SET_FLAG(f) mrF->write(mrF->read() | f)
#define CLEAR_FLAG(f) mrF->write(mrF->read() & ~f)

    Mmu *mMmu;

    Opcode mOpTable[256];
    Opcode mCbOpTable[256];

    void initOps(void);

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
    void oph_Inc_r8(uint16_t p1, uint16_t p2);

};

#endif /* CPU_H */
