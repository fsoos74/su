#include "projectviewer.h"
#include "ui_projectviewer.h"

#include "compasswidget.h"
#include "seismicdataselector.h"
#include "seismicsliceselector.h"
#include "volumesliceselector.h"
#include "volumerelativesliceselector.h"
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
#include<QDateTime>

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
#include<functional>
using namespace std::placeholders; // for _1, _2 etc.

#include<projectgeometrydialog.h>
#include<orientationdialog.h>
#include<exportvolumeprocess.h>
#include<exportvolumedialog.h>

#include<exportseismicprocess.h>
#include<exportseismicdialog.h>

#include <createtimesliceprocess.h>
#include <createtimeslicedialog.h>
#include <cropgriddialog.h>
#include <cropgridprocess.h>
#include <horizonamplitudesprocess.h>
#include <horizonamplitudesdialog.h>
#include <horizonsemblanceprocess.h>
#include <horizonsemblancedialog.h>
#include <semblancevolumeprocess.h>
#include <semblancevolumedialog.h>
#include <computeinterceptgradientdialog.h>
#include <computeinterceptgradientprocess.h>
#include <interceptgradientvolumedialog.h>
#include <interceptgradientvolumeprocess.h>
#include <fluidfactorprocess.h>
#include <fluidfactordialog.h>
#include <fluidfactorvolumeprocess.h>
#include <fluidfactorvolumedialog.h>
#include <trendbasedattributesprocess.h>
#include <trendbasedattributevolumesprocess.h>
#include <trendbasedattributesdialog.h>
#include <secondaryattributesprocess.h>
#include <secondaryattributevolumesprocess.h>
#include <secondaryavoattributesdialog.h>
#include <cropvolumedialog.h>
#include <cropvolumeprocess.h>
#include <amplitudevolumedialog.h>
#include <amplitudevolumeprocess.h>
#include <curvaturevolumedialog.h>
#include <curvaturevolumeprocess.h>
#include <convertgriddialog.h>
#include <convertgridprocess.h>
#include <smoothgriddialog.h>
#include <smoothgridprocess.h>
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

#include <reversespinbox.h>

#include <mapviewer.h>
#include <volumeviewer.h>


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
#include<cryptfunc.h>
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

    QAction* recentProjectAction = nullptr;
    for(int i = 0; i < maxRecentProjectCount; ++i){
        recentProjectAction = new QAction(this);
        recentProjectAction->setVisible(false);
        connect(recentProjectAction, SIGNAL(triggered()),this, SLOT(openRecent()));
        recentProjectActionList.append(recentProjectAction);
        ui->menu_Recent_Projects->addAction(recentProjectActionList.at(i));
    }

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

    updateRecentProjectActionList();  // after loadSettings because list is stored in config file

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

void ProjectViewer::openRecent(){
    QAction *action = qobject_cast<QAction *>(sender());
    if (action){
        openProject(action->data().toString());
    }
}

void ProjectViewer::adjustForCurrentProject(const QString &filePath){

    recentProjectFileList.removeAll(filePath);
    recentProjectFileList.prepend(filePath);
    while (recentProjectFileList.size() > maxRecentProjectCount ){
        recentProjectFileList.removeLast();
    }

    updateRecentProjectActionList();
}

void ProjectViewer::updateRecentProjectActionList(){

    for (int i = 0; i < maxRecentProjectCount; i++) {

        if( i<recentProjectFileList.size()){
            QString strippedName = QFileInfo(recentProjectFileList.at(i)).fileName();
            recentProjectActionList.at(i)->setText(strippedName);
            recentProjectActionList.at(i)->setData(recentProjectFileList.at(i));
            recentProjectActionList.at(i)->setVisible(true);
        }
        else{
            recentProjectActionList.at(i)->setVisible(false);
        }
     }
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
    bool validLicense=false;

    std::unique_ptr<CKeylok>  myCKeylok(new CKeylok);
    validLicense=myCKeylok->CheckForKeyLok();

    if( validLicense ){

        myCKeylok->ReadAuthorization();
        info.dongleSerialNumber=myCKeylok->GetSerialNumber();
        info.majorNumber=myCKeylok->ReadMemory(license::MajorNumberAddress);
        info.minorNumber=myCKeylok->ReadMemory(license::MinorNumberAddress);
        info.productVersion=myCKeylok->ReadMemory(license::ProductVersionAddress);
    }
    else{
        info.dongleSerialNumber=0;
        info.majorNumber=0;
        info.minorNumber=0;
        info.productVersion=0;
    }

    if( info.productVersion<1 ){
        validLicense=false;
    }

    if( validLicense ){

        myCKeylok->CheckExpiration();
        switch (myCKeylok->ReturnValue2)
        {
        case LEASEEXPIRED:
            validLicense=false;
            break;
        case SYSDATESETBACK:
            validLicense=false;
            break;
        case NOLEASEDATE:
            validLicense=false;
            break;
        case LEASEDATEBAD:
            validLicense=false;
            break;
        case LASTSYSDATECORRUPT:
            validLicense=false;
            break;
        default:
            unsigned DateRead = myCKeylok->GetExpiration();
            info.expirationMonth=myCKeylok->GetExpMonth(DateRead);
            info.expirationDay=myCKeylok->GetExpDay(DateRead);
            info.expirationYear=myCKeylok->GetExpYear(DateRead);
            break;
        }
    }

    if( !validLicense){
        seismic::SEGYReader::postReadFunc = crypt::decrypt;
    }

    return info;
}


/*
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
*/
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

        ui->actionCloseProject->trigger();

        AVOProject* tmp = new AVOProject(this);
        tmp->setProjectDirectory(dirName);

        m_project = tmp;
        connect( m_project, SIGNAL(changed()), this, SLOT(updateProjectViews()) );
    }
    catch( AVOProject::PathException& ex ){
        QMessageBox::critical( this, "Create new project", ex.what(), QMessageBox::Ok);
    }

    updateMenu();
    //updateProjectViews();
}

void ProjectViewer::on_actionOpenProject_triggered()
{
    QString filter("*.xpr");
    QString fileName=QFileDialog::getOpenFileName( this, tr("Open Project"), QDir::homePath(), filter);
    if( fileName.isNull()) return;

    openProject(fileName);
    /*
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
    */
}



void ProjectViewer::on_actionCloseProject_triggered()
{
    // close all viewers etc, cannot use children because they have 0 parent
    foreach (QWidget *widget, QApplication::allWidgets()){

        if( widget==this ) continue;

        if(widget->isTopLevel() ){
            widget->close();
        }
    }

    delete m_project;
    m_project=nullptr;
    //m_project.reset();
    m_lockfile.reset();

    setProjectFileName(QString());
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

        if( dlg.isUpdateAxes()){

            AxxisOrientation inlineOrientation;
            AxxisDirection inlineDirection;
            AxxisDirection crosslineDirection;
            geom.computeAxxisOrientations(inlineOrientation, inlineDirection, crosslineDirection);

            m_project->setInlineOrientation(inlineOrientation);
            m_project->setInlineDirection(inlineDirection);
            m_project->setCrosslineDirection(crosslineDirection);
        }

    }

    //setDirty(true);
}

void ProjectViewer::on_actionAxis_Orientation_triggered()
{
    OrientationDialog dlg(this);
    dlg.setWindowTitle(tr("Setup Project Axis Orientation"));
    dlg.setInlineOrientation(m_project->inlineOrientation());
    dlg.setInlineDirection(m_project->inlineDirection());
    dlg.setCrosslineDirection(m_project->crosslineDirection());

    if( dlg.exec() ){

        m_project->setInlineOrientation(dlg.inlineOrientation());
        m_project->setInlineDirection(dlg.inlineDirection());
        m_project->setCrosslineDirection(dlg.crosslineDirection());
    }

    //setDirty(true);
}


void ProjectViewer::on_actionDisplay_Map_triggered()
{
    MapViewer* mviewer=new MapViewer;
    mviewer->setWindowTitle(QString("Map of %1").arg(this->windowTitle()));
    mviewer->show();

    mviewer->setProject(m_project);
}

void ProjectViewer::importGrid(GridType gridType){

    QString gridTypeString=toQString(gridType);
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

        //updateProjectViews();


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

/*
    SeismicDatasetInfo info;
    info.setDomain(propertiesDialog.domain());
    info.setMode(propertiesDialog.mode());
*/
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

    //updateProjectViews();
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
    QString gridTypeString=toQString(gridType);

    bool ok;
    QString gridName = QInputDialog::getItem(this, QString("Export %1 Grid").arg(gridTypeString),
                                         tr("Select Grid:"), m_project->gridList(gridType), 0, false, &ok);
    if (ok && !gridName.isEmpty()){

        exportGrid(gridType, gridName);
    }
}


void ProjectViewer::exportGrid( GridType gridType, QString gridName){

    QString gridTypeString=toQString(gridType);

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
    //updateProjectViews();
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

void ProjectViewer::on_actionCrop_Grid_triggered()
{
    Q_ASSERT( m_project );

    CropGridDialog dlg;
    dlg.setWindowTitle(tr("Crop Grid"));

    dlg.setInputGrids( toQString(GridType::Attribute), m_project->gridList(GridType::Attribute));
    dlg.setInputGrids( toQString(GridType::Horizon), m_project->gridList(GridType::Horizon));
    dlg.setInputGrids( toQString(GridType::Other), m_project->gridList(GridType::Other));

    ProjectGeometry geom=m_project->geometry();
    QPoint min=geom.linesRange().first;
    QPoint max=geom.linesRange().second;

    dlg.setInlineRange( min.x(), max.x() );
    dlg.setCrosslineRange( min.y(), max.y() );

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new CropGridProcess( m_project, this ), params );
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

void ProjectViewer::on_action_Crop_Volume_triggered()
{
    Q_ASSERT( m_project );

    CropVolumeDialog dlg;
    dlg.setWindowTitle(tr("Crop Volume"));

    dlg.setInputVolumes(m_project->volumeList());

    ProjectGeometry geom=m_project->geometry();
    QPoint min=geom.linesRange().first;
    QPoint max=geom.linesRange().second;

    dlg.setInlineRange( min.x(), max.x() );
    dlg.setCrosslineRange( min.y(), max.y() );

    dlg.setTimeRange( 0, 99999 );

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new CropVolumeProcess( m_project, this ), params );
}

void ProjectViewer::on_actionVolume_Amplitudes_triggered()
{
    AmplitudeVolumeDialog dlg;
    dlg.setDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Poststack));
    dlg.setReservedVolumes(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new AmplitudeVolumeProcess( m_project, this ), params );
}

void ProjectViewer::on_actionVolume_Semblance_triggered()
{
    SemblanceVolumeDialog dlg;
    dlg.setDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Poststack));
    dlg.setReservedVolumes(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new SemblanceVolumeProcess( m_project, this ), params );
}

void ProjectViewer::on_actionVolume_Curvature_triggered()
{
    CurvatureVolumeDialog dlg;
    dlg.setInputVolumes(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new CurvatureVolumeProcess( m_project, this ), params );
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


void ProjectViewer::on_actionTrend_Based_Attribute_Grids_triggered()
{
    Q_ASSERT(m_project);

    TrendBasedAttributesDialog dlg;
    dlg.setWindowTitle(tr("Trend Based Attribute Grids"));
    dlg.setInterceptList( m_project->gridList(GridType::Attribute));
    dlg.setGradientList( m_project->gridList(GridType::Attribute));
    dlg.setReservedGrids(m_project->gridList(GridType::Attribute));
    dlg.setComputeTrend(true);
    dlg.setTrendIntercept(0);
    dlg.setTrendAngle(0);
    dlg.setVolumeMode(false);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new TrendBasedAttributesProcess( m_project, this ), params );
}


void ProjectViewer::on_actionTrend_Based_Attribute_Volumes_triggered()
{
    Q_ASSERT(m_project);

    TrendBasedAttributesDialog dlg;
    dlg.setWindowTitle(tr("Trend Based Attribute Volumes"));
    dlg.setInterceptList( m_project->volumeList());
    dlg.setGradientList( m_project->volumeList());
    dlg.setReservedGrids(m_project->volumeList());
    dlg.setComputeTrend(true);
    dlg.setTrendIntercept(0);
    dlg.setTrendAngle(0);
    dlg.setVolumeMode(true);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new TrendBasedAttributeVolumesProcess( m_project, this ), params );
}

void ProjectViewer::on_actionSecondary_Attribute_Grids_triggered()
{
    Q_ASSERT(m_project);

    SecondaryAVOAttributesDialog dlg;
    dlg.setWindowTitle(tr("Secondary Attribute Grids"));
    dlg.setInterceptList( m_project->gridList(GridType::Attribute));
    dlg.setGradientList( m_project->gridList(GridType::Attribute));
    dlg.setFluidFactorList( m_project->gridList(GridType::Attribute));
    dlg.setReservedGrids(m_project->gridList(GridType::Attribute));
    dlg.setVolumeMode(false);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new SecondaryAttributesProcess( m_project, this ), params );

}

void ProjectViewer::on_actionSecondary_Attribute_Volumes_triggered()
{
    Q_ASSERT(m_project);

    SecondaryAVOAttributesDialog dlg;
    dlg.setWindowTitle(tr("Secondary Attribute Volumes"));
    dlg.setInterceptList( m_project->volumeList());
    dlg.setGradientList( m_project->volumeList());
    dlg.setFluidFactorList( m_project->volumeList());
    dlg.setReservedGrids(m_project->volumeList());
    dlg.setVolumeMode(true);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new SecondaryAttributeVolumesProcess( m_project, this ), params );
}



void ProjectViewer::on_actionConvert_Grid_triggered()
{
    Q_ASSERT( m_project );

    ConvertGridDialog dlg;
    dlg.setWindowTitle(tr("Convert Grid"));

    dlg.setInputGrids( toQString(GridType::Attribute), m_project->gridList(GridType::Attribute));
    dlg.setInputGrids( toQString(GridType::Horizon), m_project->gridList(GridType::Horizon));
    dlg.setInputGrids( toQString(GridType::Other), m_project->gridList(GridType::Other));

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new ConvertGridProcess( m_project, this ), params );
}

void ProjectViewer::on_actionSmooth_Grid_triggered()
{
    Q_ASSERT( m_project );

    SmoothGridDialog dlg;
    dlg.setWindowTitle(tr("Smooth Grid"));

    dlg.setInputGrids( toQString(GridType::Attribute), m_project->gridList(GridType::Attribute));
    dlg.setInputGrids( toQString(GridType::Horizon), m_project->gridList(GridType::Horizon));
    dlg.setInputGrids( toQString(GridType::Other), m_project->gridList(GridType::Other));

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess( new SmoothGridProcess( m_project, this ), params );
}


void ProjectViewer::on_actionRun_Grid_User_Script_triggered()
{
     Q_ASSERT( m_project );

     GridRunUserScriptDialog dlg;
     dlg.setWindowTitle(tr("Run User Script"));

     dlg.setInputGrids( toQString(GridType::Attribute), m_project->gridList(GridType::Attribute));
     dlg.setInputGrids( toQString(GridType::Horizon), m_project->gridList(GridType::Horizon));
     dlg.setInputGrids( toQString(GridType::Other), m_project->gridList(GridType::Other));

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
    //viewer->setDetailedPointInformation( ( data.size() < MAX_POINTS ) );    // turn detailed info off if there are too many points

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
    viewer->setFixedColor(!volumea);  // if attribute is used use variable color points
    viewer->setData(data); // add data after visible!!!!
    viewer->setAxisLabels(dlg.xName(), dlg.yName());
    viewer->setDetailedPointInformation( ( data.size() < MAX_POINTS ) );    // turn detailed info off if there are too many points
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


void ProjectViewer::on_datasetsView_doubleClicked(const QModelIndex &idx)
{
    QString datasetName=ui->datasetsView->model()->itemData(idx)[Qt::DisplayRole].toString();

    displaySeismicDataset(datasetName);
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

void ProjectViewer::on_horizonsView_doubleClicked(const QModelIndex &idx)
{
    QString name=ui->horizonsView->model()->itemData(idx)[Qt::DisplayRole].toString();
    displayGrid(GridType::Horizon, name);
}

void ProjectViewer::on_attributesView_doubleClicked(const QModelIndex &idx)
{
    QString name=ui->attributesView->model()->itemData(idx)[Qt::DisplayRole].toString();
    displayGrid(GridType::Attribute, name);
}

void ProjectViewer::on_gridsView_doubleClicked(const QModelIndex &idx)
{
    QString name=ui->gridsView->model()->itemData(idx)[Qt::DisplayRole].toString();
    displayGrid(GridType::Other, name);
}

void ProjectViewer::on_volumesView_doubleClicked(const QModelIndex &idx)
{
    QString name=ui->volumesView->model()->itemData(idx)[Qt::DisplayRole].toString();

    displayVolume3D(name);
}

void ProjectViewer::runVolumeContextMenu( const QPoint& pos){

    QListView* view=ui->volumesView;

    QPoint globalPos = view->viewport()->mapToGlobal(pos);
    QModelIndex idx=view->currentIndex();
    QString volumeName=view->model()->itemData(idx)[Qt::DisplayRole].toString();

    QMenu menu;
    menu.addAction("display 3d");
    menu.addAction("display slice");
    menu.addAction("display horizon relative slice");
    menu.addAction("histogram");
    menu.addSeparator();
    menu.addAction("export");
    menu.addAction("export seisware");
    menu.addSeparator();
    menu.addAction("rename");
    menu.addAction("remove");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="display slice" ){
        displayVolumeSlice( volumeName);
    }
    else if( selectedAction->text()=="display 3d" ){
        displayVolume3D( volumeName);
    }
    else if(selectedAction->text()=="display horizon relative slice"){
        displayVolumeRelativeSlice( volumeName);
    }
    else if( selectedAction->text()=="histogram" ){
        displayVolumeHistogram( volumeName);
    }
    else if( selectedAction->text()=="export" ){
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
    menu.addAction("display slice");
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
    else if( selectedAction->text()=="display slice" ){
        displaySeismicDatasetSlice(datasetName);
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
        QString typeName=toQString(t);
        viewer->setWindowTitle(QString("Grid %1 - %2").arg(typeName, name) );

        if( t==GridType::Horizon ){    // the samples of time horizons are millis
            viewer->setTimeSource(GridViewer::TimeSource::TIME_GRID);
        }

        GridView* gridView=viewer->gridView();
        gridView->setInlineOrientation(m_project->inlineOrientation());
        gridView->setInlineDirection(m_project->inlineDirection());
        gridView->setCrosslineDirection(m_project->crosslineDirection());

        /* azimuth function not ready yes, once it works also derive inline,crossline orientations
        CompassWidget* compass=new CompassWidget(this);
        compass->setS1(0.8 * gridView->RULER_HEIGHT);
        double az=m_project->geometry().inlineAzimuth();
        std::cout<<"az="<<az<<std::endl;
        compass->setAzimuth(az);
        gridView->setCornerWidget(compass);
        */

        viewer->show();

        //viewer->orientate(m_project->geometry());  // this is now obtained from project
        viewer->setProject( m_project );    // need this for il/xl coords conversion
        viewer->setGridInfos(t, name);  // need this for reloading, maybe only give this info and let viewer load the grid
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

        HistogramDialog* viewer=new HistogramDialog; //don't make this parent because projectviewer should be able to be displayed over gridviewer
        viewer->setData( data );
        QString typeName= toQString(t);
        viewer->setWindowTitle(QString("Histogram of Grid %1 - %2").arg(typeName, name) );  

        viewer->show();

    }
}


void ProjectViewer::renameGrid( GridType t, const QString& name){

    if( m_project->gridList(t).contains(name)){
        bool ok=false;

        QString typeName=toQString(t);
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

        //updateProjectViews();
    }
}

void ProjectViewer::removeGrid( GridType t, const QString& name){

    if( m_project->gridList(t).contains(name)){
        //if( !m_project->gridList().contains(name) ) return;

        QString typeName=toQString(t);

        if( QMessageBox::question(this, QString("Remove %1").arg(typeName),
                 QString("Are you sure you want to remove %1 grid %2?").arg(typeName,name), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
            return;
        }

         if( !m_project->removeGrid( t, name)){
             QMessageBox::critical(this, QString("Remove %1").arg(typeName), QString("Removing %1 grid %2 failed!").arg(typeName, name));
         }

         //updateProjectViews();
    }
}


void ProjectViewer::selectAndExportVolume()
{
    ExportVolumeDialog dlg(this);
    dlg.setWindowTitle("Export Volume");
    dlg.setVolumes(m_project->volumeList());

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess( new ExportVolumeProcess( m_project, this ), params );
}


void ProjectViewer::displayVolume3D( const QString& name){

    if( m_project->volumeList().contains(name)){

        std::shared_ptr<Grid3D<float> > volume=m_project->loadVolume( name);
        if( !volume ) return;

        VolumeViewer* vviewer=new VolumeViewer;

        vviewer->setWindowTitle( QString("Volume Viewer").arg(name) );
        vviewer->setProject(m_project);
        vviewer->setDispatcher(m_dispatcher);
        vviewer->show();
        // call this after show because otherwise o valid gl context!!!
        vviewer->addVolume(name, volume);
        //vviewer->setVolumeName(name);   // this will also update the window title
        //vviewer->setVolume(volume);
    }
}

void ProjectViewer::displayVolumeSlice( const QString& name){

    if( m_project->volumeList().contains(name)){

        std::shared_ptr<Grid3D<float> > volume=m_project->loadVolume( name);
        if( !volume ) return;

        Grid3DBounds bounds=volume->bounds();
        int ft=static_cast<int>(1000*bounds.ft());  // convert to msecs
        int lt=static_cast<int>(1000*bounds.lt());  // convert to msecs
        int dt=static_cast<int>(1000*bounds.dt());  // convert to msecs

        VolumeSliceSelector* sliceSelector=new  VolumeSliceSelector(this);

        sliceSelector->setVolumeName(name);

        sliceSelector->setTimeRange(std::pair<int,int>( ft, lt));
        sliceSelector->setTimeStep(dt);
        sliceSelector->setTime(ft);

        GridViewer* viewer=new GridViewer();
        GridView* gridView=viewer->gridView();
        gridView->setInlineOrientation(m_project->inlineOrientation());
        gridView->setInlineDirection(m_project->inlineDirection());
        gridView->setCrosslineDirection(m_project->crosslineDirection());

        viewer->setDispatcher(m_dispatcher);

        viewer->setTimeSource(GridViewer::TimeSource::TIME_FIXED);

        connect( sliceSelector, SIGNAL(gridChanged(std::shared_ptr<Grid2D<float>>)),
                 viewer, SLOT(setGrid(std::shared_ptr<Grid2D<float>>)));
        connect( sliceSelector, SIGNAL(descriptionChanged(QString)), viewer, SLOT(setWindowTitle(QString)) );

        connect( viewer, SIGNAL(requestSlice(int)), sliceSelector, SLOT(setTime(int)) );
        connect( sliceSelector, SIGNAL(timeChanged(int)), viewer, SLOT(setFixedTime(int)) );

        viewer->navigationToolBar()->addWidget(sliceSelector);
        viewer->show();
        viewer->setProject( m_project );  // grant access to grid

        sliceSelector->setVolume(volume);
    }
}


void ProjectViewer::displayVolumeRelativeSlice( const QString& name){

    if( m_project->volumeList().contains(name)){

        std::shared_ptr<Grid3D<float> > volume=m_project->loadVolume( name);
        if( !volume ) return;

        Grid3DBounds bounds=volume->bounds();
        int lt=static_cast<int>(1000*bounds.lt());  // convert to msecs
        int dt=static_cast<int>(1000*bounds.dt());  // convert to msecs

        VolumeRelativeSliceSelector* sliceSelector=new  VolumeRelativeSliceSelector(this);

        sliceSelector->setVolumeName(name);

        sliceSelector->setTimeRange(std::pair<int,int>( -lt, lt));  // best guess for all horizons
        sliceSelector->setTimeStep(dt);
        sliceSelector->setTime(0);                                  // start at horizon

        GridViewer* viewer=new GridViewer();
        GridView* gridView=viewer->gridView();
        gridView->setInlineOrientation(m_project->inlineOrientation());
        gridView->setInlineDirection(m_project->inlineDirection());
        gridView->setCrosslineDirection(m_project->crosslineDirection());

        viewer->setTimeSource(GridViewer::TimeSource::TIME_FUNCTION);
        viewer->setTimeFunction(std::bind(&VolumeRelativeSliceSelector::timeAt, sliceSelector, _1, _2));


        viewer->setDispatcher(m_dispatcher);


        connect( sliceSelector, SIGNAL(gridChanged(std::shared_ptr<Grid2D<float>>)),
                 viewer, SLOT(setGrid(std::shared_ptr<Grid2D<float>>)));
        connect( sliceSelector, SIGNAL(descriptionChanged(QString)), viewer, SLOT(setWindowTitle(QString)) );

        connect( viewer, SIGNAL(requestSlice(int)), sliceSelector, SLOT(setTime(int)) );
        connect( sliceSelector, SIGNAL(timeChanged(int)), viewer, SLOT(setFixedTime(int)) );

        viewer->navigationToolBar()->addWidget(sliceSelector);
        viewer->show();
        viewer->setProject( m_project );  // grant access to grid

        sliceSelector->setVolume(volume);

        sliceSelector->setProject(m_project);
    }

}



void ProjectViewer::displayVolumeHistogram( const QString& name){

    if( m_project->volumeList().contains(name)){

        std::shared_ptr<Grid3D<float> > volume=m_project->loadVolume( name);
        if( !volume ) return;

        QVector<double> data;
        for( auto it=volume->values().cbegin(); it!=volume->values().cend(); ++it){
            if( *it==volume->NULL_VALUE) continue;
            data.push_back(*it);
         }

        HistogramDialog* viewer=new HistogramDialog; //don't make this parent because projectviewer should be able to be displayed over gridviewer
        viewer->setData( data );
        viewer->setWindowTitle(QString("Histogram of Volume %1").arg(name) );

        viewer->show();

    }
}

// XXX make this a process later
void ProjectViewer::exportVolume( QString volumeName){

    ExportVolumeDialog dlg(this);
    dlg.setWindowTitle("Export Volume");
    dlg.setFixedVolume(volumeName);

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess( new ExportVolumeProcess( m_project, this ), params );

}


// XXX make this a process later
void ProjectViewer::exportVolumeSeisware( QString volumeName){

    const int SCALCO=-10;

    QTransform IlXlToXY;
    QTransform XYToIlXl;
    if( !m_project->geometry().computeTransforms(XYToIlXl, IlXlToXY)){
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

        //updateProjectViews();
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

         //updateProjectViews();
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

        viewer->setShareCurrentPoint( info.mode()==SeismicDatasetInfo::Mode::Poststack  );
        viewer->view()->setPrimarySortKey(selector->primarySort()); // need to kep in sync from start on
        connect( selector, SIGNAL(primarySortChanged(GatherSortKey)), viewer->view(), SLOT(setPrimarySortKey(GatherSortKey)) );
        connect( selector, SIGNAL(gatherChanged(std::shared_ptr<seismic::Gather>)), viewer, SLOT(setGather(std::shared_ptr<seismic::Gather>)));
        connect( selector, SIGNAL(descriptionChanged(QString)), viewer, SLOT(setWindowTitle(QString)) );

        connect( viewer, SIGNAL(requestPoint(int,int)), selector,SLOT(providePoint(int,int)));
        connect( viewer, SIGNAL(requestPoints(QVector<QPoint>)), selector, SLOT( provideRandomLine(QVector<QPoint>)) );
        connect( viewer, SIGNAL(requestPerpendicularLine(int,int)), selector, SLOT(providePerpendicularLine(int,int)) );
        viewer->navigationToolBar()->addWidget(selector);
        //viewer->view()->leftRuler()->setAxxisLabel(verticalAxisLabel);
        viewer->show();
        viewer->setMinimumWidth(viewer->navigationToolBar()->width() + 100 );

        viewer->setProject( m_project );  // grant access to grids

        selector->apply();      // trigger reading of first gather
        viewer->zoomFitWindow();
        viewer->setTraceHeaderDef(reader->segyReader()->info().traceHeaderDef());

    }

}


void ProjectViewer::displaySeismicDatasetSlice(const QString& name){

    if( m_project->seismicDatasetList().contains(name)){

        SeismicDatasetInfo info=m_project->getSeismicDatasetInfo(name);

        if( info.mode()!=SeismicDatasetInfo::Mode::Poststack){
            return;
        }

        std::shared_ptr<SeismicDatasetReader> reader=m_project->openSeismicDataset(name);
        if( !reader){
            QMessageBox::critical(this, "Display Seismic Dataset Slice", "Open reader failed!");
            return;
        }

        // get sampling information from first trace
        std::pair<int, int> ftIlXl=reader->firstTraceInlineCrossline();
        std::shared_ptr<seismic::Trace> firstTrace=reader->readFirstTrace("iline", QString::number(ftIlXl.first),
                                                                          "xline", QString::number(ftIlXl.second) );
        if( !firstTrace ){
            QMessageBox::critical(this, tr("Display Seismic Slice"), tr("Reading first trace failed!"), "Ok" );
            return;
        }
        int ft=static_cast<int>(1000*firstTrace->ft());  // convert to msecs
        //int dt=static_cast<int>(1000*firstTrace->dt()); // convert to msecs
        int lt=static_cast<int>(1000*firstTrace->lt());  // convert to msecs


        SeismicSliceSelector* sliceSelector=new  SeismicSliceSelector(this);
        sliceSelector->setReader(reader);
        sliceSelector->setTimeRange(std::pair<int,int>( ft, lt));
        sliceSelector->setTime(0);

        GridViewer* viewer=new GridViewer();
        GridView* gridView=viewer->gridView();
        gridView->setInlineOrientation(m_project->inlineOrientation());
        gridView->setInlineDirection(m_project->inlineDirection());
        gridView->setCrosslineDirection(m_project->crosslineDirection());

        viewer->setDispatcher(m_dispatcher);
        viewer->setWindowTitle( tr("Time Slice") );

        viewer->setTimeSource(GridViewer::TimeSource::TIME_FIXED);

        connect( sliceSelector, SIGNAL(sliceChanged(std::shared_ptr<Grid2D<float>>)),
                 viewer, SLOT(setGrid(std::shared_ptr<Grid2D<float>>)));
        connect( sliceSelector, SIGNAL(descriptionChanged(QString)), viewer, SLOT(setWindowTitle(QString)) );

        connect( viewer, SIGNAL(requestSlice(int)), sliceSelector, SLOT(setTime(int)) );
        connect( sliceSelector, SIGNAL(timeChanged(int)), viewer, SLOT(setFixedTime(int)) );

        viewer->navigationToolBar()->addWidget(sliceSelector);

        viewer->show();
        viewer->setProject( m_project );  // grant access to grid

        sliceSelector->apply();
    }

}


void ProjectViewer::displaySeismicDatasetIndex(const QString& name){

    if( m_project->seismicDatasetList().contains(name)){

        SeismicDatasetInfo info=m_project->getSeismicDatasetInfo(name);
        info.makeAbsolute(m_project->seismicDirectory());
        QString path=info.indexPath(); //m_project->getSeismicDatasetIndexPath(name);

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
                //updateProjectViews();
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

        //updateProjectViews();
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

         //updateProjectViews();
    }
}


void ProjectViewer::setProjectFileName( const QString& fileName){

    m_projectFileName=fileName;

    setWindowTitle(QString("AVOProject - \"%1\"").arg(m_projectFileName));
}


bool ProjectViewer::openProject(const QString &fileName){

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Open Project"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }


    AVOProject* tmp = new AVOProject(this);
    XPRReader reader(tmp, fileName);
    if (!reader.read(&file)) {
        QMessageBox::warning(this, tr("Open Project"),
                             tr("Parse error in file %1:\n\n%2")
                             .arg(fileName)
                             .arg(reader.errorString()));
        delete tmp;
        return false;

    }



    // check if project already in use, create lockfile if not
    // no need to remove stale lockfiles. This is handled by QLockfile
    std::shared_ptr<QLockFile> lf(new QLockFile(tmp->lockfileName()));
    if( !lf->tryLock()){
        QMessageBox::critical(this, tr("Open Project"),
                              tr("Project already used by another program"));
        return false;
    }

    m_lockfile=lf;  // will be unlocked when deleted

    ui->actionCloseProject->trigger();
    m_project=tmp;
    connect( m_project, SIGNAL(changed()), this, SLOT(updateProjectViews()) );

    setProjectFileName(fileName);
    updateProjectViews();
    updateMenu();

    adjustForCurrentProject(fileName);

    return true;
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

    XPRWriter writer(m_project);
    if( !writer.writeFile(&file) ){
        QMessageBox::warning(this, tr("Save Project"),
                             tr("Saving project failed!"));
        return false;
    }

    return true;
}

void ProjectViewer::updateProjectViews(){

    if( !m_project ){
        ui->datasetsView->setModel(0);
        ui->horizonsView->setModel(0);
        ui->attributesView->setModel(0);
        ui->gridsView->setModel(0);
        ui->volumesView->setModel(0);
        return;
    }

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

    Q_ASSERT( process );

    try{

    QDateTime start=QDateTime::currentDateTime();

    QString name=process->name();

    statusBar()->showMessage(QString("Started %1 at %2").arg(name).arg(start.time().toString() ) );

    if( process->init(params)!=ProjectProcess::ResultCode::Ok ){

        QMessageBox::critical(this, process->name(), process->lastError());
        delete process;

        statusBar()->showMessage( QString("Canceled %1 at %2").arg(name).arg(QDateTime::currentDateTime().time().toString()) );
        return;
    }

    QProgressDialog* progress=new QProgressDialog(this);        // add destroy on close??
    progress->setWindowTitle(name);
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


    delete progress;
    delete process;


    if( code != ProjectProcess::ResultCode::Ok){
        statusBar()->showMessage( QString("Canceled %1 at %2").arg(name).arg(QDateTime::currentDateTime().time().toString()) );
        return;
    }

    //updateProjectViews();

    QDateTime end=QDateTime::currentDateTime();
    qreal duration=0.001*start.msecsTo(end);
    statusBar()->showMessage(QString("Finished %1 at %2 after %3 seconds").arg(name).arg(end.time().toString()).arg(duration) );

    if( params.contains(QString("display-grid"))){
        for( QString gridName : params.values(QString("display-grid"))){
            auto gridTypeAndName=splitFullGridName(gridName);
            displayGrid( gridTypeAndName.first, gridTypeAndName.second);
        }
    }

    }catch( std::exception& ex ){
        QMessageBox::critical(this, tr("Unhandled Exception Running Process"), ex.what() );
    }

}


void ProjectViewer::saveSettings(){

     QSettings settings(COMPANY, "ProjectViewer");

     settings.beginGroup("ProjectViewer");

       settings.setValue("size", size());
       settings.setValue("position", pos() );

       settings.setValue("recent-files", recentProjectFileList);

     settings.endGroup();

     settings.sync();
}


void ProjectViewer::loadSettings(){

    QSettings settings(COMPANY, "ProjectViewer");

    settings.beginGroup("ProjectViewer");

        resize(settings.value("size", QSize(400, 600)).toSize());
        move(settings.value("pos", QPoint(200, 200)).toPoint());

        recentProjectFileList = settings.value("recent-files").toStringList();

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
    ui->actionExportProject->setEnabled(isProject);
    ui->actionOpenGrid->setEnabled( isProject);
    ui->actionOpenSeismicDataset->setEnabled( isProject);
    ui->actionCloseProject->setEnabled(isProject);

    ui->action_Geometry->setEnabled(isProject);
    ui->actionAxis_Orientation->setEnabled(isProject);
    ui->actionDisplay_Map->setEnabled(isProject);

    ui->actionCreateTimeslice->setEnabled(isProject);
    ui->actionCrop_Grid->setEnabled(isProject);
    ui->actionHorizon_Amplitudes->setEnabled(isProject);
    ui->actionHorizon_Semblance->setEnabled(isProject);
    ui->actionCompute_Intercept_Gradient->setEnabled(isProject);
    ui->actionTrend_Based_Attribute_Grids->setEnabled(isProject);
    ui->actionSecondary_Attribute_Grids->setEnabled(isProject);
    ui->actionConvert_Grid->setEnabled(isProject);
    ui->actionSmooth_Grid->setEnabled(isProject);
    ui->actionRun_Grid_User_Script->setEnabled(isProject);

    ui->action_Crop_Volume->setEnabled(isProject);
    ui->actionVolume_Amplitudes->setEnabled(isProject);
    ui->actionVolume_Semblance->setEnabled(isProject);
    ui->actionCompute_Intercept_and_Gradient_Volumes->setEnabled(isProject);
    //ui->actionFluid_Factor_Volume->setEnabled(isProject);
    ui->actionTrend_Based_Attribute_Volumes->setEnabled(isProject);
    ui->actionSecondary_Attribute_Volumes->setEnabled(isProject);
    ui->actionRun_Volume_Script->setEnabled(isProject);

    ui->actionCrossplot_Grids->setEnabled(isProject);
    ui->actionCrossplot_Volumes->setEnabled(isProject);
    ui->actionAmplitude_vs_Offset_Plot->setEnabled(isProject);
    ui->action_3D_Viewer->setEnabled(isProject);

    ui->actionVolume_Curvature->setEnabled(isProject);
}




