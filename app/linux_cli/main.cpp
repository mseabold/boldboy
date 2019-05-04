#include <stdio.h>

#include "cpu.h"

int main(int argc, char *argv[])
{
    Cpu *cpu = new Cpu();

    //Test 8 bit writes/16 bit read
    cpu->getReg8(Cpu::rA)->write((uint8_t)0xaa);
    cpu->getReg8(Cpu::rF)->write((uint8_t)0xff);
    printf("rAF: 0x%04x\n", cpu->getReg16(Cpu::rAF)->read());

    //Test 16 bit write/8 bit reads
    cpu->getReg16(Cpu::rBC)->write((uint16_t)0xaaff);
    printf("rB: 0x%02x\n", cpu->getReg8(Cpu::rB)->read());
    printf("rC: 0x%02x\n", cpu->getReg8(Cpu::rC)->read());
    return 0;
}
