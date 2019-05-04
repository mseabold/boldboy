#ifndef REG16_H
#define REG16_H

#include <stdint.h>

#include "reg8.h"

class Reg16
{
private:
    Reg8 *mLow;
    Reg8 *mHigh;

public:
    Reg16(Reg8 *high, Reg8 *low);
    ~Reg16();

    void write(uint16_t val);
    uint16_t read(void);
    Reg8 *getLow(void);
    Reg8 *getHigh(void);
};

#endif /* REG16_H */
