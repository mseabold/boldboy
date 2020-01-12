#include "boldboy.h"

Boldboy::Boldboy() {
    mIC = new InterruptController();
    mPpu = new Ppu(mIC);
    mIO = new IoController(mIC, mPpu);
    mTimer = new IoTimer(mIC);
    mIO->setTimer(mTimer);
    mMmu = new Mmu(mIO, mPpu);
    mCpu = new Cpu(mMmu, mIC);
    mCart = NULL;
    mCpu->getReg16(Cpu::rPC)->write(0x0100);
    Logger::setLogger(NULL);
}

Boldboy::~Boldboy() {
    delete mCpu;
    delete mMmu;
    delete mIO;
    delete mTimer;
    delete mPpu;
    delete mIC;

    if(mCart)
        delete mCart;
}

bool Boldboy::loadCart(const char *filename) {
    if(mCart != NULL)
        delete mCart;

    mCart = Cartridge::loadFromFile(filename);

    if(mCart != NULL) {
        mMmu->loadCart(mCart);
        return true;
    }

    return false;
}

void Boldboy::tick() {
    uint8_t cycles = mCpu->tick();
    mIO->tick(cycles);
}

void Boldboy::run() {
    while(true)
        tick();
}

void Boldboy::setSerialHandler(IoSerial *serial) {
    mIO->setSerialHandler(serial);
}

void Boldboy::setLogger(Logger *logger) {
    Logger::setLogger(logger);
}
