#include "boldboy.h"

Boldboy::Boldboy() : Boldboy(false) {
}

Boldboy::Boldboy(bool useBootrom) {
    mIC = new InterruptController();
    mPpu = new Ppu(mIC);
    mDMA = new OAMDMA(mPpu);
    mJoypad = new IoJoypad(mIC);
    mIO = new IoController(mIC, mPpu, mDMA, mJoypad);
    mTimer = new IoTimer(mIC);
    mIO->setTimer(mTimer);
    mMmu = new Mmu(mIO, mPpu, mDMA);
    mMmu->enableBootrom(useBootrom);
    mCpu = new Cpu(mMmu, mIC);
    mCart = NULL;
    // If we aren't emulating the bootrom, move the PC past it
    mCpu->getReg16(Cpu::rPC)->write(useBootrom?0x0000:0x0100);
    if(!useBootrom)
        mCpu->getReg16(Cpu::rSP)->write(0xfffe);
    Logger::setLogger(NULL);
}

Boldboy::~Boldboy() {
    delete mCpu;
    delete mMmu;
    delete mIO;
    delete mTimer;
    delete mPpu;
    delete mJoypad;
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
    mPpu->tick(cycles);
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

void Boldboy::waitForFrame() {
    while(true) {
        Ppu::PpuMode curMode = mPpu->getMode();
        tick();
        Ppu::PpuMode newMode = mPpu->getMode();

        if(curMode != Ppu::modeVBlank && newMode == Ppu::modeVBlank)
            break;
    }
}

void Boldboy::getFrame(uint8_t frame[144][160]) {
    //DLOG(ZONE_CPU, "%s\n", "Get Frame");
    mPpu->getFrame(frame);
}

void Boldboy::buttonPressed(IoJoypad::Button button) {
    mJoypad->buttonPressed(button);
}

void Boldboy::buttonReleased(IoJoypad::Button button) {
    mJoypad->buttonReleased(button);
}
