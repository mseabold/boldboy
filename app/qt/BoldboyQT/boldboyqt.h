#ifndef BOLDBOY_H
#define BOLDBOY_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QKeyEvent>
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
    void loadRom(const char *romfile);

public slots:
    void frameDone(QPixmap frame);

signals:
    void drawFrame(void);

private:
    Ui::Boldboy *ui;
    uint32_t mCnt;
    QThread mWorkerThread;
    EmulatorWorker *mWorker;
    QTimer *mTimer;
    bool mRunning;

protected:
    void keyPressEvent(QKeyEvent *event);

};
#endif // BOLDBOY_H
