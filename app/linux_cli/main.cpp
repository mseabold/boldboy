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

#define MAX_ARGS 16
#define COMMAND_STR_SZ 2048

#define INVALID_COMMAND -1
#define INVALID_PARAMETER -2
#define NO_CPU -3
#define EXIT_CODE -4

class EmptyCart : public Cartridge {
    public:
        EmptyCart() : Cartridge(NULL, 0) {}
        ~EmptyCart() {}
        uint8_t readAddr(uint16_t addr) { return 0; }
        void writeAddr(uint16_t addr, uint8_t val) {}
};

class FileCart : public Cartridge {
    public:
        FileCart() : Cartridge(NULL, 0) {}
        ~FileCart() {}

        int load(char *filename) {
            FILE *f = fopen(filename, "rb");

            if(f == NULL)
                return -1;

            memset(buf, 0, 65535);

            size_t total = 0;

            total = fread(buf, 1, 65535, f);

            if(total != 65535)
                return ferror(f)?-1:0;

            return 0;
        }

        uint8_t readAddr(uint16_t addr) { return buf[addr]; }
        void writeAddr(uint16_t addr, uint8_t val) {
            if(addr >= 0x2000 && addr <= 0x3fff)
                printf("!!! ATTEMPT TO CHANGE MEMORY MODEL !!!!!!\n");
            else if(addr >= 0x4000 && addr <= 0x7fff)
                printf("!!! ATTEMPT TO CHANGE BANKS !!!!!!\n");
            else
                printf("!!! UNKOWN WRITE TO CART ROM !!!!!\n");
        }

        bool hasRam() { return false; }

    private:
        uint8_t buf[65535];
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

static Cpu *cpu;
static Mmu *mmu;
static Cartridge *cart;
static IoSerial *serial;
static IoTimer *timer;
static IoController *io;
static InterruptController *ic;
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
    if(!cpu)
        return NO_CPU;

    printf("BC: 0x%04x\tDE: 0x%04x\n", cpu->getReg16(Cpu::rBC)->read(), cpu->getReg16(Cpu::rDE)->read());
    printf("AF: 0x%04x\tHL: 0x%04x\n", cpu->getReg16(Cpu::rAF)->read(), cpu->getReg16(Cpu::rHL)->read());
    printf("SP: 0x%04x\tPC: 0x%04x\n", cpu->getReg16(Cpu::rSP)->read(), cpu->getReg16(Cpu::rPC)->read());

    return 0;
}

static int tick(ArgList_t *args) {
    if(!cpu)
        return NO_CPU;

    char disBuf[512];
    cpu->disassemble(disBuf, sizeof(disBuf));
    printf("%s\n", disBuf);
    uint8_t cycles = cpu->tick();
    io->tick(cycles);
    return 0;
}

static int writeSerial(ArgList_t *args) {
    if(!cpu)
        return NO_CPU;

    if(args->numArgs < 1)
        return INVALID_PARAMETER;

    IoSerial *serial = new SerialConsole();
    io->setSerialHandler(serial);

    printf("Attempting to write \"%s\" to serial\n", args->args[0].asStr);
    for(unsigned int i = 0; i<strlen(args->args[0].asStr);i++) {
        mmu->writeAddr(IOREG_SB, (uint8_t)(args->args[0].asStr[i]));
        mmu->writeAddr(IOREG_SC, IOREG_SC_START_XFER_VAL);
    }

    mmu->writeAddr(IOREG_SB, (uint8_t)'\n');
    mmu->writeAddr(IOREG_SC, IOREG_SC_START_XFER_VAL);

    io->setSerialHandler(NULL);
    delete serial;
    return 0;
}

static int loadcart_int(char *file) {
    FileCart *fCart = new FileCart();

    if(fCart->load(file) < 0) {
        delete fCart;
        printf("Unable to load cart\n");
        return INVALID_PARAMETER;
    }

    if(cpu) {
        delete cpu;
        delete mmu;
        delete cart;
        delete io;
        delete timer;
        delete ic;
    }

    cart = fCart;
    ic = new InterruptController();
    io = new IoController(ic);
    timer = new IoTimer(ic);
    io->setSerialHandler(serial);
    io->setTimer(timer);
    mmu = new Mmu(cart, io);
    cpu = new Cpu(mmu, ic);
    cpu->getReg16(Cpu::rPC)->write(0x0100);

    return 0;
}

static int loadCart(ArgList_t *args) {
    if(args->numArgs < 1)
        return INVALID_PARAMETER;

    return loadcart_int(args->args[0].asStr);
}

static bool debugOp = false;

static int run(ArgList_t *args) {
    if(!cpu)
        return NO_CPU;

    while(true) {
#if 1
        if(debugOp) {
        printf("\n");
        char disBuf[512];
        cpu->disassemble(disBuf, sizeof(disBuf));
        uint16_t pc = cpu->getReg16(Cpu::rPC)->read();
        printf("0x%02x 0x%02x 0x%02x\n", mmu->readAddr(pc), mmu->readAddr(pc+1), mmu->readAddr(pc+2));
        printf("%s\n", disBuf);
        getRegs(NULL);
        }
#if 0
        if(mmu->readAddr(cpu->getReg16(Cpu::rPC)->read()) == 0xE0 && mmu->readAddr(cpu->getReg16(Cpu::rPC)->read()+1) == 0x01) {
            printf("Break on serial write\n");
            return 0;
        }
#endif
#if 0
        if(mmu->readAddr(cpu->getReg16(Cpu::rPC)->read()) == 0x27) {
            printf("Break on DAA\n");
            return 0;
        }
#endif
#if 0
        if(cpu->getReg16(Cpu::rPC)->read() == 0xc694) {
            printf("Break on test_instr label\n");
            return 0;
        }
#endif
#if 1
        if(mmu->readAddr(cpu->getReg16(Cpu::rPC)->read()) == 0xfb) {
            printf("Break on EI\n");
            return 0;
        }
#endif
#endif
        uint8_t cycles = cpu->tick();
        io->tick(cycles);
    }
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

    cpu = NULL;
    mmu = NULL;
    cart = NULL;
    serial = new SerialConsole();

    if(argc >= 2) {
        if(loadcart_int(argv[1]) == 0)
            printf("Loaded cart: %s\n", argv[1]);
        else {
            printf("Unable to load cart\n");
           return 1;
        }
    }

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
