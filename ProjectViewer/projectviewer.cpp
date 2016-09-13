#include "projectviewer.h"
#include "ui_projectviewer.h"

#include "seismicdataselector.h"
#include "datasetpropertiesdialog.h"
#include "twocombosdialog.h"
#include "aboutdialog.h"

#include<QFileDialog>
#include<QDir>
#include<QMessageBox>
#include<QInputDialog>
#include<QStandardItem>
#include<QStandardItemModel>
#include<QProgressDialog>
#include<QSettings>
#include<QPointF>

#include<gridimportdialog.h>
#include<gridexportdialog.h>
#include<xgrwriter.h>
#include<grid.h>
#include<xprwriter.h>
#include<xprreader.h>
#include<gridviewer.h>
#include<databaseviewer.h>
#include<segyinputdialog.h>
#include<segyreader.h>
#include<segyinfo.h>
#include<trace.h>
#include<header.h>
#include<gatherviewer.h>
#include<gatherview.h>
#include<gatherruler.h>
#include<axxislabelwidget.h>
#include<seismicdatasetreader.h>
#include<histogram.h>
#include<histogramdialog.h>

#include<iostream>
#include<chrono>
#include<random>
#include<limits>
#include<memory>

#include<projectgeometrydialog.h>
#include<exportseismicprocess.h>
#include<exportseismicdialog.h>
#include <createtimesliceprocess.h>
#include <createtimeslicedialog.h>
#include <horizonamplitudesprocess.h>
#include <horizonamplitudesdialog.h>
#include <horizonsemblanceprocess.h>
#include <horizonsemblancedialog.h>
#include <computeinterceptgradientdialog.h>
#include <computeinterceptgradientprocess.h>
#include <interceptgradientvolumedialog.h>
#include <interceptgradientvolumeprocess.h>
#include <fluidfactorprocess.h>
#include <fluidfactordialog.h>
#include <fluidfactorvolumeprocess.h>
#include <fluidfactorvolumedialog.h>
#include <gridrunuserscriptdialog.h>
#include <rungridscriptprocess.h>
#include <runvolumescriptdialog.h>
#include <runvolumescriptprocess.h>
#include <windowreductionfunction.h>

#include<axxisorientation.h>
#include<projectgeometry.h>

#include "selectgridtypeandnamedialog.h"
#include "crossplotgridsinputdialog.h"
#include "crossplotvolumesinputdialog.h"
#include "crossplotviewer.h"
#include "amplitudecurveviewer.h"
#include <crossplot.h>

#include<segywriter.h>
#include<swsegywriter.h>
#include<segyreader.h>
#include<seisware_header_def.h>
#include<trace.h>
#include<header.h>
#include<headervalue.h>
#include<segy_text_header.h>
#include<fstream>
#include<set>
#ifdef USE_KEYLOCK_LICENSE
#include<QApplication>
#include<QTimer>
#include "keylokclass.h"
#include "keylok.h"
#endif

#ifdef __linux
#include <arpa/inet.h> // htonl
#elif WIN32
#include <Winsock2.h> // htonl
#endif

ProjectViewer::ProjectViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProjectViewer),
    m_dispatcher( new PointDispatcher )   // dont want dispatcher to deleted together with childs because they might use it
{
    ui->setupUi(this);

    ui->datasetsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->datasetsView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runDatasetContextMenu(const QPoint&)));

    ui->horizonsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->horizonsView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runHorizonContextMenu(const QPoint&)));

    ui->attributesView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->attributesView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runAttributeContextMenu(const QPoint&)));

    ui->gridsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->gridsView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runOtherGridContextMenu(const QPoint&)));

    ui->volumesView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->volumesView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runVolumeContextMenu(const QPoint&)));

    loadSettings();

    updateMenu();

#ifdef USE_KEYLOCK_LICENSE
    const int LICENSE_CHECK_MILLIS=30000;
    QTimer* licenseTimer=new QTimer(this);
    connect( licenseTimer, SIGNAL(timeout()), this, SLOT(on_licenseTimer()) );
    licenseTimer->start(LICENSE_CHECK_MILLIS);

    QAction* checkLicenseAct=new QAction(tr("Check &License"), this);
    connect(checkLicenseAct,SIGNAL(triggered()), this, SLOT(on_checkLicense()));
    ui->menu_Help->addAction(checkLicenseAct);
#endif
}


ProjectViewer::~ProjectViewer()
{
    delete ui;
}


#ifdef USE_KEYLOCK_LICENSE

void ProjectViewer::on_licenseTimer(){
    checkLicense();
}

void ProjectViewer::on_checkLicense(){

    license::LicenseInfo info=checkLicense();

    QMessageBox::information(this, "License Information",
        QString().sprintf(
                "<tt>"
                "Dongle Number......%05d<br>"
                "License Number.....%05d-%02d<br>"
                "First Leased.......%02d/%02d<br>"
                "Expiration Date....%02d/%02d/%d<br>"
                "</tt>",
                info.dongleSerialNumber,
                info.majorNumber,info.minorNumber,
                info.productVersion/100,info.productVersion%100,
                info.expirationMonth, info.expirationDay, info.expirationYear) );

}

license::LicenseInfo ProjectViewer::checkLicense(){

    license::LicenseInfo info;

    std::unique_ptr<CKeylok>  myCKeylok(new CKeylok);
    if (myCKeylok->CheckForKeyLok() == false)
    {
        QMessageBox::critical(0, "AVO-Detect", "No Keylok dongle found!\nTerminating...");
        qApp->closeAllWindows();
        throw std::runtime_error("NO VALID LICENSE!");
    }

    myCKeylok->ReadAuthorization();

    info.dongleSerialNumber=myCKeylok->GetSerialNumber();
    info.majorNumber=myCKeylok->ReadMemory(license::MajorNumberAddress);
    info.minorNumber=myCKeylok->ReadMemory(license::MinorNumberAddress);
    info.productVersion=myCKeylok->ReadMemory(license::ProductVersionAddress);
    if( info.productVersion<1 ){
        QMessageBox::critical(0, "AVO-Detect", "This product is not licensed!\nTerminating...");
        qApp->closeAllWindows();
        throw std::runtime_error("NO VALID LICENSE!");
    }
    myCKeylok->CheckExpiration();
    switch (myCKeylok->ReturnValue2)
    {
    case LEASEEXPIRED:
        QMessageBox::critical(0, "AVO-Detect", "Lease has expired!\nTerminating...");
        qApp->closeAllWindows();
        throw std::runtime_error("NO VALID LICENSE!");
        break;
    case SYSDATESETBACK:
        QMessageBox::critical(0, "AVO-Detect", "System clock set back!\nTerminating...");
        qApp->closeAllWindows();
        throw std::runtime_error("NO VALID LICENSE!");
        break;
    case NOLEASEDATE:
        QMessageBox::critical(0, "AVO-Detect", "No lease date set!\nTerminating...");
        qApp->closeAllWindows();
        throw std::runtime_error("NO VALID LICENSE!");
        break;
    case LEASEDATEBAD:
        QMessageBox::critical(0, "AVO-Detect", "Bad lease date. Requires reset!\nTerminating...");
        qApp->closeAllWindows();
        throw std::runtime_error("NO VALID LICENSE!");
        break;
    case LASTSYSDATECORRUPT:
        QMessageBox::critical(0, "AVO-Detect", "Problem with system clock!\nTerminating...");
        qApp->closeAllWindows();
        throw std::runtime_error("NO VALID LICENSE!");
        break;
    default:
        unsigned DateRead = myCKeylok->GetExpiration();
        info.expirationMonth=myCKeylok->GetExpMonth(DateRead);
        info.expirationDay=myCKeylok->GetExpDay(DateRead);
        info.expirationYear=myCKeylok->GetExpYear(DateRead);
        break;
    }

    return info;
}
#endif

void ProjectViewer::closeEvent(QCloseEvent *)
{
    saveSettings();

    qApp->closeAllWindows(); // close all open windows because closing this quits the program
}

void ProjectViewer::on_actionNewProject_triggered()
{

    QString dirName = QFileDialog::getExistingDirectory(this, tr("Select Project Directory"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if( dirName.isNull()) return;       // nothing selected

    try{

        std::shared_ptr<AVOProject> tmp(new AVOProject);
        tmp->setProjectDirectory(dirName);
        m_project=tmp;
    }
    catch( AVOProject::PathException& ex ){
        QMessageBox::critical( this, "Create new project", ex.what(), QMessageBox::Ok);
    }

    updateMenu();
    updateProjectViews();
}

void ProjectViewer::on_actionOpenProject_triggered()
{
    QString filter("*.xpr");
    QString fileName=QFileDialog::getOpenFileName( this, tr("Open Project"), QDir::homePath(), filter);
    if( fileName.isNull()) return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Open Project"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    std::shared_ptr<AVOProject> tmp(new AVOProject);
    XPRReader reader(*tmp);
    if (!reader.read(&file)) {
        QMessageBox::warning(this, tr("Open Project"),
                             tr("Parse error in file %1:\n\n%2")
                             .arg(fileName)
                             .arg(reader.errorString()));
        return;

    }

    // check if project already in use, create lockfile if not
    // no need to remove stale lockfiles. This is handled by QLockfile
    std::shared_ptr<QLockFile> lf(new QLockFile(tmp->lockfileName()));
    if( !lf->tryLock()){
        QMessageBox::critical(this, tr("Open Project"),
                              tr("Project already used by another program"));
        return;
    }

    m_lockfile=lf;  // will be unlocked when deleted
    m_project=tmp;

    setProjectFileName(fileName);
    updateProjectViews();
    updateMenu();
}


void ProjectViewer::on_actionOpenGrid_triggered()
{
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Open Grid"),
                                         tr("Select Grid:"), m_project->gridList(GridType::Other), 0, false, &ok);
    if (ok && !item.isEmpty()){
        displayGrid( GridType::Other, item);
    }
}


void ProjectViewer::on_actionOpenSeismicDataset_triggered()
{
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Open Seismic Dataset"),
                                         tr("Select Dataset:"), m_project->seismicDatasetList(), 0, false, &ok);
    if (ok && !item.isEmpty()){
        displaySeismicDataset(item);
    }
}

void ProjectViewer::on_action_Geometry_triggered()
{
    ProjectGeometryDialog dlg(this);
    dlg.setWindowTitle("Setup Project Geometry");
    dlg.setProject(m_project);

    if( dlg.exec()==QDialog::Accepted){

        ProjectGeometry geom=dlg.projectGeometry();

        m_project->setGeometry(geom);
    }

}

void ProjectViewer::importGrid(GridType gridType){

    QString gridTypeString=gridType2String(gridType);
    bool ok=false;
    QString gridName=QInputDialog::getText(this, QString("Import %1").arg(gridTypeString),
                                           "Name:", QLineEdit::Normal, "new", &ok);

    if( !ok || gridName.isNull()) return;

    // check if exists HERE!!!
    if( m_project->gridList(gridType).contains(gridName)){
        QMessageBox::warning( this, "Import Grid", QString("%1 \"%2\" already exists!").arg(gridTypeString, gridName));
        return;
    }

    GridImportDialog dlg;
    dlg.setWindowTitle(QString("Import %1 %2").arg(gridTypeString, gridName) );

    if( dlg.exec()==QDialog::Accepted){

        std::shared_ptr<Grid2D<float> > grid=dlg.horizon();

        if( !m_project->addGrid( gridType, gridName, grid)){
            QMessageBox::critical(this, "Import Grid", "Adding grid to project failed!");
            return;
        }

        updateProjectViews();


    }

}

void ProjectViewer::on_actionImportHorizon_triggered()
{
    importGrid(GridType::Horizon);
}

void ProjectViewer::on_actionImportAttributeGrid_triggered()
{
    importGrid(GridType::Attribute);
}

void ProjectViewer::on_actionImportOtherGrid_triggered()
{
    importGrid( GridType::Other);
}

void ProjectViewer::on_actionImportSeismic_triggered()
{

    DatasetPropertiesDialog propertiesDialog;
    propertiesDialog.setWindowTitle(QString("Properties of new dataset"));

    if( propertiesDialog.exec()!=QDialog::Accepted) return;
    QString name=propertiesDialog.name();
    if( name.isEmpty()) return;

    if( m_project->seismicDatasetList().contains(name)){
        QMessageBox::warning(this, "Import Dataset", QString("Dataset \"%1\" already exists!").arg(name));
        return;
    }


    SegyInputDialog segyInputDialog;
    segyInputDialog.setWindowTitle("Import SEG-Y");
    if( segyInputDialog.exec()!=QDialog::Accepted) return;


    SeismicDatasetInfo info;
    info.setDomain(propertiesDialog.domain());
    info.setMode(propertiesDialog.mode());

    if( !m_project->addSeismicDataset(name, segyInputDialog.path(), segyInputDialog.info())){
        QMessageBox::critical(this, "Import Dataset", "Adding seismic dataset to project failed!");
        return;
    }

    SeismicDatasetInfo datasetInfo=m_project->getSeismicDatasetInfo(name);
    datasetInfo.setDomain(propertiesDialog.domain());
    datasetInfo.setMode(propertiesDialog.mode());

    if( !m_project->setSeismicDatasetInfo(name, datasetInfo) ){
        QMessageBox::critical(this, "Import Dataset", QString("Setting properties of \"%1\" failed!").arg(name));
        return;
    }

    updateProjectViews();
}

void ProjectViewer::selectAndExportSeismicDataset(){

    ExportSeismicDialog dlg(this);
    dlg.setWindowTitle("Export Seismic Dataset");
    dlg.setDatasets(m_project->seismicDatasetList());

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess( new ExportSeismicProcess( m_project, this ), params );

}

void ProjectViewer::exportSeismicDataset( QString name ){

    ExportSeismicDialog dlg(this);
    dlg.setWindowTitle("Export Seismic Dataset");
    dlg.setFixedDataset(name);

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess( new ExportSeismicProcess( m_project, this ), params );
}

void ProjectViewer::selectAndExportGrid(GridType gridType)
{
    QString gridTypeString=gridType2String(gridType);

    bool ok;
    QString gridName = QInputDialog::getItem(this, QString("Export %1 Grid").arg(gridTypeString),
                                         tr("Select Grid:"), m_project->gridList(gridType), 0, false, &ok);
    if (ok && !gridName.isEmpty()){

        exportGrid(gridType, gridName);
    }
}


void ProjectViewer::exportGrid( GridType gridType, QString gridName){

    QString gridTypeString=gridType2String(gridType);

    std::shared_ptr<Grid2D<float>> grid=m_project->loadGrid(gridType, gridName);

    if( !grid ){
        QMessageBox::critical(this, "Export Grid", QString("Loading grid \"%1\" failed!").arg(gridName) );
        return;
    }

    GridExportDialog dlg(grid, this);
    dlg.setWindowTitle(QString("Export %1 Grid %2").arg(gridTypeString, gridName));
    dlg.exec();
}


void ProjectViewer::on_actionExportHorizon_triggered()
{
    selectAndExportGrid(GridType::Horizon);
}

void ProjectViewer::on_actionExportAttributeGrid_triggered()
{
    selectAndExportGrid(GridType::Attribute);
}

void ProjectViewer::on_actionExportOtherGrid_triggered()
{
    selectAndExportGrid(GridType::Other);
}


// XXX make this a process later
void ProjectViewer::on_actionImportVolume_triggered()
{
    Q_ASSERT( m_project);

    QString name;
    forever{
        name=QInputDialog::getText(this, "Import Volume", "Volume name:");
        if( name.isNull()) return;  // canceled
        if(m_project->volumeList().contains(name)){
            QMessageBox::warning(this,"Import Volume",
                                    QString("Volume %1 exists! Please select an other name.").arg(name));
        }else{
            break;     // got good name
        }
    }

    // open segy
    SegyInputDialog segyInputDialog;
    segyInputDialog.setWindowTitle("Import Volume SEG-Y");
    if( segyInputDialog.exec()!=QDialog::Accepted) return;
    seismic::SEGYInfo info=segyInputDialog.info();
    QString path=segyInputDialog.path();
    seismic::SEGYReader reader( path.toStdString(), info);

    QProgressDialog progress(this);
    progress.setWindowTitle("Import Volume");
    progress.show();

    // scan segy geometry
    progress.setLabelText("Scanning geometry...");
    progress.setRange(0,100);
    int onePercent=reader.trace_count()/100;

    double ft=0;
    double dt=0.000001*reader.binaryHeader().at("dt").uintValue();
    size_t nt=reader.binaryHeader().at("ns").uintValue();
    int minil=-1;//std::numeric_limits<int>::max();
    int maxil=-1;//std::numeric_limits<int>::min();
    int minxl=-1;//std::numeric_limits<int>::max();
    int maxxl=-1;//std::numeric_limits<int>::min();
    for( int i=0; i<reader.trace_count(); i++){
        reader.seek_trace(i);
        seismic::Header hdr=reader.read_trace_header();
        double tft=0.001*hdr["delrt"].intValue();
        int iline=hdr["iline"].intValue();
        int xline=hdr["xline"].intValue();
        if( i>0 ){
            // start time must be same for whole volume
            if( tft!=ft){
                QMessageBox::critical(this, "Import Volume",
                                      QString("Start time of trace %1 is different from first trace").arg(i+1));
                return;
            }

            if( iline<minil ) minil=iline;
            if( iline>maxil ) maxil=iline;
            if( xline<minxl ) minxl=xline;
            if( xline>maxxl ) maxxl=xline;

        }else{ // i==0
            ft=tft;
            minil=maxil=iline;
            minxl=maxxl=xline;
        }


        if( (i%onePercent)==0){
            progress.setValue(i/onePercent);
            qApp->processEvents();
            if(progress.wasCanceled()){
                return;
            }
        }
    }

    // create volume
    Grid3DBounds bounds( minil, maxil, minxl, maxxl, nt, ft, dt);
    std::shared_ptr<Grid3D<float>> volume(new Grid3D<float>(bounds, 0));

    // fill volume from file
    progress.show();
    progress.setLabelText("Reading traces...");
    reader.seek_trace(0);   // rewind
    while(reader.current_trace() < reader.trace_count() ){
        int i=reader.current_trace();
        seismic::Trace trace=reader.read_trace();
        int iline=trace.header()["iline"].intValue();
        int xline=trace.header()["xline"].intValue();
        for( size_t i=0; i<nt; i++){
            (*volume)(iline, xline, i)=trace.samples()[i];
        }
        if( (i%onePercent)==0){
            progress.setValue(i/onePercent);
            qApp->processEvents();
            if(progress.wasCanceled()){
                return;
            }
        }
    }


    // add volume to project
    m_project->addVolume(name, volume);
    updateProjectViews();
}


void ProjectViewer::on_actionExportVolume_triggered()
{
    selectAndExportVolume();
}

void ProjectViewer::on_actionExportSeismic_triggered()
{
    selectAndExportSeismicDataset();
}



void ProjectViewer::on_actionSave_triggered()
{
    if( m_projectFileName.isNull()){
        ui->actionSaveProjectAs->trigger();
        return;
    }

    saveProject( m_projectFileName );
}


void ProjectViewer::on_actionSaveProjectAs_triggered()
{
    QString filter=QString("*.xpr");
    // QString fileName=QFileDialog::getSaveFileName(this, "Save Project", QDir::homePath(), filter );
    // not using getSaveFileName because want to set default extension
    QFileDialog saveDialog( this, "Save Project As", QDir::homePath(), filter);
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveDialog.setDefaultSuffix("xpr");

    if( saveDialog.exec() != QDialog::Accepted ) return;

    QString fileName = saveDialog.selectedFiles().first();

    if( fileName.isNull()) return;

    if( !saveProject(fileName)){

        return;
    }

    setProjectFileName( fileName );
}

void ProjectViewer::on_actionCreateTimeslice_triggered()
{
    Q_ASSERT( m_project );

    CreateTimesliceDialog dlg;
    dlg.setDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Poststack));
    dlg.setHorizons( m_project->gridList( GridType::Horizon));
    dlg.setReservedGrids(m_project->gridList(GridType::Other));
    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess( new CreateTimesliceProcess( m_project, this ), params );
}

void ProjectViewer::on_actionHorizon_Amplitudes_triggered()
{
    Q_ASSERT( m_project );

    HorizonAmplitudesDialog dlg;
    dlg.setDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Poststack));
    dlg.setHorizons( m_project->gridList(GridType::Horizon));
    dlg.setReservedGrids(m_project->gridList(GridType::Other));
    QStringList methods;
    for( auto method : ReductionMethodsAndNames.keys() ){
        methods.append( ReductionMethodsAndNames.value(method));
    }
    dlg.setMethods(methods);
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new HorizonAmplitudesProcess( m_project, this ), params );
}

void ProjectViewer::on_actionHorizon_Semblance_triggered()
{
    Q_ASSERT( m_project );

    HorizonSemblanceDialog dlg;
    dlg.setDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Poststack));
    dlg.setHorizons( m_project->gridList(GridType::Horizon));
    dlg.setReservedGrids(m_project->gridList(GridType::Other));
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new HorizonSemblanceProcess( m_project, this ), params );

}

void ProjectViewer::on_actionCompute_Intercept_Gradient_triggered()
{

    Q_ASSERT( m_project );

    ComputeInterceptGradientDialog dlg;//=new ComputeInterceptGradientDialog(this);
    dlg.setDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Prestack));
    dlg.setHorizons( m_project->gridList(GridType::Horizon));
    dlg.setReservedGrids(m_project->gridList(GridType::Attribute));
    QStringList methods;
    for( auto method : ReductionMethodsAndNames.keys() ){
        methods.append( ReductionMethodsAndNames.value(method));
    }
    dlg.setReductionMethods(methods);
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new ComputeInterceptGradientProcess( m_project, this ), params );

}


void ProjectViewer::on_actionCompute_Intercept_and_Gradient_Volumes_triggered()
{
    InterceptGradientVolumeDialog dlg;
    dlg.setDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Prestack));
    dlg.setReservedVolumes(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new InterceptGradientVolumeProcess( m_project, this ), params );
}


void ProjectViewer::on_actionFluid_Factor_Grid_triggered()
{
    Q_ASSERT( m_project );

    FluidFactorDialog dlg;
    dlg.setInterceptGrids( m_project->gridList(GridType::Attribute));
    dlg.setGradientGrids( m_project->gridList(GridType::Attribute));
    dlg.setReservedGrids(m_project->gridList(GridType::Attribute));

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new FluidFactorProcess( m_project, this ), params );
}


void ProjectViewer::on_actionFluid_Factor_Volume_triggered()
{
    Q_ASSERT( m_project );

    FluidFactorVolumeDialog dlg;
    dlg.setInterceptVolumes( m_project->volumeList());
    dlg.setGradientVolumes( m_project->volumeList());
    dlg.setReservedVolumes(m_project->volumeList());

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new FluidFactorVolumeProcess( m_project, this ), params );
}


void ProjectViewer::on_actionRun_Grid_User_Script_triggered()
{
     Q_ASSERT( m_project );

     GridRunUserScriptDialog dlg;
     dlg.setWindowTitle(tr("Run User Script"));
     dlg.setInputGrids( m_project->gridList(GridType::Attribute));
     if( dlg.exec()!=QDialog::Accepted) return;
     QMap<QString,QString> params=dlg.params();

     runProcess( new RunGridScriptProcess( m_project, this ), params );
}


void ProjectViewer::on_actionRun_Volume_Script_triggered()
{
    Q_ASSERT( m_project );

    RunVolumeScriptDialog dlg;
    dlg.setWindowTitle(tr("Run Volume Script"));
    dlg.setInputVolumes( m_project->volumeList());
    dlg.setInputGrids( m_project->gridList(GridType::Attribute));
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new RunVolumeScriptProcess( m_project, this ), params );
}

void ProjectViewer::on_actionCrossplot_Grids_triggered()
{

    Q_ASSERT( m_project );

    if( m_project->gridList(GridType::Attribute).empty() ){
        QMessageBox::warning(this, "Crossplot", "Project contains no grids of type Attribute!");
        return;
    }

    CrossplotGridsInputDialog dlg;
    dlg.setWindowTitle("Select Grids for Crossplot");
    dlg.setAttributeGrids(m_project->gridList(GridType::Attribute));
    dlg.setHorizons(m_project->gridList(GridType::Horizon));
    dlg.setOtherGrids(m_project->gridList(GridType::Other));

    if( dlg.exec()!=QDialog::Accepted) return;

    std::shared_ptr<Grid2D<float> > grid1=m_project->loadGrid( dlg.xType(), dlg.xName());
    if( !grid1 ) return;

    std::shared_ptr<Grid2D<float> > grid2=m_project->loadGrid( dlg.yType(), dlg.yName());
    if( !grid2 ) return;

    std::shared_ptr<Grid2D<float> > horizon;
    if( dlg.useHorizon()){
         horizon=
                 m_project->loadGrid( GridType::Horizon, dlg.horizonName());
     }

    std::shared_ptr<Grid2D<float> > grida;
    if( dlg.useAttribute()){
         grida=
                 m_project->loadGrid( dlg.attributeType(), dlg.attributeName());
     }


    crossplot::Data data=crossplot::createFromGrids(grid1.get(), grid2.get(), horizon.get(), grida.get());

    CrossplotViewer* viewer=new CrossplotViewer();
    viewer->setAttribute( Qt::WA_DeleteOnClose);

    viewer->setWindowTitle( QString("%1 vs %2").arg(dlg.xName(), dlg.yName() ) );
    viewer->show();
    viewer->setFixedColor(!grida);  // if attribute is used use variable color points
    viewer->setData(data); // add data after visible!!!! /// XXX QVECTOR!!!
    viewer->setAxisLabels(dlg.xName(), dlg.yName());

    viewer->setDispatcher(m_dispatcher);
}

void ProjectViewer::on_actionCrossplot_Volumes_triggered()
{
    Q_ASSERT( m_project );

    if( m_project->volumeList().empty() ){
        QMessageBox::warning(this, "Crossplot", "Project contains no volumes!");
        return;
    }

    CrossplotVolumesInputDialog dlg;
    dlg.setWindowTitle("Select Volumes for Crossplot");
    dlg.setVolumes(m_project->volumeList());

    /*
    TwoCombosDialog dlg;
    dlg.setWindowTitle("Select Volumes for Crossplot");
    dlg.setLabelText1("Volume #1 (x-axis):");
    dlg.setLabelText2("Volune #2 (y-axis):");
    dlg.setItems1(m_project->volumeList());
    dlg.setItems2(m_project->volumeList());
*/
    if( dlg.exec()!=QDialog::Accepted) return;

    std::shared_ptr<Grid3D<float> > volume1=m_project->loadVolume(dlg.xName());
    if( !volume1 ) return;

    std::shared_ptr<Grid3D<float> > volume2=m_project->loadVolume( dlg.yName());
    if( !volume2 ) return;

    if( volume1->bounds()!=volume2->bounds()){
        QMessageBox::critical(this, "Crossplot Volumes", "Currently only volumes with the same geometry are accepted!");
        return;
    }

    std::shared_ptr<Grid3D<float> > volumea;
    if( dlg.useAttribute()){
         volumea=
                 m_project->loadVolume( dlg.attributeName());
         if( volumea->bounds()!=volume1->bounds()){
             QMessageBox::critical(this, "Crossplot Volumes",
                "Currently only volumes with the same geometry are accepted!");
             return;
         }
     }


    Grid2DBounds bounds(volume1->bounds().inline1(), volume1->bounds().crossline1(),
                        volume1->bounds().inline2(), volume1->bounds().crossline2());
    int nt=volume1->bounds().sampleCount();
    float t1=volume1->bounds().ft();
    float t2=volume2->bounds().lt();

    crossplot::Data data;

    const int MAX_POINTS=100000;    // maximum number of points to plot
    int numInputPoints=bounds.width()*bounds.height()*nt;
    bool plotAllPoints=true;

    if( numInputPoints>MAX_POINTS){
        int ret=QMessageBox::question(this, "Crossplot Volumes",
            QString("Number of datapoints is %1. This will significantly slow down operation.\n"
                    "Statistically reduce the number of plotted points?").arg(numInputPoints),
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes );

        if( ret==QMessageBox::Cancel ) return;

        if( ret==QMessageBox::Yes ){
            plotAllPoints=false;
        }
    }

    if(plotAllPoints){
        data=crossplot::createFromVolumes(volume1.get(), volume2.get(),
                                          volumea.get() );
    }
    else{
        data=crossplot::createFromVolumesReduced(volume1.get(), volume2.get(),
                                                 MAX_POINTS, volumea.get());
    }

    CrossplotViewer* viewer=new CrossplotViewer();
    viewer->setAttribute( Qt::WA_DeleteOnClose);


    viewer->setWindowTitle( QString("%1 vs %2").arg(dlg.xName(), dlg.yName() ) );
    viewer->show();
    viewer->setData(data); // add data after visible!!!!
    viewer->setAxisLabels(dlg.xName(), dlg.yName());

    viewer->setDispatcher(m_dispatcher);

}


void ProjectViewer::on_actionAmplitude_vs_Offset_Plot_triggered()
{
    AmplitudeCurveViewer* viewer=new AmplitudeCurveViewer;
    viewer->setAttribute( Qt::WA_DeleteOnClose);
    viewer->setWindowTitle( QString("Amplitude vs Offset") );
    viewer->show();
    viewer->setProject(m_project);
    viewer->setDispatcher(m_dispatcher);
    //viewer->setData(data); // add data after visible!!!!
}


void ProjectViewer::on_action_About_triggered()
{
    AboutDialog dlg(0);
    dlg.setWindowTitle(QString("About %1").arg(QString(PROGRAM) ) );
    dlg.exec();
}


void ProjectViewer::runGridContextMenu( GridType gridType, QListView* view, const QPoint& pos){

    Q_ASSERT(view);

    QPoint globalPos = view->viewport()->mapToGlobal(pos);
    QModelIndex idx=view->currentIndex();
    QString gridName=view->model()->itemData(idx)[Qt::DisplayRole].toString();

    QMenu menu;
    menu.addAction("display");
    menu.addAction("histogram");
    menu.addSeparator();
    menu.addAction("export");
    menu.addSeparator();
    menu.addAction("rename");
    menu.addAction("remove");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="display" ){
        displayGrid( gridType, gridName);
    }
    else if( selectedAction->text()=="histogram" ){
        displayGridHistogram(gridType,gridName);
    }
    else if( selectedAction->text()=="export" ){
        exportGrid( gridType,gridName);
    }
    else if( selectedAction->text()=="rename" ){
        renameGrid( gridType,gridName);
     }
    else if( selectedAction->text()=="remove" ){
        removeGrid( gridType,gridName);
    }
}


void ProjectViewer::runHorizonContextMenu(const QPoint& pos){

    runGridContextMenu(GridType::Horizon, ui->horizonsView, pos);
}


void ProjectViewer::runAttributeContextMenu(const QPoint& pos){

    runGridContextMenu(GridType::Attribute, ui->attributesView, pos);
}


void ProjectViewer::runOtherGridContextMenu(const QPoint& pos){

    runGridContextMenu(GridType::Other, ui->gridsView, pos);
}

void ProjectViewer::runVolumeContextMenu( const QPoint& pos){

    QListView* view=ui->volumesView;

    QPoint globalPos = view->viewport()->mapToGlobal(pos);
    QModelIndex idx=view->currentIndex();
    QString volumeName=view->model()->itemData(idx)[Qt::DisplayRole].toString();

    QMenu menu;
    menu.addAction("export");
    menu.addAction("export seisware");
    menu.addSeparator();
    menu.addAction("rename");
    menu.addAction("remove");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="export" ){
        exportVolume( volumeName);
    }
    else if( selectedAction->text()=="export seisware" ){
        exportVolumeSeisware( volumeName);
    }
    else if( selectedAction->text()=="rename" ){
        renameVolume( volumeName);
     }
    else if( selectedAction->text()=="remove" ){
        removeVolume( volumeName);
    }
}

void ProjectViewer::runDatasetContextMenu(const QPoint& pos){

    QPoint globalPos = ui->datasetsView->viewport()->mapToGlobal(pos);
    QModelIndex idx=ui->datasetsView->currentIndex();
    QString datasetName=ui->datasetsView->model()->itemData(idx)[Qt::DisplayRole].toString();

    QMenu menu;
    menu.addAction("display");
    menu.addSeparator();
    menu.addAction("display index");
    menu.addAction("properties");
    menu.addSeparator();
    menu.addAction("export");
    menu.addSeparator();
    menu.addAction("remove");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="display" ){
        displaySeismicDataset(datasetName);
    }
    else if( selectedAction->text()=="display index" ){
        displaySeismicDatasetIndex(datasetName);
    }
    else if( selectedAction->text()=="properties" ){
        editSeismicDatasetProperties(datasetName);
    }
    else if( selectedAction->text()=="export" ){
        exportSeismicDataset(datasetName);
    }
    else if( selectedAction->text()=="remove" ){
        removeSeismicDataset(datasetName);
    }
}



void ProjectViewer::displayGrid( GridType t, const QString& name){

    if( m_project->gridList(t).contains(name)){

        std::shared_ptr<Grid2D<float> > grid=m_project->loadGrid( t, name);
        if( !grid ) return;


        GridViewer* viewer=new GridViewer; //don't make this parent because projectviewer should be able to be displayed over gridviewer

        viewer->setGrid(grid);
        QString typeName=gridType2String(t);
        viewer->setWindowTitle(QString("Grid %1 - %2").arg(typeName, name) );
        viewer->show();

        viewer->orientate(m_project->geometry());

        viewer->setDispatcher(m_dispatcher);

    }

}


void ProjectViewer::displayGridHistogram( GridType t, const QString& name){

    if( m_project->gridList(t).contains(name)){

        std::shared_ptr<Grid2D<float> > grid=m_project->loadGrid( t, name);
        if( !grid ) return;

        QVector<double> data;
        for( auto it=grid->values().cbegin(); it!=grid->values().cend(); ++it){
            if( *it==grid->NULL_VALUE) continue;
            data.push_back(*it);
         }
/*
        // XXX better have external process for histogram generation with process events
        double min=std::numeric_limits<double>::max();
        double max=std::numeric_limits<double>::lowest();
        for( auto it=grid->values().cbegin(); it!=grid->values().cend(); ++it){
            if( *it==grid->NULL_VALUE) continue;
            if(*it<min)min=*it;
            if(*it>max)max=*it;
        }

        // estimate optimum bin width
        double w=(max-min)/N_BINS;
        double q=std::pow(10, std::ceil(std::log10(w)));
        if(q/5>w) w=q/5;
        else if(q/2>w) w=q/2;
        else w=q;
        min=w*std::floor(min/w);

        // generate histogram
        Histogram hist(min, w, N_BINS);
        for( auto it=grid->values().cbegin(); it!=grid->values().cend(); ++it){
            if( *it==grid->NULL_VALUE) continue;
            hist.addValue(*it);
        }
*/
        HistogramDialog* viewer=new HistogramDialog; //don't make this parent because projectviewer should be able to be displayed over gridviewer
        viewer->setData( data );
        QString typeName=gridType2String(t);
        viewer->setWindowTitle(QString("Histogram of Grid %1 - %2").arg(typeName, name) );  
        //viewer->setHistogram(hist);
        viewer->show();

    }
}


void ProjectViewer::renameGrid( GridType t, const QString& name){

    if( m_project->gridList(t).contains(name)){
        bool ok=false;

        QString typeName=gridType2String(t);
        QString newName=QInputDialog::getText(this, QString("Rename %1 \"%2\"").arg(typeName, name), "New Name:", QLineEdit::Normal, name, &ok);

        if( newName.isNull() || !ok) return;

        if( newName==name) return;

        if( m_project->gridList(t).contains(newName) ){
            QMessageBox::information(this, "Rename Grid", QString("Grid %1 already exists!").arg(newName));
            return;
        }

        if( ! m_project->renameGrid( t, name, newName) ){
            QMessageBox::critical(this, "Rename Grid", QString("Renaming grid %1 failed!").arg(name));
        }

        updateProjectViews();
    }
}

void ProjectViewer::removeGrid( GridType t, const QString& name){

    if( m_project->gridList(t).contains(name)){
        //if( !m_project->gridList().contains(name) ) return;

        QString typeName=gridType2String(t);

        if( QMessageBox::question(this, QString("Remove %1").arg(typeName),
                 QString("Are you sure you want to remove %1 grid %2?").arg(typeName,name), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
            return;
        }

         if( !m_project->removeGrid( t, name)){
             QMessageBox::critical(this, QString("Remove %1").arg(typeName), QString("Removing %1 grid %2 failed!").arg(typeName, name));
         }

         updateProjectViews();
    }
}


void ProjectViewer::selectAndExportVolume()
{
    bool ok=false;
    QString volumeName = QInputDialog::getItem(this, QString("Export Volume"),
                                         tr("Select Volume:"), m_project->volumeList(), 0, false, &ok);
    if (ok && !volumeName.isEmpty()){

        exportVolume(volumeName);
    }
}

bool computeTransforms(const ProjectGeometry& geom, QTransform &transformXYToIlXl, QTransform &transformIlXlToXY){

    const qreal EPS=0.001;

    QPoint inlineAndCrossline1=geom.inlineAndCrossline(0);
    QPointF coords1=geom.coordinates(0);
    QPoint inlineAndCrossline2=geom.inlineAndCrossline(1);
    QPointF coords2=geom.coordinates(1);
    QPoint inlineAndCrossline3=geom.inlineAndCrossline(2);
    QPointF coords3=geom.coordinates(2);

    int il1=inlineAndCrossline1.x();
    int xl1=inlineAndCrossline1.y();
    int il2=inlineAndCrossline2.x();
    int xl2=inlineAndCrossline2.y();
    int il3=inlineAndCrossline3.x();
    int xl3=inlineAndCrossline3.y();

    int x1=coords1.x();
    int y1=coords1.y();
    int x2=coords2.x();
    int y2=coords2.y();
    int x3=coords3.x();
    int y3=coords3.y();

    int il21=il2-il1;
    int il31=il3-il1;
    int xl21=xl2-xl1;
    int xl31=xl3-xl1;
    qreal x21=x2-x1;
    qreal x31=x3-x1;
    qreal y21=y2-y1;
    qreal y31=y3-y1;

    qreal D=x31*y21 - x21*y31;
    if( std::fabs(D)<EPS ){
        return false;
    }

    qreal m11=( il21*y31 + il31*y21 ) /D;

    if( std::fabs(y21)<EPS && std::fabs(y31)<EPS){
        return false;
    }

    qreal m21=( std::fabs(y21) > std::fabs(y31) ) ?
                ( il21 - m11*x21 ) / y21 :
                ( il31 - m11*x31 ) / y31;

    qreal dx= il1 - m11*x1 - m21*y1;


    D=y21*x31 - y31*x21;
    if( std::fabs(D)<EPS ){
        return false;
    }

    qreal m22=( xl21*x31 - xl31*x21 ) / D;

    if( std::fabs(y21)<EPS && std::fabs(y31)<EPS ){
        return false;
    }

    qreal m12=(std::fabs(x21) > std::fabs(x31) ) ?
                ( xl21 - m22*y21 ) / x21 :
                ( xl31 - m22*y31 ) / x31;

    qreal dy= xl1- m22*y1 - m12*x1;

    transformXYToIlXl.setMatrix( m11, m12, 0, m21, m22, 0, dx, dy, 1);
    bool ok=false;
    transformIlXlToXY=transformXYToIlXl.inverted(&ok);
    return ok;
}

// XXX make this a process later
void ProjectViewer::exportVolume( QString volumeName){

    const int SCALCO=-10;

    QTransform IlXlToXY;
    QTransform XYToIlXl;
    if( !computeTransforms(m_project->geometry(), XYToIlXl, IlXlToXY)){
        QMessageBox::critical(this, "Export Volume", "Invalid geometry, failed to compute transformations!");
        return;
    }


    std::shared_ptr<Grid3D<float>> volume=m_project->loadVolume(volumeName);
    if(!volume){
        QMessageBox::critical(this, "Export Volume", QString("Loading volume %1 failed!").arg(volumeName));
        return;
    }

    QString fileName=QFileDialog::getSaveFileName(this, "Choose exported volume SEGY file", QDir::homePath(), "*.segy");
    if( fileName.isNull()) return;



    Grid3DBounds bounds=volume->bounds();

    QProgressDialog progress(this);
    progress.setWindowTitle("Export Volume");
    progress.setLabelText("Writing SEGY");
    progress.setRange(0, bounds.inlineCount());
    progress.show();

    double ft=bounds.ft();
    size_t ns=bounds.sampleCount();
    size_t dt=1000000*bounds.dt();   // make microseconds

    seismic::SEGYTextHeaderStr textHeaderStr;
    textHeaderStr.push_back("SEGY created with AVOUtensil");
    textHeaderStr.push_back(QString("Content:     Volume %1").arg(volumeName).toStdString());
    textHeaderStr.push_back(QString("Inlines:     %1 - %2").arg(bounds.inline1()).arg(bounds.inline2()).toStdString());
    textHeaderStr.push_back(QString("Crosslines:  %1 - %2").arg(bounds.crossline1()).arg(bounds.crossline2()).toStdString());
    textHeaderStr.push_back("");
    textHeaderStr.push_back("Trace Header Definition:");
    textHeaderStr.push_back("Time of first sample [ms]  bytes 109-110");
    textHeaderStr.push_back("Inline                     bytes 189-192");
    textHeaderStr.push_back("Crossline                  bytes 193-196");
    textHeaderStr.push_back("X-Coordinate               bytes 181-184");
    textHeaderStr.push_back("Y-Coordinate               bytes 185-188");
    seismic::SEGYTextHeader textHeader=seismic::convertToRaw(textHeaderStr);

    seismic::Header bhdr;
    bhdr["format"]=seismic::HeaderValue::makeIntValue( toInt( seismic::SEGYSampleFormat::IEEE) );
    bhdr["ns"]=seismic::HeaderValue::makeUIntValue(ns);
    bhdr["dt"]=seismic::HeaderValue::makeUIntValue(dt);

    seismic::SEGYInfo info;
    info.setScalco(-1./SCALCO);
    info.setSwap(true);     // XXX for little endian machines , NEED to automatically use endianess of machine a compile time
    info.setSampleFormat(seismic::SEGYSampleFormat::IEEE);
    //seismic::SEGYWriter writer( fileName.toStdString(), info, textHeader, bhdr );
    std::unique_ptr<seismic::SEGYWriter> writer( new seismic::SEGYWriter(
                fileName.toStdString(), info, textHeader, bhdr ) );
    writer->write_leading_headers();


    seismic::Trace trace(bounds.ft(), bounds.dt(), bounds.sampleCount() );
    seismic::Header& thdr=trace.header();
    seismic::Trace::Samples& samples=trace.samples();

    thdr["delrt"]=seismic::HeaderValue::makeIntValue(1000*ft);
    thdr["ns"]=seismic::HeaderValue::makeUIntValue(ns);
    thdr["dt"]=seismic::HeaderValue::makeUIntValue(dt);
    thdr["scalco"]=seismic::HeaderValue::makeIntValue(SCALCO);

    int n=0;
    for( int iline=bounds.inline1(); iline<=bounds.inline2(); iline++){

        for( int xline=bounds.crossline1(); xline<=bounds.crossline2(); xline++){

            n++;

            QPointF p=IlXlToXY.map( QPoint(iline, xline));
            qreal x=p.x();
            qreal y=p.y();

            int cdp=1 + (iline-bounds.inline1())*bounds.crosslineCount() + xline - bounds.crossline1();

            thdr["cdp"]=seismic::HeaderValue::makeIntValue(cdp);                                        // XXX, maybe read from db once
            thdr["tracr"]=seismic::HeaderValue::makeIntValue(n);
            thdr["tracl"]=seismic::HeaderValue::makeIntValue(n);
            thdr["iline"]=seismic::HeaderValue::makeIntValue(iline);
            thdr["xline"]=seismic::HeaderValue::makeIntValue(xline);
            thdr["sx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["sy"]=seismic::HeaderValue::makeFloatValue(y);
            thdr["gx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["gy"]=seismic::HeaderValue::makeFloatValue(y);
            thdr["cdpx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["cdpy"]=seismic::HeaderValue::makeFloatValue(y);

            for( int i=0; i<bounds.sampleCount(); i++){

                float s=(*volume)(iline,xline,i);

                // write zero for NULL values, maybe make this selectable?
                if( s==volume->NULL_VALUE ){
                    s=0;
                }

                samples[i]=s;
            }

            //std::cout<<"n="<<n<<" cdp="<<cdp<<" iline="<<iline<<" xline="<<xline<<std::endl;
            writer->write_trace(trace);
        }

        progress.setValue(iline-bounds.inline1());
        qApp->processEvents();
        if( progress.wasCanceled()){
            // delete partial file
            QFile(fileName).remove();
            return;
        }
    }
}


// XXX make this a process later
void ProjectViewer::exportVolumeSeisware( QString volumeName){

    const int SCALCO=-10;

    QTransform IlXlToXY;
    QTransform XYToIlXl;
    if( !computeTransforms(m_project->geometry(), XYToIlXl, IlXlToXY)){
        QMessageBox::critical(this, "Export Volume", "Invalid geometry, failed to compute transformations!");
        return;
    }

    std::shared_ptr<Grid3D<float>> volume=m_project->loadVolume(volumeName);
    if(!volume){
        QMessageBox::critical(this, "Export Volume", QString("Loading volume %1 failed!").arg(volumeName));
        return;
    }

    QString fileName=QFileDialog::getSaveFileName(this,
        "Choose exported volume Seisware SEGY file", QDir::homePath(), "*.sgy");
    if( fileName.isNull()) return;

    QFileInfo fi(fileName);
    QString lname=fi.fileName().split(".").at(0);
    std::cout<<"linename: "<<lname.toStdString()<<std::endl<<std::flush;

    Grid3DBounds bounds=volume->bounds();

    // gather volume statistics
    float peak=std::numeric_limits<float>::lowest();
    double sum=0;
    double sum2=0;
    size_t n=0;
    for( size_t i=0; i<volume->size(); i++){
        const float& x=(*volume)[i];
        if( x==volume->NULL_VALUE) continue;
        if(x>peak) peak=x;
        sum+=std::fabs(x);
        sum2+=x*x;
        n++;
    }
    float average=(n>0)?sum/n : 0 ;
    float rms=(n>0) ? std::sqrt(sum2)/n : 0;

// TEST
//peak=average=rms=-1;

    double ft=bounds.ft();
    size_t ns=bounds.sampleCount();
    size_t dt=1000000*bounds.dt();   // make microseconds

    seismic::SEGYTextHeaderStr textHeaderStr;
    textHeaderStr.push_back("Seisware SEGY created with AVOUtensil");
    textHeaderStr.push_back(QString("Content:     Volume %1").arg(volumeName).toStdString());
    textHeaderStr.push_back(QString("Inlines:     %1 - %2").arg(bounds.inline1()).arg(bounds.inline2()).toStdString());
    textHeaderStr.push_back(QString("Crosslines:  %1 - %2").arg(bounds.crossline1()).arg(bounds.crossline2()).toStdString());
    seismic::SEGYTextHeader textHeader=seismic::convertToRaw(textHeaderStr);

    seismic::Header bhdr;
    bhdr["format"]=seismic::HeaderValue::makeIntValue( toInt( seismic::SEGYSampleFormat::IEEE) );
    bhdr["ns"]=seismic::HeaderValue::makeUIntValue(ns);
    bhdr["dt"]=seismic::HeaderValue::makeUIntValue(dt);
    bhdr["sswnd"]=seismic::HeaderValue::makeFloatValue(1000000*bounds.ft()); //microsecs
    bhdr["eswnd"]=seismic::HeaderValue::makeFloatValue(1000000*bounds.lt()); //us
    bhdr["speak"]=seismic::HeaderValue::makeFloatValue(peak);
    bhdr["savg"]=seismic::HeaderValue::makeFloatValue(average);
    bhdr["srms"]=seismic::HeaderValue::makeFloatValue(rms);
    bhdr["startx"]=seismic::HeaderValue::makeIntValue(bounds.crossline1());
    bhdr["starty"]=seismic::HeaderValue::makeIntValue(bounds.inline1());
    bhdr["startz"]=seismic::HeaderValue::makeIntValue(1000000*bounds.ft());
    bhdr["endx"]=seismic::HeaderValue::makeIntValue(bounds.crossline2());
    bhdr["endy"]=seismic::HeaderValue::makeIntValue(bounds.inline2());
    bhdr["endz"]=seismic::HeaderValue::makeIntValue(1000000*bounds.lt());
    bhdr["totphase"]=seismic::HeaderValue::makeFloatValue(0);
    bhdr["totgain"]=seismic::HeaderValue::makeFloatValue(1);
    bhdr["gainexp"]=seismic::HeaderValue::makeFloatValue(1);
    bhdr["corner1"]=seismic::HeaderValue::makeIntValue(0);
    bhdr["corner2"]=seismic::HeaderValue::makeIntValue(bounds.crosslineCount()-1);
    bhdr["corner3"]=seismic::HeaderValue::makeIntValue(bounds.inlineCount()*bounds.crosslineCount()-1);
    bhdr["geometry"]=seismic::HeaderValue::makeIntValue(3);
    bhdr["swflag"]=seismic::HeaderValue::makeUIntValue(91);

    // added the following keys as compatability test
    bhdr["dum1"]=seismic::HeaderValue::makeIntValue(1);
    bhdr["dum2"]=seismic::HeaderValue::makeIntValue(2);
    bhdr["dum3"]=seismic::HeaderValue::makeIntValue(3);
    bhdr["dum4"]=seismic::HeaderValue::makeIntValue(4);
    bhdr["dum5"]=seismic::HeaderValue::makeIntValue(32040);

    seismic::SEGYInfo info;
    info.setBinaryHeaderDef( seismic::SEISWARE_BINARY_HEADER_DEF );
    info.setTraceHeaderDef( seismic::SEISWARE_TRACE_HEADER_DEF );
    info.setScalco(-1./SCALCO);
    info.setSwap(false);//true);     // XXX for little endian machines , NEED to automatically use endianess of machine a compile time
    info.setSampleFormat(seismic::SEGYSampleFormat::IEEE);
    //seismic::SWSEGYWriter writer( fileName.toStdString(), info, textHeader, bhdr );
    std::unique_ptr<seismic::SEGYWriter> writer( new seismic::SWSEGYWriter(
                fileName.toStdString(), info, textHeader, bhdr, lname.toStdString() ) );
    writer->write_leading_headers();

    seismic::Trace trace(bounds.ft(), bounds.dt(), bounds.sampleCount() );
    seismic::Header& thdr=trace.header();
    seismic::Trace::Samples& samples=trace.samples();


    QProgressDialog progress(this);
    progress.setWindowTitle("Export Volume");
    progress.setLabelText("Writing Seisware SEGY");
    progress.setRange(0, bounds.inlineCount());
    progress.show();

    int tn=0;
    for( int iline=bounds.inline1(); iline<=bounds.inline2(); iline++){

        for( int xline=bounds.crossline1(); xline<=bounds.crossline2(); xline++){

            tn++;

            // gather trace sample statistics and fill trace
            float peak=std::numeric_limits<float>::lowest();
            double sum=0;
            double sum2=0;
            size_t n=0;
            for( int i=0; i<bounds.sampleCount(); i++){

                float s=(*volume)(iline,xline,i);

                // write zero for NULL values, maybe make this selectable?
                if( s==volume->NULL_VALUE ){
                    s=0;
                }
                else{
                    if(s>peak) peak=s;
                    sum+=std::fabs(s);
                    sum2+=s*s;
                    n++;
                }

                samples[i]=s;
            }
            float average=(n>0)?sum/n : 0;
            float rms=(n>0)? std::sqrt(sum2)/n : 0;

            int cdp=1 + (iline-bounds.inline1())*bounds.crosslineCount() + xline - bounds.crossline1();

            QPointF p=IlXlToXY.map( QPoint(iline, xline));
            qreal x=p.x();
            qreal y=p.y();

            thdr["tracl"]=seismic::HeaderValue::makeIntValue(tn); // added additional to spec
            thdr["ns"]=seismic::HeaderValue::makeUIntValue(ns);
            thdr["dt"]=seismic::HeaderValue::makeUIntValue(dt);
            thdr["iline"]=seismic::HeaderValue::makeIntValue(iline);
            thdr["xline"]=seismic::HeaderValue::makeIntValue(xline);
            thdr["cdp"]=seismic::HeaderValue::makeIntValue(cdp);
            thdr["x"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["y"]=seismic::HeaderValue::makeFloatValue(y);
            bhdr["speak"]=seismic::HeaderValue::makeFloatValue(peak);
            bhdr["savg"]=seismic::HeaderValue::makeFloatValue(average);
            bhdr["srms"]=seismic::HeaderValue::makeFloatValue(rms);

            std::cout<<"n="<<tn<<" cdp="<<cdp<<" iline="<<iline<<" xline="<<xline<<std::endl;
            writer->write_trace(trace);
        }

        progress.setValue(iline-bounds.inline1());
        qApp->processEvents();
        if( progress.wasCanceled()){
            // delete partial file
            QFile(fileName).remove();
            return;
        }     
    }


    // build index file
    QFileInfo finfo(fileName);
    QString indexName = QDir(finfo.path()).filePath(
                finfo.completeBaseName() + ".indx");

    std::cout<<"index file: "<<indexName.toStdString().c_str()<<std::endl;

    std::ofstream os(indexName.toStdString().c_str(),
                     std::ios::out | std::ios::binary);
    for( int iline=bounds.inline1(); iline<=bounds.inline2(); iline++){

        int32_t offset=static_cast<int32_t>((iline-bounds.inline1())*bounds.crosslineCount());
        int32_t line=static_cast<int32_t>(iline);
        int32_t trace=static_cast<int32_t>(bounds.crossline1());
        int32_t numTraces=static_cast<int32_t>(bounds.crosslineCount());

        uint32_t u=htonl(static_cast<uint32_t>(offset));
        os.write((char*)&u, sizeof(u));

        u=htonl(static_cast<uint32_t>(line));
        os.write((char*)&u, sizeof(u));

        u=htonl(static_cast<uint32_t>(trace));
        os.write((char*)&u, sizeof(u));

        u=htonl(static_cast<uint32_t>(numTraces));
        os.write((char*)&u, sizeof(u));

    }
}

void ProjectViewer::renameVolume( const QString& name){

    if( m_project->volumeList().contains(name)){
        bool ok=false;

        QString newName=QInputDialog::getText(this, QString("Rename Volume \"%1\"").arg(name), "New Name:", QLineEdit::Normal, name, &ok);

        if( newName.isNull() || !ok) return;

        if( newName==name) return;

        if( m_project->volumeList().contains(newName) ){
            QMessageBox::information(this, "Rename Volume", QString("Volume %1 already exists!").arg(newName));
            return;
        }

        if( ! m_project->renameVolume( name, newName) ){
            QMessageBox::critical(this, "Rename Volume", QString("Renaming volume %1 failed!").arg(name));
        }

        updateProjectViews();
    }
}

void ProjectViewer::removeVolume( const QString& name){

    if( m_project->volumeList().contains(name)){

        if( QMessageBox::question(this, QString("Remove Volume"),
                 QString("Are you sure you want to remove volume %1?").arg(name), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
            return;
        }

         if( !m_project->removeVolume( name)){
             QMessageBox::critical(this, QString("Remove Volume"), QString("Removing volume %1 failed!").arg(name));
         }

         updateProjectViews();
    }
}

void ProjectViewer::displaySeismicDataset(const QString& name){

    if( m_project->seismicDatasetList().contains(name)){

        std::shared_ptr<SeismicDatasetReader> reader=m_project->openSeismicDataset(name);
        if( !reader){
            QMessageBox::critical(this, "Display Seismic Dataset", "Open reader failed!");
            return;
        }

        SeismicDatasetInfo info=m_project->getSeismicDatasetInfo(name);

        int minIline=reader->minInline();
        int maxIline=reader->maxInline();
        int minXline=reader->minCrossline();
        int maxXline=reader->maxCrossline();

        SeismicDataSelector* selector=new SeismicDataSelector(this);
        selector->setInlineRange(minIline, maxIline);
        selector->setCrosslineRange(minXline, maxXline);
        selector->setInlineCountRange(1,maxIline-minIline+1);
        selector->setCrosslineCountRange(1,maxXline-minXline+1);
        selector->setInline(minIline);
        selector->setCrossline(minXline);
        selector->setReader(reader);



        QString verticalAxisLabel;
        if( info.domain()==SeismicDatasetInfo::Domain::Time){
            verticalAxisLabel="Time [Milliseconds]";
        }
        else if( info.domain()==SeismicDatasetInfo::Domain::Depth){
            verticalAxisLabel="Depth [Meters]";
        }

        GatherViewer* viewer=new GatherViewer;//(this);
        viewer->setDispatcher(m_dispatcher);
        viewer->setWindowTitle( name );
        if( info.mode()==SeismicDatasetInfo::Mode::Prestack){
            selector->setOrder(SeismicDataSelector::ORDER_INLINE_ASCENDING,
                               SeismicDataSelector::ORDER_CROSSLINE_ASCENDING,
                               SeismicDataSelector::ORDER_OFFSET_ASCENDING);
            selector->setInlineCount(1);
            selector->setCrosslineCount(10);
            std::pair<int, int> ftIlXl=reader->firstTraceInlineCrossline();
            selector->setInline(ftIlXl.first);
            selector->setCrossline(ftIlXl.second);
            GatherLabel* gatherLabel=viewer->view()->gatherLabel();
            gatherLabel->setDisplayWiggles(true);
            gatherLabel->setDisplayVariableArea(true);
            gatherLabel->setDisplayDensity(false);

            std::vector< std::pair< std::string, QString> > annos;
            annos.push_back( std::pair< std::string, QString >( "iline", QString("Inline")));
            annos.push_back( std::pair< std::string, QString >( "xline", QString("Crossline")));
            annos.push_back( std::pair< std::string, QString >( "offset", QString("Offset")));
            viewer->setTraceAnnotations(annos);
            /*
            viewer->view()->axxisLabelWidget()->setLabels(QStringList()<<QString("Offset:")<<QString("Crossline:")<<QString("Inline:") );
            viewer->view()->setTraceAnnotationFunction(
                        [](const seismic::Header& hdr){return QStringList()<<QString::number(hdr.at("offset").floatValue() )
                                                                           <<QString::number(hdr.at("xline").intValue() )
                                                                           <<QString::number(hdr.at("iline").intValue() ) ;}
                    );*/

        }
        else if(info.mode()==SeismicDatasetInfo::Mode::Poststack){
            selector->setOrder(SeismicDataSelector::ORDER_INLINE_ASCENDING,
                               SeismicDataSelector::ORDER_CROSSLINE_ASCENDING,
                               SeismicDataSelector::ORDER_NONE);
            selector->setInlineCount(1);
            selector->setCrosslineCount(maxXline-minXline+1);

            GatherLabel* gatherLabel=viewer->view()->gatherLabel();
            gatherLabel->setDisplayWiggles(false);
            gatherLabel->setDisplayVariableArea(false);
            gatherLabel->setDisplayDensity(true);

            std::vector< std::pair< std::string, QString> > annos;
            annos.push_back( std::pair< std::string, QString >( "iline", QString("Inline")));
            annos.push_back( std::pair< std::string, QString >( "xline", QString("Crossline")));
            viewer->setTraceAnnotations(annos);
            /*
            viewer->view()->axxisLabelWidget()->setLabels(QStringList()<<QString("Crossline:")<<QString("Inline:") );
            viewer->view()->setTraceAnnotationFunction(
                        [](const seismic::Header& hdr){return QStringList()<<QString::number(hdr.at("xline").intValue() )
                                                                           <<QString::number(hdr.at("iline").intValue() ) ;}
                    );*/
        }



        connect( selector, SIGNAL(gatherChanged(std::shared_ptr<seismic::Gather>)), viewer, SLOT(setGather(std::shared_ptr<seismic::Gather>)));
        connect( viewer, SIGNAL(requestPoint(int,int)), selector,SLOT(providePoint(int,int)));
        connect( viewer, SIGNAL(requestPoints(QVector<QPoint>)), selector, SLOT( provideRandomLine(QVector<QPoint>)) );

        viewer->mainToolBar()->addWidget(selector);
        //viewer->view()->leftRuler()->setAxxisLabel(verticalAxisLabel);
        viewer->show();
        viewer->setMinimumWidth(viewer->mainToolBar()->width() + 100 );

        viewer->setProject( m_project );  // grant access to grids

        selector->apply();      // trigger reading of first gather
        viewer->zoomFitWindow();
        viewer->setTraceHeaderDef(reader->segyReader()->info().traceHeaderDef());

    }

}





void ProjectViewer::displaySeismicDatasetIndex(const QString& name){

    if( m_project->seismicDatasetList().contains(name)){

        QString path=m_project->getSeismicDatasetIndexPath(name);
        if( name.isNull()){
            QMessageBox::critical(this, "Display Dataset Index", "No index path!");
            return;
        }

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(path);
        db.open();

        QString tableName="map";

        QSqlTableModel* model = new QSqlTableModel(this, db);
        model->setTable(tableName);
        model->select();

        QSqlRecord record=db.driver()->record(tableName);

        for( int i=0; i<record.count(); i++){
            model->setHeaderData(i, Qt::Horizontal, record.fieldName(i));
        }

        DatabaseViewer* viewer=new DatabaseViewer(this);
        viewer->setData( model );
        viewer->setWindowTitle(QString("Index of %1").arg(name));
        viewer->show();

    }

}


void ProjectViewer::editSeismicDatasetProperties(const QString& name){

    if( m_project->seismicDatasetList().contains(name)){

        DatasetPropertiesDialog dlg;
        dlg.setWindowTitle(QString("Properties of %1").arg(name));
        SeismicDatasetInfo info=m_project->getSeismicDatasetInfo(name);
        dlg.setName( name );
        dlg.setDomain( info.domain());
        dlg.setMode(info.mode());

        if( dlg.exec()==QDialog::Accepted){

            if( info.domain()!=dlg.domain() || info.mode()!=dlg.mode() ){

                info.setDomain(dlg.domain());
                info.setMode(dlg.mode());
                if( !m_project->setSeismicDatasetInfo(name, info) ){
                    QMessageBox::critical(this, "Change Dataset Properties", QString("Setting properties of \"%1\" failed!").arg(name));
                    return;
                }
            }

            if( dlg.name()!=name){
                if( ! m_project->renameSeismicDataset(name, dlg.name()) ){
                    QMessageBox::critical(this, "Rename Dataset", QString("Renaming dataset %1 failed!").arg(name));
                    return;

                }
                updateProjectViews();
            }

        }


    }
}


void ProjectViewer::renameSeismicDataset(const QString& name){

    if( m_project->seismicDatasetList().contains(name)){
        bool ok=false;

        QString newName=QInputDialog::getText(this, QString("Rename Dataset %1").arg(name), "New Name:", QLineEdit::Normal, name, &ok);

        if( newName.isNull() || !ok) return;

        if( newName==name) return;

        if( m_project->seismicDatasetList().contains(newName) ){
            QMessageBox::information(this, "Rename Dataset", QString("Dataset %1 already exists!").arg(newName));
            return;
        }

        if( ! m_project->renameSeismicDataset(name, newName) ){
            QMessageBox::critical(this, "Rename Dataset", QString("Renaming dataset %1 failed!").arg(name));
        }

        updateProjectViews();
    }
}

void ProjectViewer::removeSeismicDataset(const QString& name){

    if( m_project->seismicDatasetList().contains(name)){

        if( QMessageBox::question(this, "Remove Seismic Dataset",
                 QString("Are you sure you want to remove dataset %1?").arg(name), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
            return;
        }

         if( !m_project->removeSeismicDataset(name)){
             QMessageBox::critical(this, "Remove Dataset", QString("Removing dataset %1 failed!").arg(name));
         }

         updateProjectViews();
    }
}


void ProjectViewer::setProjectFileName( const QString& fileName){

    m_projectFileName=fileName;

    setWindowTitle(QString("AVOProject - \"%1\"").arg(m_projectFileName));
}

bool ProjectViewer::saveProject( const QString& fileName){

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Save Project"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    XPRWriter writer(*m_project);
    if( !writer.writeFile(&file) ){
        QMessageBox::warning(this, tr("Save Project"),
                             tr("Saving project failed!"));
        return false;
    }

    return true;
}

void ProjectViewer::updateProjectViews(){
if(!m_project) return;
    QStringListModel* datasetsModel=new QStringListModel( m_project->seismicDatasetList(), this);
    ui->datasetsView->setModel(datasetsModel);

    QStringList horizonsList= m_project->gridList( GridType::Horizon);

    QStringListModel* horizonsModel=new QStringListModel( m_project->gridList( GridType::Horizon), this);
    ui->horizonsView->setModel(horizonsModel);

    QStringListModel* attributesModel=new QStringListModel( m_project->gridList( GridType::Attribute), this);
    ui->attributesView->setModel(attributesModel);

    QStringListModel* gridsModel=new QStringListModel( m_project->gridList( GridType::Other), this);
    ui->gridsView->setModel(gridsModel);

    QStringListModel* volumesModel=new QStringListModel( m_project->volumeList(), this);
    ui->volumesView->setModel(volumesModel);
}



void ProjectViewer::runProcess( ProjectProcess* process, QMap<QString, QString> params)
{

    const int FINISH_MESSAGE_MILLIS=3000;

    Q_ASSERT( process );

    auto start = std::chrono::steady_clock::now();

    if( process->init(params)!=ProjectProcess::ResultCode::Ok ){

        QMessageBox::critical(this, process->name(), process->lastError());
        delete process;
        return;
    }

    QProgressDialog* progress=new QProgressDialog(this);        // add destroy on close??
    progress->setWindowTitle(process->name());
    connect( process, SIGNAL( currentTask(QString)), progress, SLOT( setLabelText(QString)) );
    connect( process, SIGNAL( started(int)), progress, SLOT(setMaximum(int)) );
    connect( process, SIGNAL( progress(int)), progress, SLOT(setValue(int)) );
    connect( process, SIGNAL( finished()), progress, SLOT(close()));
    connect( progress, SIGNAL(canceled()), process, SLOT( cancel()));
    progress->show();

    ProjectProcess::ResultCode code=process->run();
    if( code == ProjectProcess::ResultCode::Error){
        QMessageBox::critical(this, process->name(), process->lastError());
    }
    else if( code== ProjectProcess::ResultCode::Canceled){
        QMessageBox::information(this, process->name(), "Process was canceled by user.");
    }

    QString name=process->name();

    delete progress;
    delete process;


    if( code != ProjectProcess::ResultCode::Ok){
        return;
    }

    updateProjectViews();

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<float> diff = end - start;
    QString message=QString("Process %1 finished after %2 seconds").arg(name).arg(diff.count(), 0, 'g', 3);

    statusBar()->showMessage(message, FINISH_MESSAGE_MILLIS );


    if( params.contains(QString("display-grid"))){
        for( QString gridName : params.values(QString("display-grid"))){
            auto gridTypeAndName=splitFullGridName(gridName);
            displayGrid( gridTypeAndName.first, gridTypeAndName.second);
        }
    }

}


void ProjectViewer::saveSettings(){

     QSettings settings(COMPANY, "ProjectViewer");

     settings.beginGroup("ProjectViewer");
       settings.setValue("size", size());
       settings.setValue("position", pos() );
     settings.endGroup();

     settings.sync();
}


void ProjectViewer::loadSettings(){

    QSettings settings(COMPANY, "ProjectViewer");

    settings.beginGroup("ProjectViewer");
        resize(settings.value("size", QSize(400, 600)).toSize());
        move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();


}


void ProjectViewer::updateMenu(){

    bool isProject=false;
    if( m_project) isProject=true;//(m_project)?true:false;

    ui->actionSave->setEnabled(isProject);
    ui->actionSaveProjectAs->setEnabled(isProject);
    ui->actionImportSeismic->setEnabled( isProject );
    ui->actionImportHorizon->setEnabled( isProject );
    ui->actionImportAttributeGrid->setEnabled(isProject);
    ui->actionImportOtherGrid->setEnabled(isProject);
    ui->actionImportVolume->setEnabled(isProject);
    ui->actionExportHorizon->setEnabled(isProject);
    ui->actionExportOtherGrid->setEnabled(isProject);
    ui->actionExportAttributeGrid->setEnabled(isProject);
    ui->actionExportVolume->setEnabled(isProject);
    ui->actionExportSeismic->setEnabled(isProject);
    ui->actionOpenGrid->setEnabled( isProject);
    ui->actionOpenSeismicDataset->setEnabled( isProject);

    ui->action_Geometry->setEnabled(isProject);

    ui->actionCreateTimeslice->setEnabled(isProject);
    ui->actionHorizon_Amplitudes->setEnabled(isProject);
    ui->actionHorizon_Semblance->setEnabled(isProject);
    ui->actionCompute_Intercept_Gradient->setEnabled(isProject);
    ui->actionCompute_Intercept_and_Gradient_Volumes->setEnabled(isProject);
    ui->actionFluid_Factor_Grid->setEnabled(isProject);
    ui->actionFluid_Factor_Volume->setEnabled(isProject);
    ui->actionRun_Grid_User_Script->setEnabled(isProject);
    ui->actionRun_Volume_Script->setEnabled(isProject);

    ui->actionCrossplot_Grids->setEnabled(isProject);
    ui->actionCrossplot_Volumes->setEnabled(isProject);
    ui->actionAmplitude_vs_Offset_Plot->setEnabled(isProject);
}




