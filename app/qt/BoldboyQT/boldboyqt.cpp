#include "boldboyqt.h"
#include "./ui_boldboyqt.h"
#include <qnamespace.h>
#include <qfiledialog.h>

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
    connect(this, &BoldboyQT::drawFrame, mWorker, &EmulatorWorker::drawFrame);
    connect(this, &BoldboyQT::keyPressed, mWorker, &EmulatorWorker::keyPressed);
    mWorkerThread.start();
    mTimer->start(17);
    mRunning = true;
}

void BoldboyQT::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Z) {
        if(mRunning) {
            mTimer->stop();
            mRunning = false;
        } else {
            mTimer->start(17);
            mRunning = true;
        }
    } else if(event->key() == Qt::Key_F && !mRunning) {
        emit drawFrame();
    } else if(event->key() == Qt::Key_L && !mRunning) {
        mWorker->mLogger->setLevel(LOG_DEBUG);
    } else if(event->key() == Qt::Key_T && mRunning) {
        mTimer->setInterval(6);
    } else {
        emit keyPressed(event->key(), true);
    }
}

void BoldboyQT::keyReleaseEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_T)
        mTimer->setInterval(17);
    else
        emit keyPressed(event->key(), false);
}

void BoldboyQT::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    QByteArray fileNameBytes = fileName.toLocal8Bit();
    loadRom(fileNameBytes.data());
}
