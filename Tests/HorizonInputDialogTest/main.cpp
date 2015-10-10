#include "horizoninputdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HorizonInputDialog w;
    w.show();

    return a.exec();
}
