#include <stdio.h>

#include "cpu.h"
#include "mmu.h"
#include "test_cart.h"
#include <string.h>
#include <stdlib.h>

#define MAX_ARGS 16
#define COMMAND_STR_SZ 2048

#define INVALID_COMMAND -1
#define INVALID_PARAMETER -2
#define EXIT_CODE -3

class EmptyCart : public Cartridge {
    public:
        EmptyCart() {}
        ~EmptyCart() {}
        uint8_t readAddr(uint16_t addr) { return 0; }
        void writeAddr(uint16_t addr, uint8_t val) {}
};

typedef struct _Arg_s {
    char *asStr;
    bool validInt;
    long asLong;
} Arg_t;

typedef struct _ArgList_s {
    unsigned int numArgs;
    Arg_t args[MAX_ARGS];
} ArgList_t;

typedef int (*CommandHandler)(ArgList_t *args);

typedef struct _Command_s {
    const char *command;
    const char *shortCommand;
    CommandHandler handler;
} Command_t;

static int getRegs(ArgList_t *args);
static int tick(ArgList_t *args);

static Command_t commandList[] = {
    { "regs", NULL, getRegs },
    { "tick", "t", tick },
};

static Cpu *cpu;
static Mmu *mmu;

#define NUM_COMMANDS sizeof(commandList)/sizeof(Command_t)

static int parseCommand(char *command) {
    char *tok;

    tok = strtok(command, " \n");

    if(strlen(tok) == 0)
        return INVALID_COMMAND;

    int i;
    for(i=0;i<NUM_COMMANDS;++i) {
        if(commandList[i].shortCommand && commandList[i].shortCommand[0] == tok[0])
            break;

        if(!strncmp(commandList[i].command, tok, strlen(command)))
            break;
    }

    if(i == NUM_COMMANDS)
        return INVALID_COMMAND;

    char *endptr;
    ArgList_t args;
    args.numArgs = 0;
    long val;

    while((tok = strtok(NULL, " \n")) != NULL) {
        args.args[args.numArgs].asStr = tok;
        val = strtol(tok, &endptr, -1);

        if((endptr) && (*endptr == 0)) {
            args.args[args.numArgs].validInt = true;
            args.args[args.numArgs].asLong = val;
        }

        if(++args.numArgs == MAX_ARGS)
            return INVALID_COMMAND;
    }

    return commandList[i].handler(&args);
}

static int getRegs(ArgList_t *args) {
    printf("BC: 0x%04x\tDE: 0x%04x\n", cpu->getReg16(Cpu::rBC)->read(), cpu->getReg16(Cpu::rDE)->read());
    printf("AF: 0x%04x\tHL: 0x%04x\n", cpu->getReg16(Cpu::rAF)->read(), cpu->getReg16(Cpu::rHL)->read());
    printf("SP: 0x%04x\tPC: 0x%04x\n", cpu->getReg16(Cpu::rSP)->read(), cpu->getReg16(Cpu::rPC)->read());

    return 0;
}

static int tick(ArgList_t *args) {
    char disBuf[512];
    cpu->disassemble(disBuf, sizeof(disBuf));
    printf("%s\n", disBuf);
    cpu->tick();
    return 0;
}

int main(int argc, char *argv[])
{
    mmu = new Mmu(new EmptyCart());
    cpu =  new Cpu(mmu);
    char command[COMMAND_STR_SZ];

    int exit = 0;
    while(exit != EXIT_CODE) {
        printf(">");
        fflush(stdout);

        if(fgets(command, COMMAND_STR_SZ, stdin)) {
            exit = parseCommand(command);
        }
        else
            exit = EXIT_CODE;
    }

    return 0;
}
