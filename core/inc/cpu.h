#ifndef CPU_H
#define CPU_H

#include "reg16.h"
#include "reg8.h"
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
        NUM_R8
    };
    enum reg16_name {
        rAF,
        rBC,
        rDE,
        rHL,
        rSP,
        rPC,
        NUM_R16
    };
    Cpu();
    ~Cpu();

    inline Reg8 *getReg8(reg8_name name) { return mReg8s[name]; };
    inline Reg16 *getReg16(reg16_name name) { return mReg16s[name]; };
private:
    Reg8 *mReg8s[NUM_R8];
    Reg16 *mReg16s[NUM_R16];
};

#endif /* CPU_H */
