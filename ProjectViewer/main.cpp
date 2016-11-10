#include "projectviewer.h"
#include <QApplication>
#include <QMessageBox>

#ifdef USE_KEYLOCK_LICENSE
#include<segyreader.h>
#include<crypt.h>
#include<memory>
#include "keylokclass.h"
#include "keylok.h"
#endif


//#define DEMO

#ifdef DEMO
#include<segyreader.h>//
#include<crypt.h>
#endif

int main(int argc, char *argv[]){

    QApplication a(argc, argv);

    #ifdef USE_KEYLOCK_LICENSE
    {
        license::LicenseInfo info;
        std::unique_ptr<CKeylok>  myCKeylok(new CKeylok);
        if (myCKeylok->CheckForKeyLok() == false)
        {

            QMessageBox::warning(0, "AVO-Detect", "No Keylok dongle found!\nRunning in Demo mode...");
            seismic::SEGYReader::postReadFunc = crypt::decrypt;

        }
    }
    #endif


    #ifdef DEMO
    QMessageBox::warning(0, "AVO-Detect", "No Keylok dongle found!\nRunning in Demo mode...");
    seismic::SEGYReader::postReadFunc = crypt::decrypt;
    #endif

    ProjectViewer w;
    w.show();

    return a.exec();

}
