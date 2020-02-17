#include "boldboyqt.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BoldboyQT w;
    w.show();
    return a.exec();
}
