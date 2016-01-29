#include "projectviewer.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[]){

    QApplication a(argc, argv);

    ProjectViewer w;
    w.show();

    return a.exec();
}
