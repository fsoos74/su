#include <segyinputdialog.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SegyInputDialog w;
    w.show();

    return a.exec();
}
