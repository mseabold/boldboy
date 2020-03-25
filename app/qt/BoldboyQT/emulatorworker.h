#ifndef __EMULATORWORKER_H__
#define __EMULATORWORKER_H__

#include <QObject>
#include <QPixmap>
#include "boldboy.h"

class EmulatorWorker : public QObject {
    Q_OBJECT

    public:
        EmulatorWorker(const char *romfile);
        Boldboy *mEmu;
        Logger *mLogger;

    public slots:
        void drawFrame(void);

    signals:
        void frameDone(QPixmap frame);

    private:
        int mCount;
};

#endif /* __EMULATORWORKER_H__ */
