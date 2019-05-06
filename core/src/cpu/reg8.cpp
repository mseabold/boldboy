#include "reg8.h"

Reg8::Reg8()
{
}

Reg8::~Reg8()
{
}

uint8_t Reg8::read(void) {
    return mVal;
}

void Reg8::write(uint8_t val) {
    mVal = val;
}
