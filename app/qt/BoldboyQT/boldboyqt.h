#ifndef BOLDBOY_H
#define BOLDBOY_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include "emulatorworker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Boldboy; }
QT_END_NAMESPACE

class BoldboyQT : public QMainWindow
{
    Q_OBJECT

public:
    BoldboyQT(QWidget *parent = nullptr);
    ~BoldboyQT();

public slots:
    void frameDone(QPixmap frame);

private:
    Ui::Boldboy *ui;
    uint32_t mCnt;
    QThread mWorkerThread;
    EmulatorWorker *mWorker;
    QTimer *mTimer;

};
#endif // BOLDBOY_H
