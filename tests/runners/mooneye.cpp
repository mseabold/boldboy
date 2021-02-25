#include "boldboy.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int result;
    if(argc <= 1)
    {
        fprintf(stderr, "Usage: mooneye_runner test_rom\n");
        return 1;
    }

    Cpu *cpu;
    Boldboy *emulator = new Boldboy(true);

    cpu = emulator->getCpu();
    cpu->setMagicOpcode(0x40 /* LD B,B */);

    if(!emulator->loadCart(argv[1])) {
        delete emulator;
        fprintf(stderr, "Unable to open %s\n", argv[1]);
        return 1;
    }

    /* TODO Timeout */
    while(!cpu->magicOpcodeRun()) {
        emulator->tick();
    }

    if(cpu->getReg8(Cpu::rB)->read() == 3 && cpu->getReg8(Cpu::rC)->read() == 5 && cpu->getReg8(Cpu::rD)->read() == 8 && cpu->getReg8(Cpu::rE)->read() == 13 && cpu->getReg8(Cpu::rH)->read() == 21 && cpu->getReg8(Cpu::rL)->read() == 34)
    {
        result = 0;
        printf("Success\n");
    }
    else
    {
        printf("Failure\n");
        result = 1;
    }


    delete emulator;

    return result;
}
