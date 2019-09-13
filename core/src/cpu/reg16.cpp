#include "reg16.h"

Reg16::Reg16(Reg8 *high, Reg8 *low)
{
    mLow = low;
    mHigh = high;
}

Reg16::~Reg16()
{
}

void Reg16::write(uint16_t val) {
    mLow->write(val & 0xFF);
    mHigh->write(val >> 8);
}

uint16_t Reg16::read(void) {
    return (mHigh->read() << 8) | mLow->read();
}

Reg8 *Reg16::getLow(void) {
    return mLow;
}

Reg8 *Reg16::getHigh() {
    return mHigh;
}

uint16_t Reg16::increment(void) {
    uint16_t val = (mHigh->read() << 8) | mLow->read();
    ++val;
    mLow->write(val & 0xFF);
    mHigh->write(val >> 8);
    return val;
}

uint16_t Reg16::decrement(void) {
    uint16_t val = (mHigh->read() << 8) | mLow->read();
    --val;
    mLow->write(val & 0xFF);
    mHigh->write(val >> 8);
    return val;
}

uint16_t Reg16::postIncrement(void) {
    uint16_t val = (mHigh->read() << 8) | mLow->read();
    mLow->write((val+1) & 0xFF);
    mHigh->write((val+1) >> 8);
    return val;
}

uint16_t Reg16::postDecrement(void) {
    uint16_t val = (mHigh->read() << 8) | mLow->read();
    --val;
    mLow->write((val-1) & 0xFF);
    mHigh->write((val-1) >> 8);
    return val;
}
