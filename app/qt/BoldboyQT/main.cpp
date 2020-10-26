#include "boldboyqt.h"

#include <QApplication>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BoldboyQT w;
    if(argc >- 2) {
        w.loadRom((const char *)argv[1]);
    }
    w.show();
    return a.exec();
}
