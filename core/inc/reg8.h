#ifndef REG8_H
#define REG8_H

#include <stdint.h>

class Reg8
{
private:
    uint8_t mVal;

public:
    Reg8();
    virtual ~Reg8();

    void write(uint8_t val);
    uint8_t read(void);
    uint8_t increment(void);
    uint8_t decrement(void);
    uint8_t postIncrement(void);
    uint8_t postDecrement(void);
};

#endif /* REG8_H */
