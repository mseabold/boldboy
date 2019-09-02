#include <stdio.h>

#include "cpu.h"
#include "mmu.h"
#include "test_cart.h"

class EmptyCart : public Cartridge {
    public:
        EmptyCart() {}
        ~EmptyCart() {}
        uint8_t readAddr(uint16_t addr) { return 0; }
        void writeAddr(uint16_t addr, uint8_t val) {}
};
int main(int argc, char *argv[])
{

    Cpu *cpu = new Cpu(new Mmu(new EmptyCart()));

    while(true)
        cpu->tick();

    return 0;
}
