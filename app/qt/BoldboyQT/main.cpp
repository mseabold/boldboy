#include "boldboyqt.h"

#include <QApplication>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Usage: %s romfile\n", argv[0]);
        return 1;
    }
    QApplication a(argc, argv);
    BoldboyQT w;
    w.loadRom((const char *)argv[1]);
    w.show();
    return a.exec();
}
