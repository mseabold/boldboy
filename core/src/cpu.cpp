#include "cpu.h"

Cpu::Cpu()
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
}

Cpu::~Cpu()
{
    for(int i=0;i<NUM_R16;i++)
        delete mReg16s[i];

    for(int i=0;i<NUM_R8;i++)
        delete mReg8s[i];

    delete mReg16s[rPC]->getLow();
    delete mReg16s[rPC]->getHigh();
    delete mReg16s[rPC];

    delete mReg16s[rSP]->getLow();
    delete mReg16s[rSP]->getHigh();
    delete mReg16s[rSP];
}
