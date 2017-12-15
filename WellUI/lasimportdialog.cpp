#include "lasimportdialog.h"
#include "ui_lasimportdialog.h"

#include<QFileDialog>
#include<QMessageBox>
#include<QProgressDialog>
#include<QStandardItemModel>
#include<memory>
#include<iostream>
#include<fstream>
#include <las_data.h>
#include <las_reader.h>
#include <log.h>

bool getStringMnemonic( const std::unordered_map<std::string,well::LASMnemonic>& s, const std::string& mnem, QString& data){

    std::unordered_map<std::string, well::LASMnemonic>::const_iterator it;
    it=s.find(mnem);
    if( it==s.end()){
        return false;
    }
    bool ok=false;
    data=QString( it->second.data.c_str());

    return true;
}

bool getDoubleMnemonic( const std::unordered_map<std::string, well::LASMnemonic>& s, const std::string& mnem, double& data){

    std::unordered_map<std::string, well::LASMnemonic>::const_iterator it;
    it=s.find(mnem);
    if( it==s.end()){
        return false;
    }
    bool ok=false;
    data=QString( it->second.data.c_str()).toDouble(&ok);

    return ok;
}



LASImportDialog::LASImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LASImportDialog)
{
    ui->setupUi(this);

    ui->pbBrowse->click();      // browse for input file
}

LASImportDialog::~LASImportDialog()
{
    delete ui;
}

QVector<int> LASImportDialog::selectedCurves(){

    auto ids = ui->lwCurves->selectionModel()->selectedIndexes();
    QVector<int> res;
    for( auto mi : ids ){
        res.push_back(mi.row());
    }
    return res;
}


void LASImportDialog::on_pbBrowse_clicked()
{
    static QString lastDir = QDir::homePath();
    QString fn = QFileDialog::getOpenFileName( this, tr("Open LAS file"), lastDir );
    if( fn.isEmpty() ) return;
    readLAS(fn);
    lastDir = QFileInfo(fn).absoluteDir().absolutePath();
}


void LASImportDialog::readLAS(QString fn){

    std::ifstream fin(fn.toStdString());
    if( !fin.good()){
        QMessageBox::critical(this, "Open LAS file", QString("Could not open file \"%1\% for reading!").arg(fn));
        return;
    }

    well::LASData data;
    std::unique_ptr<well::LASReader> ureader( new well::LASReader( data, this ) ); // c++11 has no make_unique
    auto reader=ureader.get();

    QProgressDialog* progress=new QProgressDialog(this);        // add destroy on close??
    progress->setWindowTitle(QString("Reading ")+fn);
    //connect( reader, SIGNAL( currentTask(QString)), progress, SLOT( setLabelText(QString)) );
    connect( reader, SIGNAL( started(int)), progress, SLOT(setMaximum(int)) );
    connect( reader, SIGNAL( progress(int)), progress, SLOT(setValue(int)) );
    connect( reader, SIGNAL( finished()), progress, SLOT(close()));
    connect( progress, SIGNAL(canceled()), reader, SLOT( cancel()));
    progress->show();

    if( !reader->read(fin)){

        QMessageBox::critical(this, "Open LAS file", QString("Error reading file:\n%1").arg(reader->lastError().c_str()));
        return;
    }

    ui->leFileName->setText(fn);
    m_data=data;
    updateWellInformation();
    updateCurvesList();
    ui->lwCurves->selectAll();  // XXX
}

void LASImportDialog::updateWellInformation(){

    QStandardItemModel* model=new QStandardItemModel( m_data.wellInformation.size(), 4, this);

    QStringList labels;
    labels<<"Mnemonic"<<"Unit"<<"Value"<<"Description";
    model->setHorizontalHeaderLabels(labels);

    int row=0;
    for( auto m : m_data.wellInformation ){

        int column=0;

        QStandardItem* mitem=new QStandardItem(QString(m.second.name.c_str()).trimmed());
        //mitem->setData(Qt::AlignRight, Qt::TextAlignmentRole);
        model->setItem(row, column++, mitem );

        QStandardItem* uitem=new QStandardItem(QString(m.second.unit.c_str()).trimmed());
        //uitem->setData(Qt::AlignRight, Qt::TextAlignmentRole);
        model->setItem(row, column++, uitem );

        QStandardItem* ditem=new QStandardItem(QString(m.second.data.c_str()).trimmed());
        //ditem->setData(Qt::AlignRight, Qt::TextAlignmentRole);
        model->setItem(row, column++, ditem );

        QStandardItem* citem=new QStandardItem(QString(m.second.description.c_str()).trimmed());
        //citem->setData(Qt::AlignLeft, Qt::TextAlignmentRole);
        model->setItem(row, column++, citem );

        row++;
    }

    ui->tvWellInformation->setModel(model);
}

void LASImportDialog::updateCurvesList(){

    QStringList model;

    for( auto m : m_data.curveInformation ){

        model<<QString("%1 - %2").arg(m.name.c_str()).arg(m.description.c_str());
    }

    ui->lwCurves->clear();
    ui->lwCurves->addItems(model);
}



void LASImportDialog::import(AVOProject * project){

    if( !project) return;

    try{

    LASImportDialog dlg;
    dlg.setWindowTitle(tr("Import LAS file"));
    if( dlg.exec()!=QDialog::Accepted) return;

    QString lease;
    QString well;
    QString suffix;
    QString uwi;

    getStringMnemonic(dlg.data().wellInformation, "BHLSE", lease);
    getStringMnemonic(dlg.data().wellInformation, "WELL", well);
    getStringMnemonic(dlg.data().wellInformation, "WELSF", suffix);
    getStringMnemonic(dlg.data().wellInformation, "UWI", uwi);

    if( uwi.isEmpty()){
        QMessageBox::warning(nullptr,tr("Import Logs"), tr("No UWI found in LAS-file, trying API"), QMessageBox::Yes );
        getStringMnemonic(dlg.data().wellInformation, "API", uwi);
        if( uwi.isEmpty()){
            QMessageBox::critical(nullptr, tr("Import Logs"), tr("Neither UWI nor API found in LAS-file, Aborting..."), QMessageBox::Ok);
            return;
        }
    }

    uwi=uwi.trimmed();

    WellInfo info;
    info.setName(lease + "_" + well + "_" + suffix );
    info.setUWI(uwi);

    double startz;
    double stopz;
    double stepz;
    if( !getDoubleMnemonic(dlg.data().wellInformation, "STRT", startz )
            || !getDoubleMnemonic( dlg.data().wellInformation, "STOP", stopz )
            || !getDoubleMnemonic( dlg.data().wellInformation, "STEP", stepz )){
        QMessageBox::critical(nullptr, tr("Import Logs"), tr("Could not read STRT, STOP, and STEP mnemonic(s)"));
        return;
    }

    if( (stopz-startz)*stepz <=0 ){
        QMessageBox::critical( nullptr,tr("Import Logs"), tr("Invalid range (STRT,STOP,STEP)!") );
        return;
    }

    int nz=(stopz - startz ) / stepz + 1;


    QString wid(uwi);
    if( !project->existsWell(wid)){
        project->addWell( wid, info );
        QMessageBox::information( nullptr, tr("Import Logs"), QString("Added well name=\"%1\", uwi=\"%2\"").arg(info.name(), uwi));
    }

    auto curve_indices = dlg.selectedCurves();
    for( auto index : curve_indices ){

        Log log( dlg.data().curveInfo(index).name.c_str(), dlg.data().curveInfo(index).unit.c_str(), dlg.data().curveInfo(index).description,
                 startz, stepz, dlg.data().curveData(index) );
        QString name(dlg.data().curveInfo(index).name.c_str());
        if( !project->addLog( wid, name, log ) ){
            QMessageBox::critical(nullptr, tr("Add Logs"), QString("Adding log \"%1\" failed!").arg(name));
        }
    }

    }
    catch( std::exception& ex){
        QMessageBox::critical(nullptr, tr("Import Logs"), QString("Exception occured:\n%1").arg(ex.what()), QMessageBox::Ok);
    }

}


void LASImportDialog::bulkImport(AVOProject *project){

    if( !project) return;

   static QString dir=QDir::homePath();// QStandardPaths::locate(QStandardPaths::DocumentsLocation,"");
    auto paths=QFileDialog::getOpenFileNames(nullptr, tr("Import Logs (Bulk)"), dir);
    if(paths.empty()) return;
    //dir=QDir(paths.front()). // need to update dir here for next use
    bool ignoreErrors=false;

    QProgressDialog dlg;
    dlg.setMaximum(paths.size());
    dlg.setWindowTitle(QString("Importing %1 wells").arg(paths.size()));
    dlg.show();
    int i=0;

    for( auto path : paths ){

        try{

            dlg.setLabelText(path);

            std::ifstream fin(path.toStdString());
            if( !fin.good()){
                if( ignoreErrors) continue;
                auto ret=QMessageBox::critical(nullptr, "Open LAS file", QString("Could not open file \"%1\% for reading! Proceed with next well?").arg(path),
                                                                     QMessageBox::Abort | QMessageBox::Yes | QMessageBox::YesToAll);
                if( ret==QMessageBox::Abort) break;
                else if(ret==QMessageBox::YesToAll) ignoreErrors=true;
                continue;
            }

            well::LASData data;
            well::LASReader reader( data );

            if( !reader.read(fin)){
                if( ignoreErrors) continue;
                auto ret=QMessageBox::critical(nullptr, "Open LAS file", QString("Error reading file:\n%1\nProceed with next well?").arg(reader.lastError().c_str()),
                                      QMessageBox::Abort | QMessageBox::Yes | QMessageBox::YesToAll);
                if( ret==QMessageBox::Abort) break;
                else if(ret==QMessageBox::YesToAll) ignoreErrors=true;
                continue;
            }


            QString lease;
            QString well;
            QString suffix;
            QString uwi;

            getStringMnemonic(data.wellInformation, "BHLSE", lease);
            getStringMnemonic(data.wellInformation, "WELL", well);
            getStringMnemonic(data.wellInformation, "WELSF", suffix);
            getStringMnemonic(data.wellInformation, "UWI", uwi);

            if( uwi.isEmpty()){
                //QMessageBox::warning(nullptr,tr("Import Logs"), tr("No UWI found in LAS-file, trying API"), QMessageBox::Yes );
                getStringMnemonic(data.wellInformation, "API", uwi);
                if( uwi.isEmpty()){
                    if( ignoreErrors) continue;
                    auto ret=QMessageBox::critical(nullptr,tr("Import Logs"), tr("Neither UWI or APU found!\nProceed with next well?"),
                                          QMessageBox::Abort | QMessageBox::Yes | QMessageBox::YesToAll);
                    if( ignoreErrors) continue;
                    if( ret==QMessageBox::Abort) break;
                    else if(ret==QMessageBox::YesToAll) ignoreErrors=true;
                    continue;
                }
            }

            uwi=uwi.trimmed();

            WellInfo info;
            info.setName(lease + "_" + well + "_" + suffix );
            info.setUWI(uwi);

            double startz;
            double stopz;
            double stepz;
            if( !getDoubleMnemonic(data.wellInformation, "STRT", startz )
                    || !getDoubleMnemonic( data.wellInformation, "STOP", stopz )
                    || !getDoubleMnemonic( data.wellInformation, "STEP", stepz )){
                if( ignoreErrors) continue;
                int ret=QMessageBox::critical(nullptr, tr("Import Logs"), tr("Could not read STRT, STOP, and STEP mnemonic(s)!\nProceed with next well?"),
                                              QMessageBox::Abort | QMessageBox::Yes | QMessageBox::YesToAll);
                if( ret==QMessageBox::Abort) break;
                else if(ret==QMessageBox::YesToAll) ignoreErrors=true;
                continue;
            }

            if( (stopz-startz)*stepz <=0 ){
                auto ret=QMessageBox::critical(nullptr,tr("Import Logs"), tr("Invalid range (STRT,STOP,STEP)!\nProceed with next well?"),
                                      QMessageBox::Abort | QMessageBox::Yes | QMessageBox::YesToAll);
                if( ignoreErrors) continue;
                if( ret==QMessageBox::Abort) break;
                else if(ret==QMessageBox::YesToAll) ignoreErrors=true;
                continue;
            }

            int nz=(stopz - startz ) / stepz + 1;

            QString wid(uwi);

            if( !project->existsWell(wid)){
                project->addWell( wid, info );
                //QMessageBox::information(this, tr("Import Logs"), QString("Added well name=%1, uwi=%2").arg(info.name(), uwi));
            }

            for( int cidx=0; cidx<data.curveCount(); cidx++ ){

                auto cinfo=data.curveInfo(cidx);
                Log log( cinfo.name.c_str(), cinfo.unit.c_str(), cinfo.description, startz, stepz, data.curveData(cidx) );
                QString name(cinfo.name.c_str());
                if( !project->addLog( wid, name, log ) ){
                    if( ignoreErrors ) continue;
                    int ret=QMessageBox::critical(nullptr, tr("Add Logs"), QString("Adding log \"%1\" failed!\nProceed with next log?").arg(name),
                                                  QMessageBox::Abort | QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No );
                    if( ret==QMessageBox::Abort) return;                    // exit whole import
                    else if(ret==QMessageBox::YesToAll) ignoreErrors=true;
                    else if(ret==QMessageBox::No) break;                    // only exit this well
                    continue;
                }
            }

        }catch( std::exception& ex){
            if( ignoreErrors) continue;
            auto ret=QMessageBox::critical(nullptr, tr("Import Logs (BULK)"), QString("Exception occured:\n%1\nProceed with next well?").arg(ex.what()),
                                  QMessageBox::Abort | QMessageBox::Yes | QMessageBox::YesToAll );

            if( ret==QMessageBox::Abort) break;
            else if(ret==QMessageBox::YesToAll) ignoreErrors=true;
            continue;
        }


        dlg.setValue(++i);
        qApp->processEvents();
        if( dlg.wasCanceled() ) break;
    }
}
