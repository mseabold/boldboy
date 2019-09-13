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

uint8_t Reg8::increment(void) {
    return ++mVal;
}

uint8_t Reg8::decrement(void) {
    return --mVal;
}

uint8_t Reg8::postIncrement(void) {
    return mVal++;
}

uint8_t Reg8::postDecrement(void) {
    return mVal--;
}
