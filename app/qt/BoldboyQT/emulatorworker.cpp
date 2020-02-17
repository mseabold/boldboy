#include "emulatorworker.h"
#include "console_logger.h"

EmulatorWorker::EmulatorWorker() {
    mCount = 0;
    mEmu = new Boldboy(false);
    Logger *l = new ConsoleLogger();
    l->setLevel(LOG_DEBUG);
    mEmu->setLogger(l);
    mEmu->loadCart("/home/matt/Downloads/pokemon_red.gb");
}

void EmulatorWorker::drawFrame() {
    uint8_t frame[144][160];
    mEmu->waitForFrame();
    mEmu->getFrame(frame);
    QImage frameImg(160, 144, QImage::Format_Grayscale8);
    memcpy(frameImg.bits(), frame, 144*160);
    emit frameDone(QPixmap::fromImage(frameImg));
}
