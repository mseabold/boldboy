#ifndef CPU_H
#define CPU_H

#include "reg16.h"
#include "reg8.h"

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

    Cpu();
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

    void oph_Nop(uint16_t p1, uint16_t p2);
    void oph_Invalid(uint16_t p1, uint16_t p2);

    Opcode mOpTable[256];
    Opcode mCbOpTable[256];

    void initOps(void);
};

#endif /* CPU_H */
