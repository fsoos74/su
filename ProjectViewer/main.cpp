#include "projectviewer.h"
#include <QApplication>
#include <QMessageBox>

//uncomment to use demo data on linux
//#define DEMO 1

#ifdef USE_KEYLOCK_LICENSE
#include<segyreader.h>
#include<segywriter.h>
#include<cryptfunc.h>
#include<memory>
#include "keylokclass.h"
#include "keylok.h"
#endif

#ifdef DEMO
#include<segyreader.h>
#include<segywriter.h>
#include<cryptfunc.h>
#endif


class MyApplication : public QApplication {
public:
  MyApplication(int& argc, char ** argv) :
    QApplication(argc, argv) { }
  virtual ~MyApplication() { }

  // reimplemented from QApplication so we can throw exceptions in slots
  virtual bool notify(QObject * receiver, QEvent * event) {
    try {
      return QApplication::notify(receiver, event);
    } catch(std::exception& e) {
      QMessageBox::critical(nullptr, "RUNTIME EXCEPTION", e.what(), QMessageBox::Ok);
    }
    return false;
  }
};


int main(int argc, char *argv[]){

    try{

    MyApplication a(argc, argv);

    #ifdef USE_KEYLOCK_LICENSE
    {
        license::LicenseInfo info;
        std::unique_ptr<CKeylok>  myCKeylok(new CKeylok);
        if (myCKeylok->CheckForKeyLok() == false)
        {

            QMessageBox::warning(0, "AVO-Detect", "No Keylok dongle found!\nRunning in Demo mode...");
            seismic::SEGYReader::postReadFunc = crypt::decrypt;
            seismic::SEGYWriter::preWriteFunc = crypt::encrypt;
        }
    }
    #endif


    #ifdef DEMO
    QMessageBox::warning(0, "AVO-Detect", "No Keylok dongle found!\nRunning in Demo mode...");
    seismic::SEGYReader::postReadFunc = crypt::decrypt;
    seismic::SEGYWriter::preWriteFunc = crypt::encrypt;
    #endif

    ProjectViewer w;
    w.show();

    return a.exec();

    }
    catch( std::exception& ex){
        QMessageBox::critical(nullptr, "An exception has occured", ex.what(), QMessageBox::Ok);
    }

}
