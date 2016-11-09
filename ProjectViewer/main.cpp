#include "projectviewer.h"
#include <QApplication>
#include <QMessageBox>

#include<segyreader.h>
#include<crypt.h>

int main(int argc, char *argv[]){

    QApplication a(argc, argv);

    // activate demo mode
    //seismic::SEGYReader::postReadFunc = crypt::decrypt;

    ProjectViewer w;
    w.show();

    return a.exec();

}
