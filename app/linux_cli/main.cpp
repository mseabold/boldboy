#include <stdio.h>

#include "cpu.h"
#include "mmu.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "serial_console.h"
#include "io_regs.h"
#include "io_controller.h"
#include "int_controller.h"
#include "io_timer.h"
#include "cart_rom.h"
#include "boldboy.h"
#include "console_logger.h"

#define MAX_ARGS 16
#define COMMAND_STR_SZ 2048

#define INVALID_COMMAND -1
#define INVALID_PARAMETER -2
#define NO_CPU -3
#define EXIT_CODE -4

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
static int writeSerial(ArgList_t *args);
static int loadCart(ArgList_t *args);
static int run(ArgList_t *args);
static int quiet(ArgList_t *args);

static Command_t commandList[] = {
    { "regs", NULL, getRegs },
    { "tick", "t", tick },
    { "writeserial", NULL, writeSerial },
    { "loadcart", NULL, loadCart },
    { "run", NULL, run },
    { "quiet", NULL, quiet },
};

static Boldboy *mEmu;
static char *lastCmd;
static char lastCmdBuf[COMMAND_STR_SZ];

#define NUM_COMMANDS sizeof(commandList)/sizeof(Command_t)

static int parseCommand(char *command) {
    char *tok;

    if(!strlen(command) && lastCmd)
        command = lastCmd;

    tok = strtok(command, " \n");

    if(!tok)
        return INVALID_COMMAND;

    if(strlen(tok) == 0)
        return INVALID_COMMAND;

    int i;
    for(i=0;i<NUM_COMMANDS;++i) {
        if(commandList[i].shortCommand && commandList[i].shortCommand[0] == tok[0])
            break;

        if(!strncasecmp(commandList[i].command, tok, strlen(command)))
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

    lastCmd = lastCmdBuf;
    strncpy(lastCmd, command, COMMAND_STR_SZ);

    return commandList[i].handler(&args);
}

static int getRegs(ArgList_t *args) {
#if 0
    if(!cpu)
        return NO_CPU;

    printf("BC: 0x%04x\tDE: 0x%04x\n", cpu->getReg16(Cpu::rBC)->read(), cpu->getReg16(Cpu::rDE)->read());
    printf("AF: 0x%04x\tHL: 0x%04x\n", cpu->getReg16(Cpu::rAF)->read(), cpu->getReg16(Cpu::rHL)->read());
    printf("SP: 0x%04x\tPC: 0x%04x\n", cpu->getReg16(Cpu::rSP)->read(), cpu->getReg16(Cpu::rPC)->read());
#endif

    return 0;
}

static int tick(ArgList_t *args) {
    mEmu->tick();
    return 0;
}

static int writeSerial(ArgList_t *args) {
    return 0;
}

static int loadcart_int(char *file) {
    return 0;
}

static int loadCart(ArgList_t *args) {
    if(args->numArgs < 1)
        return INVALID_PARAMETER;

    return loadcart_int(args->args[0].asStr);
}

static bool debugOp = false;

static int run(ArgList_t *args) {
    mEmu->run();
    return 0;
}

static int quiet(ArgList_t *args) {
    if(args->numArgs > 0 && args->args[0].asLong == 0)
        debugOp = false;
    else
        debugOp = true;
    return 0;
}

int main(int argc, char *argv[])
{
    char command[COMMAND_STR_SZ];

    mEmu = new Boldboy();
    Logger *l = new ConsoleLogger();
    l->setLevel(LOG_VERBOSE);
    mEmu->setLogger(l);
    mEmu->setSerialHandler(new SerialConsole());

    if(argc >= 2) {
        if(mEmu->loadCart(argv[1]))
            printf("Loaded cart: %s\n", argv[1]);
        else {
            printf("Unable to load cart\n");
           return 1;
        }
    }

    mEmu->run();

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
