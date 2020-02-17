#include "boldboyqt.h"
#include "./ui_boldboyqt.h"
#include <qnamespace.h>

BoldboyQT::BoldboyQT(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Boldboy)
{
    ui->setupUi(this);
    mCnt = 0;
    mWorker = new EmulatorWorker();
    mWorker->moveToThread(&mWorkerThread);

    setCentralWidget(ui->frameContainer);

    mTimer = new QTimer(this);
    mTimer->setTimerType(Qt::TimerType::PreciseTimer);
    connect(mTimer, &QTimer::timeout, mWorker, &EmulatorWorker::drawFrame);
    connect(mWorker, &EmulatorWorker::frameDone, this, &BoldboyQT::frameDone);
    mWorkerThread.start();
    mTimer->start(17);
}

BoldboyQT::~BoldboyQT()
{
    mTimer->stop();
    mWorkerThread.quit();
    mWorkerThread.wait();
    delete mTimer;
    delete mWorker;
    delete ui;
}

void BoldboyQT::frameDone(QPixmap frame) {
    ui->frameContainer->setPixmap(frame.scaled(ui->frameContainer->size(), Qt::KeepAspectRatio));
}
