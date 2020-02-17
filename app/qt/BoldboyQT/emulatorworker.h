#ifndef __EMULATORWORKER_H__
#define __EMULATORWORKER_H__

#include <QObject>
#include <QPixmap>
#include "boldboy.h"

class EmulatorWorker : public QObject {
    Q_OBJECT

    public:
        EmulatorWorker();

    public slots:
        void drawFrame(void);

    signals:
        void frameDone(QPixmap frame);

    private:
        Boldboy *mEmu;
        int mCount;
};

#endif /* __EMULATORWORKER_H__ */
