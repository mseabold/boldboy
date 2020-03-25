#include "emulatorworker.h"
#include "console_logger.h"

EmulatorWorker::EmulatorWorker(const char *romfile) {
    mCount = 0;
    mEmu = new Boldboy(false);
    mLogger = new ConsoleLogger();
    mLogger->setLevel(LOG_NONE);
    mEmu->setLogger(mLogger);
    mEmu->loadCart(romfile);
}

void EmulatorWorker::drawFrame() {
    uint8_t frame[144][160];
    mEmu->waitForFrame();
    mEmu->getFrame(frame);
    QImage frameImg(160, 144, QImage::Format_Grayscale8);
    memcpy(frameImg.bits(), frame, 144*160);
    emit frameDone(QPixmap::fromImage(frameImg));
}

void EmulatorWorker::keyPressed(int key, bool pressed) {
    bool handle = true;
    IoJoypad::Button padButton;
    switch(key) {
        case Qt::Key_A:
            padButton = IoJoypad::A;
            break;
        case Qt::Key_B:
            padButton = IoJoypad::B;
            break;
        case Qt::Key_X:
            padButton = IoJoypad::START;
            break;
        case Qt::Key_Shift:
            padButton = IoJoypad::SELECT;
            break;
        case Qt::Key_Up:
            padButton = IoJoypad::UP;
            break;
        case Qt::Key_Down:
            padButton = IoJoypad::DOWN;
            break;
        case Qt::Key_Left:
            padButton = IoJoypad::LEFT;
            break;
        case Qt::Key_Right:
            padButton = IoJoypad::RIGHT;
            break;
        default:
            handle = false;
            break;
    }

    if(handle) {
        if(pressed)
            mEmu->buttonPressed(padButton);
        else
            mEmu->buttonReleased(padButton);
    }
}
