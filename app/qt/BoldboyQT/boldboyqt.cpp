#include "boldboyqt.h"
#include "./ui_boldboyqt.h"
#include <qnamespace.h>

BoldboyQT::BoldboyQT(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Boldboy)
{
    ui->setupUi(this);
    mCnt = 0;

    setCentralWidget(ui->frameContainer);

    mTimer = NULL;
    mWorker = NULL;
}

BoldboyQT::~BoldboyQT()
{
    if(mTimer != NULL) {
        mTimer->stop();
        mWorkerThread.quit();
        mWorkerThread.wait();
        delete mTimer;
        delete mWorker;
        delete ui;
    }
}

void BoldboyQT::frameDone(QPixmap frame) {
    ui->frameContainer->setPixmap(frame.scaled(ui->frameContainer->size(), Qt::KeepAspectRatio));
}

void BoldboyQT::loadRom(const char *romfile) {
    mWorker = new EmulatorWorker(romfile);
    mWorker->moveToThread(&mWorkerThread);
    mTimer = new QTimer(this);
    mTimer->setTimerType(Qt::TimerType::PreciseTimer);
    connect(mTimer, &QTimer::timeout, mWorker, &EmulatorWorker::drawFrame);
    connect(mWorker, &EmulatorWorker::frameDone, this, &BoldboyQT::frameDone);
    mWorkerThread.start();
    mTimer->start(17);
}
