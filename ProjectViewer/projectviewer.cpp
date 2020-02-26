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
#include <multiitemselectiondialog.h>
#include <multiinputdialog.h>
#include <xyzimportdialog.h>
#include <sliceviewer.h>
#include <volumepropertydialog.h>
#include <logpropertiesdialog.h>
#include<QFileDialog>
#include<QDir>
#include<QMessageBox>
#include<QInputDialog>
#include<QStandardItem>
#include<QStandardItemModel>
#include<QProgressDialog>
#include<QSettings>
#include<QPointF>
    //void invalidateBuffers();
#include<QDateTime>
#include<xgrwriter.h>
#include<grid.h>
#include<log.h>
#include<editwelldialog.h>
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
#include<lasimportdialog.h>
#include<iostream>
#include<chrono>
#include<random>
#include<limits>
#include<memory>
#include<regex>
#include<functional>
using namespace std::placeholders; // for _1, _2 etc.

#include<projectgeometrydialog.h>
#include<orientationdialog.h>
#include<importgridprocess.h>
#include<importgriddialog.h>
#include<exportgridprocess.h>
#include<exportgriddialog.h>
#include<exportvolumeprocess.h>
#include<exportvolumedialog.h>
#include<exportvolumerefprocess.h>
#include<exportvolumerefdialog.h>
#include<exportseismicprocess.h>
#include<exportseismicdialog.h>
#include<exportlogprocess.h>
#include<exportlogdialog.h>
#include<exportlasdialog.h>
#include<exportlasprocess.h>
#include <offsetstackdialog.h>
#include <offsetstackprocess.h>
#include <offsetcomputationdialog.h>
#include <offsetcomputationprocess.h>
#include <stacktogatherdialog.h>
#include <stacktogatherprocess.h>
#include <replacebadtracesdialog.h>
#include <replacebadtracesprocess.h>
#include <cropdatasetdialog.h>
#include <cropdatasetprocess.h>
#include <createtimesliceprocess.h>
#include <createtimeslicedialog.h>
#include <creategriddialog.h>
#include <creategridprocess.h>
#include <cropgriddialog.h>
#include <cropgridprocess.h>
#include <horizonamplitudesprocess.h>
#include <horizonamplitudesdialog.h>
#include <horizonfrequenciesprocess.h>
#include <horizonfrequenciesdialog.h>
#include <horizonsemblanceprocess.h>
#include <horizonsemblancedialog.h>
#include <horizoncurvatureprocess.h>
#include <curvaturedialog.h>
#include <instantaneousattributesdialog.h>
#include <instantaneousattributesprocess.h>
#include <semblancevolumeprocess.h>
#include <semblancevolumedialog.h>
#include <volumesimilaritydialog.h>
#include <volumesimilarityprocess.h>
#include <variancevolumeprocess.h>
// variance volume uses semblancevolumedialog
#include <volumestatisticsprocess.h>
#include <volumestatisticsdialog.h>
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
#include <trendbasedattributegridsdialog.h>
#include <trendbasedattributevolumesdialog.h>
#include <secondaryattributesprocess.h>
#include <secondaryavoattributesdialog.h>
#include <createvolumedialog.h>
#include <createvolumeprocess.h>
#include <cropvolumedialog.h>
#include <cropvolumeprocess.h>
#include <mergevolumesdialog.h>
#include <mergevolumesprocess.h>
#include <punchoutvolumedialog.h>
#include <punchoutvolumeprocess.h>
#include <volumemathdialog.h>
#include <volumemathprocess.h>
#include <flattenvolumedialog.h>
#include <flattenvolumeprocess.h>
#include <velocityconversiondialog.h>
#include <velocityconversionprocess.h>
#include <volumetdconversiondialog.h>
#include <volumetdconversionprocess.h>
#include <extracttimesliceprocess.h>
#include <extractdatasetdialog.h>
#include <extractdatasetprocess.h>
#include <amplitudevolumedialog.h>
#include <amplitudevolumeprocess.h>
#include <frequencyvolumedialog.h>
#include <frequencyvolumeprocess.h>
#include <dipscandialog.h>
#include <volumedipscanprocess.h>
#include <curvaturevolumedialog.h>
#include <curvaturevolumeprocess.h>
#include <convertgriddialog.h>
#include <convertgridprocess.h>
#include <smoothgriddialog.h>
#include <smoothgridprocess.h>
#include <gridmathdialog.h>
#include <gridmathprocess.h>
#include <gridrunuserscriptdialog.h>
#include <rungridscriptprocess.h>
#include <nnvolumetrainer.h>
#include <nnvolumeclassificationdialog.h>
#include <nnvolumeclassificationprocess.h>
#include <runvolumescriptdialog.h>
#include <runvolumescriptprocess.h>
#include <windowreductionfunction.h>
#include <buildwellvolumedialog.h>
#include <buildwellvolumeprocess.h>
#include <horizontotopdialog.h>
#include <horizontotopprocess.h>
#include <smoothlogdialog.h>
#include <smoothlogprocess.h>
#include <logmathdialog.h>
#include <logmathprocess.h>
#include <logintegrationdialog.h>
#include <logintegrationprocess.h>
#include <copylogdialog.h>
#include <copylogprocess.h>
#include <nnlogdialog.h>
#include <nnlogprocess.h>
#include <nnlogtrainer.h>
#include <nnloginterpolationdialog.h>
#include <nnloginterpolationprocess.h>
#include <logscriptdialog.h>
#include <logscriptprocess.h>
#include <vshaledialog.h>
#include <vshaleprocess.h>
#include <castagnadialog.h>
#include <castagnaprocess.h>
#include <rockpropertiesdialog.h>
#include <rockpropertiesprocess.h>
#include <tabletohorizondialog.h>
#include <tabletohorizonprocess.h>
#include <importmarkersdialog.h>
#include <importmarkersprocess.h>
#include <importtabledialog.h>
#include <importtableprocess.h>
#include <editmarkersdialog.h>
#include <fmcdp2ddialog.h>
#include <fmcdp2dprocess.h>
#include <exporttabledialog.h>
#include <exporttableprocess.h>
#include <volumeiotestdialog.h>
#include <volumeiotestprocess.h>
#include <interceptgradientvolumeprocess_2.h>
#include<interceptgradientvolumedialog_2.h>
#include<axxisorientation.h>
#include<projectgeometry.h>
#include "selectgridtypeandnamedialog.h"
#include "crossplotgridsinputdialog.h"
#include "crossplotvolumesinputdialog.h"
#include "crossplotviewer.h"
#include "amplitudecurveviewer.h"
#include "spectrumviewer.h"
#include <logviewer.h>
#include <logeditor.h>
#include <logeditdialog.h>
#include <logspectrumviewer.h>
#include <crossplot.h>
#include <gridcolorcompositeinputdialog.h>
#include <colorcompositeviewer.h>
#include <scatterplotviewer.h>
#include <reversespinbox.h>
#include <logsitemdelegate.h>
#include <selectwellsbylogdialog.h>
#include<mapviewer2.h>
#include <volumeviewer.h>
#include <processparamsviewer.h>
#include<ebcdicdialog.h>
#include<topsdbmanager.h>
#include<table.h>
#include<tabledialog.h>
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
#include<las_reader.h>
#include<multilogviewer.h>

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

    for( auto child : ui->menuBar->children() ){
        if( auto p=dynamic_cast<QMenu*>(child)){
            p->setToolTipsVisible(true);
        }
    }

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
    ui->datasetsView->installEventFilter(this);
    //ui->datasetsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->datasetsView->horizontalHeader()->setStretchLastSection(true);

    ui->horizonsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->horizonsView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runHorizonContextMenu(const QPoint&)));
    ui->horizonsView->installEventFilter(this);

    ui->attributesView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->attributesView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runAttributeContextMenu(const QPoint&)));
    ui->attributesView->installEventFilter(this);

    ui->gridsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->gridsView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runOtherGridContextMenu(const QPoint&)));
    ui->gridsView->installEventFilter(this);

    ui->volumesView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->volumesView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runVolumeContextMenu(const QPoint&)));
    ui->volumesView->installEventFilter(this);

    ui->wellsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->wellsView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runWellContextMenu(const QPoint&)));
    ui->wellsView->installEventFilter(this);
    // ui->wellsView->setItemDelegateForColumn(5, new LogsItemDelegate(this)); // XXX don't use thie because this requires editing enabled

    ui->tablesView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tablesView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runTableContextMenu(const QPoint&)));
    ui->tablesView->installEventFilter(this);


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


ProjectViewer::~ProjectViewer()
{
    delete ui;
}



/*
 * File menu
*/


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
        connectViews(m_project);
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
    segyInputDialog.setPrestack(propertiesDialog.mode()==SeismicDatasetInfo::Mode::Prestack);
    segyInputDialog.set3D(propertiesDialog.dimensions()==3);
    segyInputDialog.setWindowTitle("Import SEG-Y");

    std::cout<<"ppdlg dims="<<propertiesDialog.dimensions()<<" sidlg 3d="<<std::boolalpha<<segyInputDialog.is3D()<<std::endl<<std::flush;

    if( segyInputDialog.exec()!=QDialog::Accepted) return;

    if( !m_project->addSeismicDataset(name, propertiesDialog.dimensions(), propertiesDialog.domain(), propertiesDialog.mode(),
                                      segyInputDialog.path(), segyInputDialog.info())){
        QMessageBox::critical(this, "Import Dataset", "Adding seismic dataset to project failed!");
        return;
    }

    //updateProjectViews();
}


void ProjectViewer::on_actionImport_Attached_Datasets_triggered()
{
    QStringList avail;
    for( auto name : m_project->seismicDatasetList()){
        if( m_project->isAttachedSeismicDataset(name)) avail<<name;
    }

    bool ok=false;
    QStringList names=MultiItemSelectionDialog::getItems(this, "Import Attached Datasets", "Select Datasets:", avail, &ok);
    if( !ok || names.empty()) return;

    int i=0;
    QProgressDialog progress(this);
    progress.show();
    progress.setMinimumDuration(0);
    progress.setWindowTitle("Import Attached Datasets");
    progress.setRange(0,names.size());
    qApp->processEvents();

    for( auto name: names){
        progress.setLabelText(QString("%1 ( %2 of %3)").arg(name, QString::number(i+1). QString::number(names.size())));
        progress.setValue(i);
        qApp->processEvents();
        if( progress.wasCanceled()) break;

        m_project->importAttachedSeismicDataset(name);
    }
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
    std::shared_ptr<Volume> volume(new Volume(bounds));

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
    // make this a process!!!! For now dummy params
    ProcessParams pp;
    pp.insert(tr("__process"), tr("Import Seismic") );
    pp.insert(tr("__started"), QDateTime::currentDateTime().toString());
    pp.insert(tr("segy-file"), path);
    m_project->addVolume(name, volume, pp );
    //updateProjectViews();
}

void ProjectViewer::on_actionImportLogs_triggered(){

    LASImportDialog::import(m_project);
}



void ProjectViewer::on_action_Logs_Bulk_Mode_triggered()
{
    LASImportDialog::bulkImport(m_project);
}


void ProjectViewer::on_actionWellpath_triggered()
{
    bool ok=false;
    QString name=QInputDialog::getItem(this, tr("Import Wellpath"), tr("Select Well:"), m_project->wellList(), 0, false, &ok);
    if( name.isEmpty() || !ok) return;

    importWellPath(name);
}


void ProjectViewer::on_actionWellpaths_Bulk_Mode_triggered()
{
    bool ok=false;
    QStringList wells=MultiItemSelectionDialog::getItems(this, tr("Import Wellpaths(bulk)"), tr("Select Wells:"), m_project->wellList(), &ok );
    if( !ok | wells.empty()) return;

    QString name=QFileDialog::getOpenFileName(this, tr("Import Wellpaths(bulk)"), QDir::homePath());

    if( name.isEmpty()) return;

    QVector<std::pair<QString,QString>> uwi_path;
    std::ifstream is(name.toStdString());
    std::string line, uwi, path;
    while(getline(is, line) ){

        std::regex re("^([0-9]+)[ ]+(.*)$");
        std::smatch matched;
        if (std::regex_match(line, matched, re)) {
            if( matched.size()!=1 + 2 ) continue;
            uwi=matched[1].str();
            path=matched[2].str();

            if( wells.contains(uwi.c_str())){

                uwi_path.push_back(std::make_pair(QString(uwi.c_str()), QString(path.c_str())));
            }

        }
        else continue;
    }

    importWellPathBulk(uwi_path);
}


void ProjectViewer::on_actionWell_Markers_triggered()
{
    /*
    QString filename=QFileDialog::getOpenFileName(this, tr("Import Well Markers"), QDir::homePath());
    std::ifstream is(filename.toStdString().c_str());

    QMap<QString, std::shared_ptr<WellMarkers>> wms;

    std::string line;
    while(getline(is, line) ){
        QString s=QString(line.c_str()).trimmed();
        auto cols=s.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if( s.size() <3 ) continue;
        QString uwi=cols[0];
        QString name=cols[1];
        qreal md=cols[2].toDouble();
        //std::cout<<uwi.toStdString()<<" "<<name.toStdString()<<" "<<md<<std::endl<<std::flush;
        if( !wms.contains(uwi)){
            auto wm=m_project->loadWellMarkers(uwi);
            if(!wm) wm=std::make_shared<WellMarkers>();

            wms.insert(uwi, wm);
        }

        auto wm=wms.value(uwi);
        wm->push_back(WellMarker(name, md));
    }

    for( auto well : wms.keys()){
        auto wm=wms.value(well);
        if( !wm ) continue;
        std::cout<<"saving well markers: "<<well.toStdString()<<" count="<<wm->size()<<std::endl;
        m_project->saveWellMarkers(well, *wm);
    }
    */

    Q_ASSERT( m_project );

    ImportMarkersDialog dlg;
    dlg.setWindowTitle("Import Well Markers");
    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    //runProcess( (std::make_shared<ImportMarkersProcess>( m_project, this )).get(), params );
    runProcess<ImportMarkersProcess>(params);
}


void ProjectViewer::on_actionImportTable_triggered()
{
    Q_ASSERT( m_project );

    ImportTableDialog dlg;
    dlg.setWindowTitle("Import Table");
    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<ImportTableProcess>(params);
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


void ProjectViewer::on_actionExportVolume_triggered()
{
    selectAndExportVolume();
}


void ProjectViewer::on_actionExportSeismic_triggered()
{
    selectAndExportSeismicDataset();
}

void ProjectViewer::on_actionExportLog_triggered()
{
    selectAndExportLog();
}

void ProjectViewer::on_actionExport_Logs_triggered()
{
    ExportLASDialog dlg;

    dlg.setProject(m_project);
    dlg.setWindowTitle(tr("Export Logs(LAS)"));

    if( dlg.exec()!=QDialog::Accepted) return;

    auto params=dlg.params();
    runProcess<ExportLASProcess>( params );
}

void ProjectViewer::on_action_ExportTable_triggered()
{
    selectAndExportTable();
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




/*
 * Project Menu
*/

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
    MapViewer2* viewer=new MapViewer2();
    viewer->setWindowTitle(QString("Project Map - %1").arg( m_projectFileName ) );
    viewer->setProject(m_project);
    viewer->setShowProjectArea(true);
    viewer->show();
}


void ProjectViewer::on_actionEdit_Tops_triggered()
{
    if( !m_project) return;

    try{
    auto tmgr = m_project->openTopsDatabase();
    if( !tmgr){
        QMessageBox::critical(this, "Edit Tops", "Open Database failed!", QMessageBox::Ok);
        return;
    }

    auto mks = tmgr->markers();

    EditMarkersDialog dlg;
    dlg.setWindowTitle(QString("Edit Tops"));
    dlg.setMarkers(mks);
    if( dlg.exec()!=QDialog::Accepted) return;

    mks=dlg.markers();
    tmgr->replaceAll(mks.begin(), mks.end());
    }
    catch(std::exception& ex){
        std::cerr<<"Exception: "<<ex.what()<<std::endl<<std::flush;
    }
}


/*
 * Process Seismic Menu
*/


void ProjectViewer::on_actionCrop_Dataset_triggered()
{
    Q_ASSERT( m_project );

    CropDatasetDialog dlg;
    dlg.setProject(m_project);
    dlg.setInputDatasets( m_project->seismicDatasetList());
    dlg.setReservedDatasets( m_project->seismicDatasetList() );

    ProjectGeometry geom=m_project->geometry();
    QPoint min=geom.linesRange().first;
    QPoint max=geom.linesRange().second;
    if( min!=max){      // geometry assigned
        dlg.setInlineRange( min.x(), max.x() );
        dlg.setCrosslineRange( min.y(), max.y() );
    }
    else{               // geometry not set
        dlg.setInlineRange( 0, (std::numeric_limits<int>::max)() );
        dlg.setCrosslineRange( 0, (std::numeric_limits<int>::max)() );
    }

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<CropDatasetProcess>( params );
}


void ProjectViewer::on_actionCreateTimeslice_triggered()
{
    Q_ASSERT( m_project );

    CreateTimesliceDialog dlg;
    dlg.setInputs( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Poststack));
    dlg.setInputLabel(tr("Dataset:"));
    dlg.setHorizons( m_project->gridList( GridType::Horizon));
    dlg.setReservedGrids(m_project->gridList(GridType::Other));
    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<CreateTimesliceProcess>( params );
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

    runProcess<HorizonAmplitudesProcess>( params );
}


void ProjectViewer::on_actionHorizon_Frequencies_triggered()
{
    Q_ASSERT( m_project );

    HorizonFrequenciesDialog dlg;
    dlg.setDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Poststack));
    dlg.setHorizons( m_project->gridList(GridType::Horizon));
    dlg.setReservedGrids(m_project->gridList(GridType::Other));
    QStringList methods;
    methods<<"Maximum";
    /*
    for( auto method : ReductionMethodsAndNames.keys() ){
        methods.append( ReductionMethodsAndNames.value(method));
    }*/

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<HorizonFrequenciesProcess>( params );
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

    runProcess<HorizonSemblanceProcess>( params );

}


void ProjectViewer::on_actionVolume_Amplitudes_triggered()
{
    AmplitudeVolumeDialog dlg;
    dlg.setWindowTitle(tr("Volume Amplitudes"));
    dlg.setProject(m_project);
    auto geom=m_project->geometry().bboxLines();
    dlg.setMinIline(geom.left());
    dlg.setMaxIline(geom.right());
    dlg.setMinXline(geom.top());
    dlg.setMaxXline(geom.bottom());

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<AmplitudeVolumeProcess>( params );
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

    runProcess<ComputeInterceptGradientProcess>(params );

}


void ProjectViewer::on_actionCompute_Intercept_and_Gradient_Volumes_triggered()
{
    InterceptGradientVolumeDialog dlg;
    dlg.setDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Prestack));
    dlg.setReservedVolumes(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<InterceptGradientVolumeProcess>( params );
}


void ProjectViewer::on_action_Offset_Stack_triggered()
{
    Q_ASSERT( m_project );

    OffsetStackDialog dlg;
    dlg.setInputDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Prestack));
    dlg.setReservedDatasets( m_project->seismicDatasetList() );
    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess< OffsetStackProcess>( params );
}

void ProjectViewer::on_actionOffset_Computation_triggered()
{
    Q_ASSERT( m_project );

    OffsetComputationDialog dlg;
    dlg.setInputDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Prestack));
    dlg.setReservedDatasets( m_project->seismicDatasetList() );
    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess< OffsetComputationProcess>( params );
}


void ProjectViewer::on_actionStack_To_Gather_triggered()
{
    Q_ASSERT( m_project);

    StackToGatherDialog dlg;
    dlg.setWindowTitle("Stack(s) to Gather");
    dlg.setInputStacks(m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Poststack));
    dlg.setReservedDatasets( m_project->seismicDatasetList() );
    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<StackToGatherProcess>( params );
}


/*
 * Process Grid menu
*/


void ProjectViewer::on_actionCreate_New_Grid_triggered()
{
    Q_ASSERT( m_project );

    CreateGridDialog dlg;
    dlg.setWindowTitle(tr("Create New Grid"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<CreateGridProcess>( params );
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

    runProcess<ConvertGridProcess>( params );
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

    runProcess<CropGridProcess>( params );
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

    runProcess<SmoothGridProcess>( params );
}


void ProjectViewer::on_actionGrid_Math_triggered()
{
    Q_ASSERT( m_project );

    GridMathDialog dlg;
    dlg.setWindowTitle(tr("Grid Math"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<GridMathProcess>( params );
}


void ProjectViewer::on_actionHorizon_Curvature_triggered()
{
    Q_ASSERT( m_project );

    CurvatureDialog dlg;
    dlg.setInputs( m_project->gridList(GridType::Horizon));
    //dlg.setReservedGrids(m_project->gridList(GridType::Other));
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<HorizonCurvatureProcess>( params );

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

    runProcess<FluidFactorProcess>( params );
}


void ProjectViewer::on_actionTrend_Based_Attribute_Grids_triggered()
{
    Q_ASSERT(m_project);

    TrendBasedAttributeGridsDialog dlg;
    dlg.setWindowTitle(tr("Trend Based Attribute Grids"));
    dlg.setInterceptList( m_project->gridList(GridType::Attribute));
    dlg.setGradientList( m_project->gridList(GridType::Attribute));
    dlg.setReservedGrids(m_project->gridList(GridType::Attribute));

    dlg.setComputeTrend(true);
    dlg.setTrendIntercept(0);
    dlg.setTrendAngle(0);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<TrendBasedAttributesProcess>( params );
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

    runProcess<SecondaryAttributesProcess>( params );

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

     runProcess<RunGridScriptProcess>( params );
}



/*
 * Process volume menu
*/

void ProjectViewer::on_actionCreate_New_Volume_triggered()
{
    CreateVolumeDialog dlg;
    dlg.setWindowTitle(tr("Create New Volume"));
    dlg.setProject(m_project);
    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<CreateVolumeProcess>(  params );
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
    if( min!=max){      // geometry assigned
        dlg.setInlineRange( min.x(), max.x() );
        dlg.setCrosslineRange( min.y(), max.y() );
    }
    else{               // geometry not set
        dlg.setInlineRange( 0, (std::numeric_limits<int>::max)() );
        dlg.setCrosslineRange( 0, (std::numeric_limits<int>::max)() );
    }

    dlg.setTimeRange( 0, 99999 );

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<CropVolumeProcess>( params );
}

void ProjectViewer::on_actionMerge_Volumes_triggered()
{
    Q_ASSERT( m_project );

    MergeVolumesDialog dlg;
    dlg.setWindowTitle(tr("Merge Volumes"));
    dlg.setInputVolumes(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<MergeVolumesProcess>( params );
}

void ProjectViewer::on_actionPunch_Out_Volume_triggered()
{
    Q_ASSERT( m_project );

    PunchOutVolumeDialog dlg;
    dlg.setWindowTitle(tr("Punch Out Volume"));
    dlg.setProject(m_project);
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<PunchOutVolumeProcess>( params );
}

void ProjectViewer::on_actionVolume_Math_triggered()
{
    Q_ASSERT( m_project );

    VolumeMathDialog dlg;
    dlg.setWindowTitle(tr("Volume Math"));
    dlg.setProject(m_project);
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<VolumeMathProcess>( params );
}


void ProjectViewer::on_actionFlatten_Volume_triggered()
{
    Q_ASSERT( m_project );

    FlattenVolumeDialog dlg;
    dlg.setInputs(m_project->volumeList());
    dlg.setHorizons(m_project->gridList(GridType::Horizon));

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<FlattenVolumeProcess>( params );
}


void ProjectViewer::on_actionVelocity_Conversion_triggered()
{
    Q_ASSERT( m_project );

    VelocityConversionDialog dlg;
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();
    runProcess<VelocityConversionProcess>( params );
}

void ProjectViewer::on_actionVolume_Time_Depth_Conversion_triggered()
{
    Q_ASSERT( m_project );

    VolumeTDConversionDialog dlg;
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();
    runProcess<VolumeTDConversionProcess>( params );
}


void ProjectViewer::on_actionExtract_Timeslice_triggered()
{
    Q_ASSERT( m_project );

    CreateTimesliceDialog dlg;
    dlg.setInputs( m_project->volumeList());
    dlg.setInputLabel(tr("Volume:"));
    dlg.setHorizons( m_project->gridList( GridType::Horizon));
    dlg.setReservedGrids(m_project->gridList(GridType::Other));
    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<ExtractTimesliceProcess>( params );
}


void ProjectViewer::on_actionExtract_Dataset_triggered()
{
    Q_ASSERT( m_project );

    ExtractDatasetDialog dlg;

    dlg.setInputVolumes( m_project->volumeList());

    auto geom=m_project->geometry();
    auto min=geom.linesRange().first;
    auto max=geom.linesRange().second;
    dlg.setInlineRange( min.x(), max.x() );
    dlg.setCrosslineRange( min.y(), max.y() );
    dlg.setTimeRange( 0, 99999 );

    dlg.setReservedDatasets( m_project->seismicDatasetList() );

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<ExtractDatasetProcess>( params );
}


void ProjectViewer::on_actionVolume_Dip_triggered()
{
    Q_ASSERT(m_project);

    DipScanDialog dlg;
    dlg.setProject(m_project);
    dlg.setInputs(m_project->volumeList());
    dlg.setWindowTitle("Scan Volume Dips");

    if( dlg.exec()!=QDialog::Accepted) return;

    auto params=dlg.params();

    runProcess<VolumeDipScanProcess>(params );
}

void ProjectViewer::on_actionVolume_Frequencies_triggered()
{

    Q_ASSERT(m_project);
    FrequencyVolumeDialog dlg;
    dlg.setWindowTitle(tr("Volume Frequencies"));
    dlg.setProject(m_project);
    dlg.setInputs( m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();
/*
    QMap<QString,QString> params{
            {"input-volume", "stack2"}, {"output-volume", "dumsynth02"},
            {"window-samples","32"}, {"minimum-frequency", "30."}, {"maximum-frequency","40.2"}
            };
  */
  runProcess<FrequencyVolumeProcess>( params );
}


void ProjectViewer::on_actionVolume_Semblance_triggered()
{
    SemblanceVolumeDialog dlg;
    dlg.setInputVolumes( m_project->volumeList());
    dlg.setReservedVolumes(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<SemblanceVolumeProcess>(params );
}


void ProjectViewer::on_actionVolume_Similarity_triggered()
{
    VolumeSimilarityDialog dlg;
    dlg.setProject( m_project );
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();
    runProcess<VolumeSimilarityProcess>(params );
}

void ProjectViewer::on_actionVolume_Variance_triggered()
{
    SemblanceVolumeDialog dlg;
    dlg.setWindowTitle(tr("Variance Volume"));
    dlg.setInputVolumes( m_project->volumeList());
    dlg.setReservedVolumes(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<VarianceVolumeProcess>( params );
}


void ProjectViewer::on_actionVolume_Statistics_triggered()
{
    VolumeStatisticsDialog dlg;
    dlg.setWindowTitle(tr("Volume Statistics"));
    dlg.setProject(m_project);
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<VolumeStatisticsProcess>( params );
}


void ProjectViewer::on_actionVolume_Curvature_triggered()
{
    CurvatureVolumeDialog dlg;
    dlg.setInputs(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<CurvatureVolumeProcess>( params );
}


void ProjectViewer::on_actionInstantaneous_Attribute_Volumes_triggered()
{
    InstantaneousAttributesDialog dlg;
    dlg.setInputVolumes(m_project->volumeList());
    dlg.setReservedVolumes(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<InstantaneousAttributesProcess>( params );
}

void ProjectViewer::on_actionNN_Volume_Trainer_triggered()
{
    Q_ASSERT( m_project );

    NNVolumeTrainer dlg;
    dlg.setWindowTitle(tr("NN Volume Trainer"));
    dlg.setProject(m_project);

    dlg.exec();
}

void ProjectViewer::on_actionNN_Volume_Classification_triggered()
{
    Q_ASSERT( m_project );

    NNVolumeClassificationDialog dlg;
    dlg.setWindowTitle(tr("NN Volume Classification"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<NNVolumeClassificationProcess>( params );
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

    runProcess<FluidFactorVolumeProcess>( params );
}


void ProjectViewer::on_actionTrend_Based_Attribute_Volumes_triggered()
{
    Q_ASSERT(m_project);

    TrendBasedAttributeVolumesDialog dlg;
    dlg.setWindowTitle(tr("Trend Based Attribute Volumes"));
    dlg.setInterceptList( m_project->volumeList());
    dlg.setGradientList( m_project->volumeList());
    dlg.setReservedGrids(m_project->volumeList());
    dlg.setHorizons( m_project->gridList(GridType::Horizon));
    dlg.setComputeTrend(true);
    dlg.setTrendIntercept(0);
    dlg.setTrendAngle(0);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<TrendBasedAttributeVolumesProcess>( params );
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

    runProcess<RunVolumeScriptProcess>( params );
}



/*
 * Process Well menu
*/


void ProjectViewer::on_action_Smooth_Log_triggered()
{
    Q_ASSERT( m_project );

    SmoothLogDialog dlg;
    dlg.setWindowTitle(tr("Smooth Log"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<SmoothLogProcess>( params );
}


void ProjectViewer::on_actionLog_Math_triggered()
{
    Q_ASSERT( m_project );

    LogMathDialog dlg;
    dlg.setWindowTitle(tr("Log Math"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<LogMathProcess>( params );
}


void ProjectViewer::on_actionLog_Integration_triggered()
{
    Q_ASSERT( m_project );

    LogIntegrationDialog dlg;
    dlg.setWindowTitle(tr("Log Integration"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<LogIntegrationProcess>( params );
}


void ProjectViewer::on_actionCopy_Log_triggered()
{

    Q_ASSERT( m_project );

    CopyLogDialog dlg;
    dlg.setWindowTitle(tr("Copy Log"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<CopyLogProcess>( params );
}

void ProjectViewer::on_actionNN_Log_old_triggered()
{
    Q_ASSERT( m_project );

    NNLogDialog dlg;
    dlg.setWindowTitle(tr("NN Log"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<NNLogProcess>( params );
}

void ProjectViewer::on_actionTrain_NN_Log_triggered()
{
    Q_ASSERT( m_project );

    NNLogTrainer dlg;
    dlg.setWindowTitle(tr("NN Log Trainer"));
    dlg.setProject(m_project);

    dlg.exec();
}

void ProjectViewer::on_actionNN_Log_Interpolation_triggered()
{
    Q_ASSERT( m_project );

    NNLogInterpolationDialog dlg;
    dlg.setWindowTitle(tr("NN Log Interpolation"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<NNLogInterpolationProcess>( params );
}


void ProjectViewer::on_actionVShale_Computation_triggered()
{
    Q_ASSERT( m_project );

    VShaleDialog dlg;
    dlg.setWindowTitle(tr("VShale Computation"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<VShaleProcess>( params );
}

void ProjectViewer::on_actionCastagna_VS_Computation_triggered()
{
    Q_ASSERT( m_project );

    CastagnaDialog dlg;
    dlg.setWindowTitle(tr("Castagna VS Computation"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<CastagnaProcess>( params );
}

void ProjectViewer::on_actionRock_Properties_Computation_triggered()
{
    Q_ASSERT( m_project );

    RockPropertiesDialog dlg;
    dlg.setWindowTitle(tr("Rock Properties Computation"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<RockPropertiesProcess>( params );
}

void ProjectViewer::on_actionRun_Log_Script_triggered()
{
    Q_ASSERT( m_project );

    LogScriptDialog dlg;
    dlg.setWindowTitle(tr("Log Script"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<LogScriptProcess>( params );
}


void ProjectViewer::on_actionBuild_Volume_triggered()
{
    Q_ASSERT( m_project );

    BuildWellVolumeDialog dlg;
    dlg.setWindowTitle(tr("Build Well Volume"));
    dlg.setProject(m_project);
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<BuildWellVolumeProcess>( params );
}


void ProjectViewer::on_actionTops_from_Horizon_triggered()
{
    Q_ASSERT( m_project );

    HorizonToTopDialog dlg;
    dlg.setWindowTitle(tr("Tops from Horizon"));
    dlg.setProject(m_project);
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<HorizonToTopProcess>( params );
}




/*
 * Process Table menu
*/

void ProjectViewer::on_actionConvert_Picks_To_Horizon_triggered()
{
    Q_ASSERT( m_project );

    TableToHorizonDialog dlg;
    dlg.setWindowTitle(tr("Convert Table To Horizon"));
    dlg.setProject(m_project);
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<TableToHorizonProcess>( params );
}


/*
 * Inversion menu
*/

void ProjectViewer::on_actionFD_CDP_Acoustic_triggered()
{
    Q_ASSERT( m_project );

    FMCDP2DDialog dlg;
    dlg.setWindowTitle(tr("FD CDP 2D (Acoustic)"));
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<FMCDP2DProcess>( params );
}


/*
 * Analyze menu
*/


void ProjectViewer::on_actionCrossplot_Grids_triggered()
{

    Q_ASSERT( m_project );

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


    Crossplot::Data data=Crossplot::createFromGrids(grid1.get(), grid2.get(), horizon.get(), grida.get());
    Crossplot crossplot;
    crossplot.addDataset(data);

    CrossplotViewer* viewer=new CrossplotViewer();
    viewer->setAttribute( Qt::WA_DeleteOnClose);

    viewer->setWindowTitle( QString("%1 vs %2").arg(dlg.xName(), dlg.yName() ) );
    viewer->show();
    viewer->setCrossplot(crossplot); // add data after visible!!!! /// XXX QVECTOR!!!
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

    std::shared_ptr<Volume > volume1=m_project->loadVolume(dlg.xName());
    if( !volume1 ) return;

    std::shared_ptr<Volume > volume2=m_project->loadVolume( dlg.yName());
    if( !volume2 ) return;

    if( volume1->bounds()!=volume2->bounds()){
        QMessageBox::critical(this, "Crossplot Volumes", "Currently only volumes with the same geometry are accepted!");
        return;
    }

    std::shared_ptr<Volume > volumea;
    if( dlg.useAttribute()){
         volumea=
                 m_project->loadVolume( dlg.attributeName());
         if( volumea->bounds()!=volume1->bounds()){
             QMessageBox::critical(this, "Crossplot Volumes",
                "Currently only volumes with the same geometry are accepted!");
             return;
         }
     }

    Grid2DBounds bounds(volume1->bounds().i1(), volume1->bounds().j1(),
                        volume1->bounds().i2(), volume1->bounds().j2());
    int nt=volume1->bounds().nt();
    //float t1=volume1->bounds().ft();
    //float t2=volume2->bounds().lt();

    Crossplot::Data data;

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
        data=Crossplot::createFromVolumes(volume1.get(), volume2.get(),
                                          volumea.get() );
    }
    else{
        data=Crossplot::createFromVolumesReduced(volume1.get(), volume2.get(),
                                                 MAX_POINTS, volumea.get());
    }

    Crossplot crossplot;
    crossplot.addDataset(data);

    CrossplotViewer* viewer=new CrossplotViewer();
    viewer->setAttribute( Qt::WA_DeleteOnClose);

    viewer->setWindowTitle( QString("%1 vs %2").arg(dlg.xName(), dlg.yName() ) );
    viewer->show();
    viewer->setCrossplot(crossplot); // add data after visible!!!!
    viewer->setAxisLabels(dlg.xName(), dlg.yName());
    viewer->setDetailedPointInformation( ( data.size() < MAX_POINTS ) );    // turn detailed info off if there are too many points
    viewer->setDispatcher(m_dispatcher);

}


void ProjectViewer::on_actionCrossplot_Logs_triggered()
{
    bool ok=false;
    //QString well = QInputDialog::getItem( this, tr("Crossplot Logs"), tr("Select Well:"), m_project->wellList(), 0, false, &ok );
    //if( well.isEmpty() || !ok ) return;
    // build list of uwi with well name for easier selection
    QStringList avail;
    for(auto uwi : m_project->wellList()){
        auto winfo=m_project->getWellInfo(uwi);
        auto name=tr("%1|%2").arg(uwi,winfo.name());
        avail.append(name);
    }
    QStringList selected=MultiItemSelectionDialog::getItems(this, "Crossplot Logs", "Select Well(s)", avail, &ok);
    if( selected.isEmpty() || !ok ) return;

    // extract uwi from selected names
    QStringList wells;
    for(auto name : selected){
        auto uwiAndName=name.split("|", QString::SkipEmptyParts);
        if(uwiAndName.size()<1) continue;   // skip invalid empty names
        auto uwi=uwiAndName[0];
        wells.append(uwi);
    }
    crossplotLogs(wells);
    /*
    // find logs that all selected wells have in common
    QSet<QString> savail;
    for(auto well : wells){
        auto wlogs=m_project->logList(well);
        if( savail.isEmpty()){
            savail=wlogs.toSet();
        }
        else{
            savail=savail.intersect(wlogs.toSet());
        }
    }

    ok=false;
    QStringList avail;
    avail<<"NONE";
    //avail<<m_project->logList(well);
    auto tmp=savail.toList();
    std::sort( std::begin(tmp), std::end(tmp));
    avail<<tmp;
    auto names=MultiInputDialog::getItems(this, tr("Select Logs for Crossplot"), QStringList{"X-Axis:", "Y-Axis:", "Attribute:"}, avail, &ok  );
    if( names.size()<2 || !ok ) return;

    auto log1name=names[0];//dlg.selection1();
    auto log2name=names[1];//dlg.selection2();
    auto log3name=names[2];//dlg.selection2();
    QString log1unit;
    QString log2unit;
    QString log3unit;
    crossplot::Data data;

    for( auto well : wells){

        if( log1name=="NONE") return;
        auto log1=m_project->loadLog(well, log1name);
        if( !log1 ){
            QMessageBox::critical(this, tr("Crossplot Logs"), QString("Loading log \"%1-%2\" failed!").arg(well,log1name));
            return;
        }
        log1unit=log1->unit().c_str();

        if( log2name=="NONE") return;
        auto log2=m_project->loadLog(well, log2name);
        if( !log2 ){
            QMessageBox::critical(this, tr("Crossplot Logs"), QString("Loading log \"%1-%2\" failed!").arg(well,log2name));
            return;
        }
        log2unit=log2->unit().c_str();

        std::shared_ptr<Log> log3;  // null by default
        if( log3name!="NONE"){
            log3=m_project->loadLog(well, log3name);
            if( !log3 ){
                QMessageBox::critical(this, tr("Crossplot Logs"), QString("Loading log \"%1-%2\" failed!").arg(well,log3name));
                return;
            }
        }

        crossplot::Data wdata=crossplot::createFromLogs(log1.get(), log2.get(), log3.get());
        data.insert( std::end(data), std::begin(wdata), std::end(wdata));

    }



    CrossplotViewer* viewer=new CrossplotViewer();
    viewer->setAttribute( Qt::WA_DeleteOnClose);

    viewer->setWindowTitle( QString("%2 vs %3").arg( log1name, log2name ) );
    viewer->show();
    viewer->setFixedColor(log3name=="NONE");
    viewer->setInlinesSelectable(false);
    viewer->setCrosslinesSelectable(false);
    viewer->setData(data); // add data after visible!!!
    auto view=viewer->view();
    view->xAxis()->setName(log1name);
    view->xAxis()->setUnit(log1unit);
    view->zAxis()->setName(log2name);
    view->zAxis()->setUnit(log2unit);
    //viewer->setAxisLabels(QString("%1 [%2]").arg(log1name, log1unit),
    //                      QString("%1 [%2]").arg(log2name, log2unit) );
    //viewer->setDetailedPointInformation( ( data.size() < MAX_POINTS ) );    // turn detailed info off if there are too many points
    viewer->setDispatcher(m_dispatcher);
*/
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


void ProjectViewer::on_actionFrequency_Spectrum_Plot_triggered()
{
    SpectrumViewer* viewer=new SpectrumViewer;
    viewer->setAttribute( Qt::WA_DeleteOnClose);
    viewer->setWindowTitle( QString("Frequency Spectrum") );
    viewer->show();
    viewer->setProject(m_project);
    viewer->setDispatcher(m_dispatcher);
}


void ProjectViewer::on_actionLog_TVD_Plot_triggered()
{

    SelectWellsByLogDialog dlg;
    dlg.setProject(m_project);
    dlg.setWindowTitle(tr("Select Log/Wells"));
    if( dlg.exec()!=QDialog::Accepted) return;

    QVector<ScatterPlotPoint> data;

    auto name=dlg.log();
    for( auto uwi : dlg.wells()){

        auto log=m_project->loadLog(uwi, name);
        if( !log ){
            QMessageBox::critical(this, "Log TVD Scatter Plot", QString("Loading log %1-%2 failed!").arg(uwi,name), QMessageBox::Ok);
            return;
        }

        auto wp=m_project->loadWellPath(uwi);
        if( !wp ){
            QMessageBox::critical(this, "Log TVD Scatter Plot", QString("Loading well path for %1 failed!").arg(uwi), QMessageBox::Ok);
            return;
        }

        for( size_t i=0; i<log->nz(); i++){

            auto md = log->index2z(i);
            auto z=wp->zAtMD(md);
            auto value=(*log)[i];
            if( value==log->NULL_VALUE) continue;
            data.push_back(ScatterPlotPoint{ QPointF(value,z), uwi } );
        }
    }

    auto viewer=new ScatterplotViewer();
    viewer->setWindowTitle("Log(s) vs Z Scatter Plot");
    viewer->view()->zAxis()->setReversed(true);
    viewer->setAxisLabels(name, "Z");
    viewer->show();
    viewer->setData(data);
}


void ProjectViewer::on_actionColor_Composite_Grids_triggered()
{
    ColorCompositeViewer* viewer=new ColorCompositeViewer();
    viewer->setWindowTitle(tr("Color Composite Viewer"));
    viewer->setProject(m_project);
    viewer->show();
}


void ProjectViewer::on_actionOpen_Slice_Viewer_triggered()
{
    auto viewer=new sliceviewer::SliceViewer;
    viewer->setProject(m_project);
    viewer->setWindowTitle("2D Volume Viewer");
    viewer->show();
}


void ProjectViewer::on_actionOpen_2DViewer_triggered()
{
    MapViewer2* viewer=new MapViewer2();
    viewer->setWindowTitle(QString("2D Viewer - %1").arg( m_projectFileName ) );
    viewer->setProject(m_project);
    viewer->show();
}


void ProjectViewer::on_actionOpen_3D_Viewer_triggered()
{
    VolumeViewer* viewer=new VolumeViewer;
    viewer->setWindowTitle(QString("3D Viewer - %1").arg(m_projectFileName));
    viewer->setProject(m_project);

    viewer->setDispatcher(m_dispatcher);
    viewer->show();

    auto bb = m_project->geometry().bboxLines();
    auto dims=VolumeDimensions(bb.left(), bb.right(), bb.top(), bb.bottom(), 0, 5000 );
    viewer->setDimensions(dims);
}


void ProjectViewer::on_actionOpen_Log_Viewer_triggered()
{
    LogViewer* lv=new LogViewer();
    lv->setWindowTitle(tr("Log Viewer"));
    lv->setAttribute( Qt::WA_DeleteOnClose);
    lv->show();

    lv->setProject(m_project);
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



/*
 * Help menu
*/


void ProjectViewer::on_action_About_triggered()
{
    AboutDialog dlg(0);
    dlg.setWindowTitle(QString("About %1").arg(QString(PROGRAM) ) );
    dlg.exec();
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
//*  EXPIRATION CHECK DISABLED FOR MIKE'S BROKEN DONGLE 17-11-15
    if( validLicense ){

        myCKeylok->CheckExpiration();
        unsigned DateRead = myCKeylok->GetExpiration();
        info.expirationMonth=myCKeylok->GetExpMonth(DateRead);
        info.expirationDay=myCKeylok->GetExpDay(DateRead);
        info.expirationYear=myCKeylok->GetExpYear(DateRead);
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
            break;
        }
    }

    if( !validLicense){
        seismic::SEGYReader::postReadFunc = crypt::decrypt;
    }

    //ui->menuProcess_Volume->setEnabled(validLicense);
    ui->menuProcess_Well->setEnabled(validLicense);
    //ui->volumesView->setEnabled(validLicense);
    ui->wellsView->setEnabled(validLicense);
    ui->actionCrossplot_Logs->setEnabled(validLicense);
    ui->actionOpen_Log_Viewer->setEnabled(validLicense);

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

bool ProjectViewer::eventFilter(QObject *watched, QEvent *event){
    QTableView* tv=dynamic_cast<QTableView*>(watched);
    if( tv && event->type() == QEvent::KeyPress ) {
        QKeyEvent *ke = (QKeyEvent *)event;
        if ( ke->key() == Qt::Key_Delete ) {

            QStringList names;
            for( auto idx : tv->selectionModel()->selectedRows(0)){
                names.push_back(tv->model()->itemData( idx)[Qt::DisplayRole].toString());
            }



           if( tv==ui->datasetsView){
                for( auto name:names){
                    removeSeismicDataset(name);
                }
           }
           else if( tv==ui->horizonsView){
                removeGrids(GridType::Horizon, names);
           }
           else if( tv==ui->attributesView){
                removeGrids(GridType::Attribute, names);
           }
           else if( tv==ui->gridsView){
                removeGrids(GridType::Other, names);
           }
           else if( tv==ui->volumesView){
                removeVolumes( names);
           }
           else if( tv==ui->wellsView){
                removeWells(names);
            }

            ke->accept();
            return true;
        }
    }
    return watched->event(event);
}



void ProjectViewer::importGrid(GridType gridType){

    ImportGridDialog dlg;
    dlg.setProject(m_project);
    dlg.setType(gridType);
    dlg.setWindowTitle(tr("Import Grid"));

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<ImportGridProcess>( params );
}



void ProjectViewer::selectAndExportSeismicDataset(){

    ExportSeismicDialog dlg(this);
    dlg.setWindowTitle("Export Seismic Dataset");
    dlg.setDatasets(m_project->seismicDatasetList());

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<ExportSeismicProcess>( params );
}

void ProjectViewer::exportSeismicDataset( QString name ){

    ExportSeismicDialog dlg(this);
    dlg.setWindowTitle("Export Seismic Dataset");
    dlg.setFixedDataset(name);

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<ExportSeismicProcess>( params );
}

void ProjectViewer::selectAndExportGrid(GridType gridType)
{
    ExportGridDialog dlg;

    dlg.setProject(m_project);
    dlg.setWindowTitle(tr("Export Grid"));
    dlg.setType(gridType);

    if( dlg.exec()!=QDialog::Accepted) return;

    auto params=dlg.params();
    runProcess<ExportGridProcess>( params );
}


void ProjectViewer::exportGrid( GridType gridType, QString gridName){

    ExportGridDialog dlg;

    dlg.setProject(m_project);
    dlg.setWindowTitle(tr("Export Grid"));
    dlg.setType(gridType);
    dlg.setName(gridName);

    if( dlg.exec()!=QDialog::Accepted) return;

    auto params=dlg.params();
    runProcess<ExportGridProcess>( params );
}

void ProjectViewer::selectAndExportLog()
{
    ExportLogDialog dlg;

    dlg.setProject(m_project);
    dlg.setWindowTitle(tr("Export Log"));

    if( dlg.exec()!=QDialog::Accepted) return;

    auto params=dlg.params();
    runProcess<ExportLogProcess>( params );
}


void ProjectViewer::selectAndExportTable()
{
    ExportTableDialog dlg;

    auto tables=m_project->tableList();
    dlg.setTables(tables);
    dlg.setWindowTitle(tr("Export Table"));

    if( dlg.exec()!=QDialog::Accepted) return;

    auto params=dlg.params();
    runProcess<ExportTableProcess>( params );
}

void ProjectViewer::exportTable(const QString& table)
{
    ExportTableDialog dlg;

    dlg.setFixedTable(table);
    dlg.setWindowTitle(tr("Export Table"));

    if( dlg.exec()!=QDialog::Accepted) return;

    auto params=dlg.params();
    runProcess<ExportTableProcess>( params );
}


void ProjectViewer::on_datasetsView_doubleClicked(const QModelIndex &idx)
{
    auto model=ui->datasetsView->model();
    QString name=model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    displaySeismicDataset(name);
}


void ProjectViewer::runGridContextMenu( GridType gridType, QTableView* view, const QPoint& pos){

    Q_ASSERT(view);

    auto idx=view->currentIndex();
    auto model=view->model();
    QString name=model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    QPoint globalPos = view->viewport()->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("display");
    menu.addAction("histogram");
    menu.addAction("view process parameters");
    menu.addSeparator();
    menu.addAction("export");
    menu.addSeparator();
    menu.addAction("rename");
    menu.addAction("remove");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="display" ){
        displayGrid( gridType, name);
    }
    else if( selectedAction->text()=="histogram" ){
        displayGridHistogram(gridType, name);
    }
    else if( selectedAction->text()=="view process parameters" ){
        displayProcessParams( name, m_project->getGridProcessParams(gridType, name));
    }
    else if( selectedAction->text()=="export" ){
        exportGrid( gridType,name);
    }
    else if( selectedAction->text()=="rename" ){
        renameGrid( gridType,name);
     }
    else if( selectedAction->text()=="remove" ){

        // if items are selected remove all
        if( view->selectionModel()->selectedIndexes().size()>0 ){
            QStringList names;
            for( auto idx : view->selectionModel()->selectedRows(0)){
                names.push_back(view->model()->itemData(idx)[Qt::DisplayRole].toString() );
            }
            removeGrids( gridType, names);
        }
        else{   // no items selected remove only one at pos
            removeGrid( gridType,name);
        }
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
    auto model=ui->horizonsView->model();
    QString name=model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    displayGrid(GridType::Horizon, name);
}

void ProjectViewer::on_attributesView_doubleClicked(const QModelIndex &idx)
{
    auto model=ui->attributesView->model();
    QString name=model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    displayGrid(GridType::Attribute, name);
}

void ProjectViewer::on_gridsView_doubleClicked(const QModelIndex &idx)
{
    auto model=ui->gridsView->model();
    QString name=model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    displayGrid(GridType::Other, name);
}

void ProjectViewer::on_volumesView_doubleClicked(const QModelIndex &idx)
{
    auto model=ui->volumesView->model();
    QString name=model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    displayVolumes(QStringList(name));
}

void ProjectViewer::runVolumeContextMenu( const QPoint& pos){

    auto view=ui->volumesView;

    /*
    auto idx=view->currentIndex();
    auto model=view->model();
    QString name=model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    */
    QPoint globalPos = view->viewport()->mapToGlobal(pos);

    if( view->selectionModel()->selectedRows().isEmpty() ) return;
    QStringList names;
    for( auto idx : view->selectionModel()->selectedRows(0)){
        names.push_back(view->model()->itemData( idx)[Qt::DisplayRole].toString());
    }

    QMenu menu;
    menu.addAction("display");
    menu.addAction("display slice");
    menu.addAction("display horizon relative slice");
    menu.addAction("display 3d");
    menu.addAction("histogram");
    menu.addAction("view process parameters");
    menu.addSeparator();
    menu.addAction("export");
    menu.addAction("export seisware");
    menu.addSeparator();
    menu.addAction("properties");
    menu.addAction("rename");
    menu.addAction("remove");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="display slice" ){
        displayVolumeSlice( names.first());
    }
    else if( selectedAction->text()=="display" ){
        displayVolumes( names);
    }
    else if( selectedAction->text()=="display 3d" ){
        displayVolume3D( names.first());
    }
    else if(selectedAction->text()=="display horizon relative slice"){
        displayVolumeRelativeSlice( names.first());
    }
    else if( selectedAction->text()=="histogram" ){
        displayVolumeHistogram( names.first());
    }
    else if( selectedAction->text()=="view process parameters" ){
        displayProcessParams( names.first(), m_project->getVolumeProcessParams(names.first()));
    }
    else if( selectedAction->text()=="export" ){
        exportVolume( names.first());
    }
    else if( selectedAction->text()=="export seisware" ){
        exportVolumeSeisware( names.first());
    }
    else if( selectedAction->text()=="properties" ){
        editVolumeProperties( names.first());
    }
    else if( selectedAction->text()=="rename" ){
        renameVolume( names.first());
    }
    else if( selectedAction->text()=="remove" ){

        removeVolumes(names);
        /*
        // if items are selected remove all
        if( view->selectionModel()->selectedRows().size()>0 ){
            QStringList names;
            for( auto idx : view->selectionModel()->selectedRows(0)){
                names.push_back(view->model()->itemData( idx)[Qt::DisplayRole].toString());
            }
            removeVolumes( names);
        }
        else{   // no items selected remove only one at pos
            removeVolume( name);
        }*/
    }
}


void ProjectViewer::on_wellsView_doubleClicked(const QModelIndex &idx)
{
    auto model=ui->wellsView->model();
    QString name=model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    selectAndDisplayLog(QStringList(name));
}

void ProjectViewer::runWellContextMenu( const QPoint& pos){

    auto view=ui->wellsView;
    auto idx=view->currentIndex();
    auto model=view->model();
    //QString name=model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    QStringList names;
    for( auto idx : view->selectionModel()->selectedRows(0)){
        names.push_back(view->model()->itemData(idx)[Qt::DisplayRole].toString() );
    }
    if( names.isEmpty()) return;
    QPoint globalPos = view->viewport()->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("display log(s)");
    menu.addAction("display log spectrum");
    menu.addAction("crossplot logs");
    menu.addAction("log properties");
    menu.addAction("edit log");
    menu.addAction("remove log");
    menu.addSeparator();
    //menu.addAction("import well path");
    menu.addAction("edit tops");
    menu.addAction("remove top");
    menu.addSeparator();
    menu.addAction("edit well");
    menu.addAction("remove well");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="display log(s)" ){
        selectAndDisplayLog( names );
    }
    else if( selectedAction->text()=="display log spectrum" ){
        selectAndDisplayLogSpectrum( names[0] );
    }
    else if( selectedAction->text()=="crossplot logs" ){
        crossplotLogs( names);
    }
    else if( selectedAction->text()=="log properties" ){
        logProperties( names[0]);
     }
    else if( selectedAction->text()=="edit log" ){
        editLog( names[0] );
     }
    else if( selectedAction->text()=="remove log" ){
        removeLogs( names );
     }
    else if( selectedAction->text()=="edit well" ){
        editWell( names[0]);
     }
    else if( selectedAction->text()=="import well path" ){
        importWellPath( names[0]);
     }
    else if( selectedAction->text()=="edit tops" ){
        editTops( names[0]);
     }
    else if( selectedAction->text()=="remove top" ){
        removeTops( names );
     }
    else if( selectedAction->text()=="remove well" ){

        removeWells( names);
    }
}

void ProjectViewer::on_tablesView_doubleClicked(const QModelIndex &idx)
{
    auto model=ui->tablesView->model();
    QString name=model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    displayTable(name);
}

void ProjectViewer::runTableContextMenu( const QPoint& pos){

    auto view=ui->tablesView;
    auto idx=view->currentIndex();
    auto model=view->model();
    QStringList names;
    for( auto idx : view->selectionModel()->selectedRows(0)){
        names.push_back(view->model()->itemData(idx)[Qt::DisplayRole].toString() );
    }
    if( names.isEmpty()) return;
    QPoint globalPos = view->viewport()->mapToGlobal(pos);

    QMenu menu;

    menu.addAction("display");
    menu.addAction("export");
    menu.addAction("rename");
    menu.addAction("remove");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="display" ){
        displayTable( names[0] );
    }
    else if( selectedAction->text()=="export" ){
        exportTable( names[0] );
    }
    else if( selectedAction->text()=="rename" ){
        renameTable( names[0] );
    }
    else if( selectedAction->text()=="remove" ){
        removeTables( names );
    }
}

void ProjectViewer::runDatasetContextMenu(const QPoint& pos){

    auto view=ui->datasetsView;
    auto idx=view->currentIndex();
    auto model=view->model();
    QStringList names;
    for( auto idx : view->selectionModel()->selectedRows(0)){
        names.push_back(view->model()->itemData(idx)[Qt::DisplayRole].toString() );
    }
    if( names.isEmpty()) return;
    QString name=names.front(); //model->itemData( model->index(idx.row(), 0))[Qt::DisplayRole].toString();
    QPoint globalPos = view->viewport()->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("display");
    menu.addAction("display slice");
    menu.addSeparator();
    menu.addAction("display index");
    menu.addAction("display EBCDIC");
    menu.addAction("view process parameters");
    menu.addAction("properties");
    menu.addSeparator();
    menu.addAction("export");
    menu.addSeparator();
    menu.addAction("remove");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="display" ){
        displaySeismicDataset(name);
    }
    else if( selectedAction->text()=="display slice" ){
        displaySeismicDatasetSlice(name);
    }
    else if( selectedAction->text()=="display index" ){
        displaySeismicDatasetIndex(name);
    }
    else if( selectedAction->text()=="display EBCDIC" ){
        displaySeismicDatasetEBCDICHeader(name);
    }
    else if( selectedAction->text()=="view process parameters" ){
        displayProcessParams( name, m_project->getSeismicDatasetProcessParams(name));
    }
    else if( selectedAction->text()=="properties" ){
        editSeismicDatasetProperties(name);
    }
    else if( selectedAction->text()=="export" ){
        exportSeismicDataset(name);
    }
    else if( selectedAction->text()=="remove" ){
        removeSeismicDatasets(names);
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

void ProjectViewer::removeGrids( GridType t, const QStringList& names){

    if( QMessageBox::question(this, QString("Remove Grids"),
             QString("Are you sure you want to remove %1 grids?").arg(names.size()), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
        return;
    }

    QString typeName=toQString(t);

    int nfail=0;
    for( auto name : names){
        if( !m_project->removeGrid( t, name)) nfail++;
    }

    if( nfail>0 ){
        QMessageBox::critical(this, QString("Remove Grids"), QString("Removing %1 grids of type %2 failed!").arg(QString::number(nfail), typeName));
    }
}


void ProjectViewer::selectAndExportVolume()
{
    ExportVolumeDialog dlg(this);
    dlg.setWindowTitle("Export Volume");
    dlg.setVolumes(m_project->volumeList());

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<ExportVolumeProcess>( params );
}

void ProjectViewer::on_actionExport_with_reference_triggered()
{
    ExportVolumeRefDialog dlg(this);
    dlg.setWindowTitle("Export Volume (reference)");
    dlg.setProject(m_project);

    if( dlg.exec()!=QDialog::Accepted) return;

    QMap<QString,QString> params=dlg.params();

    runProcess<ExportVolumeRefProcess>( params );
}

void ProjectViewer::displayVolume3D( const QString& name){

    if( m_project->volumeList().contains(name)){

        std::shared_ptr<Volume > volume=m_project->loadVolume( name);
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

        std::shared_ptr<Volume > volume=m_project->loadVolume( name);
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

        std::shared_ptr<Volume > volume=m_project->loadVolume( name);
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


void ProjectViewer::displayVolumes(const QStringList & names){

    auto viewer=new sliceviewer::SliceViewer;
    viewer->setProject(m_project);
    viewer->show();
    for( auto name:names){
        viewer->addVolume(name);
    }
}


void ProjectViewer::editVolumeProperties(const QString& name){

    auto volume=m_project->loadVolume(name);
    if( !volume) return;

    VolumePropertyDialog dlg;
    dlg.setWindowTitle(QString("Volume Properties - %1").arg(name));
    dlg.setDomain(volume->domain());
    dlg.setType(volume->type());
    dlg.setZ0(volume->ft());
    dlg.setDZ(volume->dt());
    if( dlg.exec()!=QDialog::Accepted) return;

    volume->setDomain(dlg.domain());
    volume->setType(dlg.type());
    volume->setFT(dlg.z0());
    volume->setDT(dlg.dz());

    if( !m_project->saveVolume(name, volume)){
        QMessageBox::critical(this, tr("Edit Volume Properties"), QString("Saving volume %1 failed!").arg(name), QMessageBox::Ok);
    }
}

void ProjectViewer::displayVolumeHistogram( const QString& name){

    if( m_project->volumeList().contains(name)){

        std::shared_ptr<Volume > volume=m_project->loadVolume( name);
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

    runProcess<ExportVolumeProcess>( params );

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

    std::shared_ptr<Volume> volume=m_project->loadVolume(volumeName);
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

    //double ft=bounds.ft();
    size_t ns=bounds.nt();
    size_t dt=1000000*bounds.dt();   // make microseconds

    seismic::SEGYTextHeaderStr textHeaderStr;
    textHeaderStr.push_back("Seisware SEGY created with AVOUtensil");
    textHeaderStr.push_back(QString("Content:     Volume %1").arg(volumeName).toStdString());
    textHeaderStr.push_back(QString("Inlines:     %1 - %2").arg(bounds.i1()).arg(bounds.i2()).toStdString());
    textHeaderStr.push_back(QString("Crosslines:  %1 - %2").arg(bounds.j1()).arg(bounds.j2()).toStdString());
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
    bhdr["startx"]=seismic::HeaderValue::makeIntValue(bounds.j1());
    bhdr["starty"]=seismic::HeaderValue::makeIntValue(bounds.i1());
    bhdr["startz"]=seismic::HeaderValue::makeIntValue(1000000*bounds.ft());
    bhdr["endx"]=seismic::HeaderValue::makeIntValue(bounds.j2());
    bhdr["endy"]=seismic::HeaderValue::makeIntValue(bounds.i2());
    bhdr["endz"]=seismic::HeaderValue::makeIntValue(1000000*bounds.lt());
    bhdr["totphase"]=seismic::HeaderValue::makeFloatValue(0);
    bhdr["totgain"]=seismic::HeaderValue::makeFloatValue(1);
    bhdr["gainexp"]=seismic::HeaderValue::makeFloatValue(1);
    bhdr["corner1"]=seismic::HeaderValue::makeIntValue(0);
    bhdr["corner2"]=seismic::HeaderValue::makeIntValue(bounds.nj()-1);
    bhdr["corner3"]=seismic::HeaderValue::makeIntValue(bounds.ni()*bounds.nj()-1);
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

    seismic::Trace trace(bounds.ft(), bounds.dt(), bounds.nt() );
    seismic::Header& thdr=trace.header();
    seismic::Trace::Samples& samples=trace.samples();


    QProgressDialog progress(this);
    progress.setWindowTitle("Export Volume");
    progress.setLabelText("Writing Seisware SEGY");
    progress.setRange(0, bounds.ni());
    progress.show();

    int tn=0;
    for( int iline=bounds.i1(); iline<=bounds.i2(); iline++){

        for( int xline=bounds.j1(); xline<=bounds.j2(); xline++){

            tn++;

            // gather trace sample statistics and fill trace
            float peak=std::numeric_limits<float>::lowest();
            double sum=0;
            double sum2=0;
            size_t n=0;
            for( int i=0; i<bounds.nt(); i++){

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

            int cdp=1 + (iline-bounds.i1())*bounds.nj() + xline - bounds.j1();

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

        progress.setValue(iline-bounds.i1());
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
    for( int iline=bounds.i1(); iline<=bounds.i2(); iline++){

        int32_t offset=static_cast<int32_t>((iline-bounds.i1())*bounds.nj());
        int32_t line=static_cast<int32_t>(iline);
        int32_t trace=static_cast<int32_t>(bounds.j1());
        int32_t numTraces=static_cast<int32_t>(bounds.nj());

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

void ProjectViewer::removeVolumes( const QStringList& names){

    if( QMessageBox::question(this, QString("Remove Grids"),
             QString("Are you sure you want to remove %1 volumes?").arg(names.size()), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
        return;
    }

    int nfail=0;
    for( auto name : names){
        if( !m_project->removeVolume( name)) nfail++;
    }

    if( nfail>0 ){
        QMessageBox::critical(this, QString("Remove Volumes"), QString("Removing %1 volumes failed!").arg(QString::number(nfail)));
    }
}


void ProjectViewer::selectAndDisplayLog( const QStringList& wells){

    // find logs that all wells contain
    QSet<QString> commonLogs;
    for( auto well:wells){
        auto wlogs=QSet<QString>::fromList(m_project->logList(well));
        if( commonLogs.empty()){
            commonLogs=wlogs;
        }
        else{
            commonLogs=commonLogs.intersect(wlogs);
        }
    }

    auto avail=QStringList::fromSet(commonLogs);
    std::sort(avail.begin(), avail.end());
    QStringList names = MultiItemSelectionDialog::getItems( this, tr("Display Logs"), tr("Select logs"), avail );

    if( names.empty()) return;

    //auto info =m_project->getWellInfo(well);

    LogViewer* viewer=new LogViewer;
    viewer->setWindowTitle( "LogViewer");//QString("%1 - %2").arg(well, info.name() ) );
    viewer->setProject(m_project);
    viewer->setDispatcher(m_dispatcher);
    //viewer->setReceptionEnabled(true);
    //viewer->setUWI(well);
    viewer->show();

    for( auto name : names){
        for( auto well:wells){
            viewer->addLog(well, name);
        }
    }
}


void ProjectViewer::selectAndDisplayLogSpectrum( const QString& well){

    LogSpectrumViewer* viewer=new LogSpectrumViewer();
    viewer->setProject(m_project);
    viewer->setUWI(well);
    viewer->show();
    viewer->openLog();
}


void ProjectViewer::editWell( const QString& name){

    if( ! m_project->existsWell(name)) return;

    auto info = m_project->getWellInfo(name);

    EditWellDialog dlg(this);
    dlg.setWindowTitle(tr("Edit Well %1").arg(name));
    dlg.setUWI(info.uwi());
    dlg.setName(info.name());
    dlg.setX(info.x());
    dlg.setY(info.y());
    dlg.setZ(info.z());

    if( dlg.exec()==QDialog::Accepted){
        info.setUWI(dlg.uwi());
        info.setName(dlg.name());
        info.setX(dlg.x());
        info.setY(dlg.y());
        info.setZ(dlg.z());

        if( !m_project->setWellInfo(name, info)){
            QMessageBox::critical(this, tr("Edit Well"), tr("Storing well info failed!"));
        }
    }
}

void ProjectViewer::editTops( const QString& name){

    if( ! m_project->existsWell(name)) return;

    auto pmks=m_project->loadWellMarkersByWell(name);
    if( !pmks ) qFatal("Loading well markers failed!");

    EditMarkersDialog dlg;
    dlg.setWindowTitle(QString("Edit Tops - %1").arg(name));
    dlg.setMarkers(*pmks);
    if( dlg.exec()!=QDialog::Accepted) return;

    auto mks=dlg.markers();
    if( !m_project->saveWellMarkers(name, mks )){
        QMessageBox::critical(this, tr("Edit Tops"), tr("Saving tops failed!"), QMessageBox::Ok);
    }
}

void ProjectViewer::removeTops(const QStringList& names){
/*
    QStringList topnames;
    for( auto well : names ){
        auto llist=m_project->logList(well);
        for( auto lname:llist){
            if( !lognames.contains(lname)) lognames.push_back(lname);
        }
    }
    std::sort( lognames.begin(), lognames.end());
    QStringList lnames = MultiItemSelectionDialog::getItems( this, tr("Remove Log(s)"), tr("Select log(s)"), lognames );

    if( lnames.empty()) return;

    auto reply=QMessageBox::warning(this, tr("Remove Log(s)"), QString("Remove %1 log(s)?").arg(lnames.size()), QMessageBox::Yes | QMessageBox::No);
    if( reply!=QMessageBox::Yes) return;

    for( auto well: names ){
        for( auto lname : lnames ){
            m_project->removeLog(well, lname);
        }
    }*/
}

bool ProjectViewer::importWellPath(const QString & id, const QString& initialPath){

    if( ! m_project->existsWell(id)) return false;
    auto info = m_project->getWellInfo(id);

    XYZImportDialog dlg;
    dlg.setWindowTitle(QString("Import Well Path - %1 (%2)").arg(id, info.name()));
    if( !initialPath.isEmpty()) dlg.setFilename(initialPath);
    if( dlg.exec()!=QDialog::Accepted) return false;

    if( dlg.data().empty() ) return false;

    if( !m_project->saveWellPath(id, dlg.data())){
        QMessageBox::critical(this, tr("Import Well Path"), tr("Saving Path failed!"), QMessageBox::Ok);
        return false;
    }

    auto result = QMessageBox::question( this, tr("Import Well Path"), tr("Update Well Coordinates?"), QMessageBox::Yes | QMessageBox::No);
    if(result==QMessageBox::Yes  ){
        info.setX( dlg.data().front().x());
        info.setY( dlg.data().front().y());
        info.setZ( dlg.data().front().z());
        if( !m_project->setWellInfo(id, info)){
            QMessageBox::critical(this, tr("Set Well Coordinates"), tr("Storing well info failed!"));
        }
    }

    return true;
}


bool ProjectViewer::importWellPathBulk( QVector<std::pair<QString, QString>> uwi_path){

    if( uwi_path.empty() ) return false;

    XYZImportDialog dlg;
    auto uwi = uwi_path.front().first;
    auto path=uwi_path.front().second;
    dlg.setWindowTitle(QString("Import Well Paths Bulk - template (%1)").arg(uwi));
    dlg.setFilename(path);
    if( dlg.exec()!=QDialog::Accepted) return false;
    dlg.hide();

    auto result = QMessageBox::question( this, tr("Import Well Paths Bulk"), tr("Update Well Coordinates?"), QMessageBox::Yes | QMessageBox::No);
    bool updateCoords=(result==QMessageBox::Yes);

    QProgressDialog pdlg(this);
    pdlg.setWindowTitle(QString("Import %1 paths").arg(uwi_path.size()));
    pdlg.show();
    pdlg.setMaximum(uwi_path.size());

    for( int i=0; i<uwi_path.size(); i++){

        auto uwi = uwi_path[i].first;
        auto path=uwi_path[i].second;
        pdlg.setLabelText(QString("Well %1").arg(uwi));

        if( ! m_project->existsWell(uwi)) return false;
        auto info = m_project->getWellInfo(uwi);

        dlg.setFilename(path);
        auto data=dlg.loadData();


        if( !data.empty()){

            if( !m_project->saveWellPath(uwi, data)){
                QMessageBox::critical(this, tr("Import Well Path"), tr("Saving Path failed!"), QMessageBox::Ok);
                return false;
            }

            if( updateCoords ){
                info.setX( data.front().x());
                info.setY( data.front().y());
                info.setZ( data.front().z());
                if( !m_project->setWellInfo(uwi, info)){
                    QMessageBox::critical(this, tr("Set Well Coordinates"), tr("Storing well info failed!"));
                }
            }

        }

        pdlg.setValue(i+1);
        qApp->processEvents();
        if( pdlg.wasCanceled()) break;
    }

    return true;
}

void ProjectViewer::crossplotLogs( const QStringList& wells ){

    if( !m_project ) return;

    if( wells.empty()) return;

    // find logs that all selected wells have in common
    QSet<QString> savail;
    for(auto well : wells){
        auto wlogs=m_project->logList(well);
        if( savail.isEmpty()){
            savail=wlogs.toSet();
        }
        else{
            savail=savail.intersect(wlogs.toSet());
        }
    }

    bool ok=false;
    QStringList avail;
    //avail<<"NONE";
    //avail<<m_project->logList(well);
    auto tmp=savail.toList();
    std::sort( std::begin(tmp), std::end(tmp));
    avail<<tmp;
    auto names=MultiInputDialog::getItems(this, tr("Select Logs for Crossplot"), QStringList{"X-Axis:", "Y-Axis:", "Attribute:"}, avail, &ok  );
    if( names.size()<2 || !ok ) return;

    auto log1name=names[0];//dlg.selection1();
    auto log2name=names[1];//dlg.selection2();
    auto log3name=names[2];//dlg.selection2();
    QString log1unit;
    QString log2unit;
    QString log3unit;
    Crossplot crossplot;

    QTransform xy_to_ilxl, ilxl_to_xy;
    m_project->geometry().computeTransforms(xy_to_ilxl, ilxl_to_xy);

    for( int ds=0; ds<wells.size(); ds++){

        auto well=wells[ds];

        auto log1=m_project->loadLog(well, log1name);
        if( !log1 ){
            QMessageBox::critical(this, tr("Crossplot Logs"), QString("Loading log \"%1-%2\" failed!").arg(well,log1name));
            return;
        }
        log1unit=log1->unit().c_str();

        auto log2=m_project->loadLog(well, log2name);
        if( !log2 ){
            QMessageBox::critical(this, tr("Crossplot Logs"), QString("Loading log \"%1-%2\" failed!").arg(well,log2name));
            return;
        }
        log2unit=log2->unit().c_str();

        auto log3=m_project->loadLog(well, log3name);
        if( !log3 ){
            QMessageBox::critical(this, tr("Crossplot Logs"), QString("Loading log \"%1-%2\" failed!").arg(well,log3name));
            return;
        }

        std::shared_ptr<WellPath> wp=m_project->loadWellPath(well);
        if( !wp ){
            QMessageBox::warning( this, "Crossplot Logs", QString("Loading wellpath for well \"%1\" failed!\nUsing generic wellpath...").arg(well),
                                  QMessageBox::Ok);
            QVector<QVector3D> points;
            for(int i=0; i<log1->nz(); i++){
                points.push_back(QVector3D(0,0,-log1->index2z(i)));  // z means md here!!!
            }
            wp=std::make_shared<WellPath>(points);
            if( !wp ){
                QMessageBox::critical(this, "Crossplot Logs", "Creating generic wellpath failed!", QMessageBox::Ok);
                return;
            }
        }

        auto wdata=Crossplot::createFromLogs( log1.get(), log2.get(), log3.get(), *wp, xy_to_ilxl);
        crossplot.addDataset(wdata,well);
    }

    CrossplotViewer* viewer=new CrossplotViewer();
    viewer->setAttribute( Qt::WA_DeleteOnClose);

    viewer->setWindowTitle( QString("%2 vs %3").arg( log1name, log2name ) );
    viewer->show();
    //viewer->setInlinesSelectable(false);
    //viewer->setCrosslinesSelectable(false);
    viewer->setCrossplot(crossplot); // add data after visible!!!
    auto view=viewer->view();
    view->xAxis()->setName(log1name);
    view->xAxis()->setUnit(log1unit);
    view->zAxis()->setName(log2name);
    view->zAxis()->setUnit(log2unit);
    //viewer->setAxisLabels(QString("%1 [%2]").arg(log1name, log1unit),
    //                      QString("%1 [%2]").arg(log2name, log2unit) );
    //viewer->setDetailedPointInformation( ( data.size() < MAX_POINTS ) );    // turn detailed info off if there are too many points
    viewer->setDispatcher(m_dispatcher);
}

void ProjectViewer::logProperties(const QString & well){

    QString name = QInputDialog::getItem( this, tr("Log Properties"), tr("Select log:"), m_project->logList(well) );

    if( name.isEmpty()) return;

    auto log = m_project->loadLog(well, name);
    if( !log){
        QMessageBox::critical(this, tr("Log Properties"), tr("Loading log \"%1 - %2\" failed!").arg(well, name), QMessageBox::Ok);
        return;
    }

    LogPropertiesDialog dlg;
    dlg.setWindowTitle( tr("Properties Log %1 - %2").arg(well,name));
    dlg.setName(log->name().c_str());
    dlg.setUnit(log->unit().c_str());
    dlg.setDescription(log->descr().c_str());

    if(dlg.exec()!=QDialog::Accepted) return;

    log->setName(dlg.name().toStdString());
    log->setUnit(dlg.unit().toStdString());
    log->setDescr(dlg.description().toStdString());

    bool ok=false;

    if( dlg.name() == name ){   // overwrite
        ok = m_project->saveLog(well, name, *log);
    }
    else{                   // rename
        ok = m_project->addLog(well, dlg.name(), *log);
        if( ok ) m_project->removeLog(well, name);     // only remove old after new is saved!!
    }

    if(!ok){
        QMessageBox::critical(this, tr("Log Properties"), tr("Saving log failed!"), QMessageBox::Ok);
    }
}

void ProjectViewer::editLog(const QString & well){

    auto avail=m_project->logList(well);
    std::sort( std::begin(avail), std::end(avail));
    QString name = QInputDialog::getItem( this, tr("Edit Log"), tr("Select log:"), avail );

    if( name.isEmpty()) return;

    auto log = m_project->loadLog(well, name);
    if( !log){
        QMessageBox::critical(this, tr("Edit Log"), tr("Loading log \"%1 - %2\" failed!").arg(well, name), QMessageBox::Ok);
        return;
    }

    LogEditDialog dlg;
    dlg.setWindowTitle( tr("Edit Log %1 - %2").arg(well,name));
    dlg.setLog(*log);

    if( dlg.exec()!=QDialog::Accepted ) return;

    if( !m_project->saveLog( well, name, dlg.log()) ){
        QMessageBox::critical( this, "Edit Log", "Saving log failed!", QMessageBox::Ok);
    }
}

void ProjectViewer::removeLogs(const QStringList& names){

    QStringList lognames;
    for( auto well : names ){
        auto llist=m_project->logList(well);
        for( auto lname:llist){
            if( !lognames.contains(lname)) lognames.push_back(lname);
        }
    }
    std::sort( lognames.begin(), lognames.end());
    QStringList lnames = MultiItemSelectionDialog::getItems( this, tr("Remove Log(s)"), tr("Select log(s)"), lognames );

    if( lnames.empty()) return;

    auto reply=QMessageBox::warning(this, tr("Remove Log(s)"), QString("Remove %1 log(s)?").arg(lnames.size()), QMessageBox::Yes | QMessageBox::No);
    if( reply!=QMessageBox::Yes) return;

    for( auto well: names ){
        for( auto lname : lnames ){
            m_project->removeLog(well, lname);
        }
    }
}

/*
void ProjectViewer::removeWell( const QString& name){

    if( name.isEmpty()) return;

    if( m_project->wellList().contains(name)){

        if( QMessageBox::question(this, QString("Remove Well"),
                 QString("Are you sure you want to remove well %1?").arg(name), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
            return;
        }

        m_project->removeWell( name );
    }
}
*/

void ProjectViewer::removeWells( const QStringList& names){

    if( QMessageBox::question(this, QString("Remove Wells"),
             QString("Are you sure you want to remove %1 wells?").arg(names.size()), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
        return;
    }

    int nfail=0;
    for( auto name : names){
       m_project->removeWell( name);
    }
}


void ProjectViewer::renameTable( const QString& name){

    if( m_project->tableList().contains(name)){
        bool ok=false;

        QString newName=QInputDialog::getText(this, QString("Rename Table \"%1\"").arg(name), "New Name:", QLineEdit::Normal, name, &ok);

        if( newName.isNull() || !ok) return;

        if( newName==name) return;

        if( m_project->tableList().contains(newName) ){
            QMessageBox::information(this, "Rename Table", QString("Table %1 already exists!").arg(newName));
            return;
        }

        if( ! m_project->renameTable( name, newName) ){
            QMessageBox::critical(this, "Rename Table", QString("Renaming table %1 failed!").arg(name));
        }

    }
}


void ProjectViewer::removeTables( const QStringList& names){

    if( QMessageBox::question(this, QString("Remove Tables"),
             QString("Are you sure you want to remove %1 tables?").arg(names.size()), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
        return;
    }

    int nfail=0;
    for( auto name : names){
       m_project->removeTable( name);
    }
}


void ProjectViewer::displayTable( const QString& name){

    auto table=m_project->loadTable(name);

    if( !table ){
        QMessageBox::critical(this, "Display Table", "Loading Table failed!", QMessageBox::Ok);
        return;
    }

    TableDialog* dlg=new TableDialog();
    dlg->setProject(m_project);
    dlg->loadTable(name);
    dlg->setAttribute( Qt::WA_DeleteOnClose);
    dlg->show();
}

void ProjectViewer::displayProcessParams(const QString& name, const ProcessParams& pp){

    if( pp.isEmpty()){  // for backwards compatability
        QMessageBox::warning(this, tr("View Process Parameters"), QString("No parameters found for %1!").arg(name));
        return;
    }

    auto pv=new ProcessParamsViewer(0);
    pv->setWindowTitle(QString("Process Parameters \"%1\"").arg(name));
    pv->setParams(pp);
    pv->show();
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
        viewer->setDatasetInfo(info);

        if( info.mode()==SeismicDatasetInfo::Mode::Prestack){

            if( info.dimensions()==3){
                selector->setOrder(SeismicDataSelector::ORDER_INLINE_ASCENDING,
                               SeismicDataSelector::ORDER_CROSSLINE_ASCENDING,
                               SeismicDataSelector::ORDER_OFFSET_ASCENDING);
            }else{  // 2d data: inline=const, xline=cdp
                selector->setOrder(SeismicDataSelector::ORDER_CROSSLINE_ASCENDING,
                               SeismicDataSelector::ORDER_OFFSET_ASCENDING,
                               SeismicDataSelector::ORDER_INLINE_ASCENDING);
            }

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
            //annos.push_back( std::pair< std::string, QString >( "cdp", QString("cdp")));
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
        connect( viewer, SIGNAL(firstRequested()), selector, SLOT(first()));
        connect( viewer, SIGNAL(previousRequested()), selector, SLOT(previous()));
        connect( viewer, SIGNAL(nextRequested()), selector, SLOT(next()));
        connect( viewer, SIGNAL(lastRequested()), selector, SLOT(last()));
        viewer->navigationToolBar()->addWidget(selector);
        //viewer->view()->leftRuler()->setAxxisLabel(verticalAxisLabel);
        connect( reader.get(), SIGNAL(started(int)), viewer->progressBar(), SLOT(setMaximum(int)));
        connect( reader.get(), SIGNAL(progress(int)), viewer->progressBar(), SLOT(setValue(int)));
        connect( reader.get(), SIGNAL(finished()), viewer->progressBar(), SLOT(reset()));
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

void ProjectViewer::displaySeismicDatasetEBCDICHeader(const QString& name){

    if( m_project->seismicDatasetList().contains(name)){

        std::shared_ptr<SeismicDatasetReader> reader=m_project->openSeismicDataset(name);
        if( !reader){
            QMessageBox::critical(this, "Display EBDIC Header", "Open reader failed!");
            return;
        }

        auto dlg=new EBCDICDialog;

        dlg->setHeader(reader->segyReader()->textHeaders()[0]);
        dlg->setWindowTitle( QString( "Textual Header of \"") + name + QString("\""));
        dlg->show();
     }
}

void ProjectViewer::editSeismicDatasetProperties(const QString& name){

    if( m_project->seismicDatasetList().contains(name)){

        DatasetPropertiesDialog dlg;
        dlg.setWindowTitle(QString("Properties of %1").arg(name));
        SeismicDatasetInfo info=m_project->getSeismicDatasetInfo(name);
        dlg.setName( name );
        dlg.setDimensions(info.dimensions());
        dlg.setDomain( info.domain());
        dlg.setMode(info.mode());

        if( dlg.exec()==QDialog::Accepted){

            if( info.domain()!=dlg.domain() || info.mode()!=dlg.mode() || info.dimensions()!=dlg.dimensions()){

                info.setDimensions(dlg.dimensions());
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
                 QString("Are you sure you want to remove dataset %1?\nThe underlying SEG-Y file will also ne deleted!").arg(name), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
            return;
        }

         if( !m_project->removeSeismicDataset(name)){
             QMessageBox::critical(this, "Remove Dataset", QString("Removing dataset %1 failed!").arg(name));
         }

         //updateProjectViews();
    }
}


void ProjectViewer::removeSeismicDatasets( const QStringList& names){

    if( QMessageBox::question(this, QString("Remove Datasets"),
             QString("Are you sure you want to remove %1 datasets?\nThe underlying SEG-Y files will also be deleted!").arg(names.size()), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes){
        return;
    }

    int nfail=0;
    for( auto name : names){
        if( !m_project->removeSeismicDataset(name)) nfail++;
    }

    if( nfail>0 ){
        QMessageBox::critical(this, QString("Remove Datasets"), QString("Removing %1 datasets failed!").arg(QString::number(nfail)));
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

    connectViews(m_project);

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


void ProjectViewer::connectDatasetsView(AVOProject* p){
    connect( p, SIGNAL(datasetsChanged()), this, SLOT(updateDatasetsView()) );
    connect( p,SIGNAL(datasetChanged(QString)), this, SLOT(updateDatasetsView(QString)) );
}

void ProjectViewer::connectGridViews( AVOProject* p){
    connect( p, SIGNAL(gridsChanged(GridType)), this, SLOT(updateGridsView(GridType)) );
    connect( p, SIGNAL(gridChanged(GridType, QString)), this, SLOT(updateGridsView(GridType, QString)) );
}

void ProjectViewer::connectVolumesView(AVOProject* p){
    connect( p, SIGNAL(volumesChanged()), this, SLOT(updateVolumesView()) );
    connect( p, SIGNAL(volumeChanged(QString)), this, SLOT(updateVolumesView(QString)) );
}

void ProjectViewer::connectWellsView(AVOProject* p){
    connect( p, SIGNAL(wellsChanged()), this, SLOT(updateWellsView()) );
    connect( p, SIGNAL(wellChanged(QString)), this, SLOT(updateWellsView(QString)) );
}

void ProjectViewer::connectTablesView(AVOProject* p){
    connect( p, SIGNAL(tablesChanged()), this, SLOT(updateTablesView()) );
    connect( p, SIGNAL(tableChanged(QString)), this, SLOT(updateTablesView(QString)) );
}

void ProjectViewer::connectViews(AVOProject* p){
    connectDatasetsView(p);
    connectGridViews(p);
    connectVolumesView(p);
    connectWellsView(p);
    connectTablesView(p);
}


// this allows numeric columns to be sorted correctly
template<typename T>
QStandardItem* newAlignedItem( const T& t, Qt::Alignment align=Qt::AlignCenter ){
    QStandardItem* item=new QStandardItem;
    if( !item ) return item;
    item->setData( QVariant(t), Qt::DisplayRole);
    item->setTextAlignment(align);
    return item;
}

QStandardItemModel* ProjectViewer::buildDatasetModelItem(QStandardItemModel *model, int row, QString name){

    if( !model ) return model;

    SeismicDatasetInfo info;
    QFileInfo finfo;
    bool attached=false;

    try{
        info=m_project->getSeismicDatasetInfo(name);
        finfo=m_project->getSeismicDatasetFileInfo(name);
        //std::cout<<QFileInfo(info.path()).canonicalPath().toStdString()<<" vs "<<m_project->seismicDirectory().toStdString()<<std::endl<<std::flush;
        attached=QFileInfo(info.path()).absolutePath()!=m_project->seismicDirectory();
    }catch(std::exception& e){
        QMessageBox::warning(this, tr("build datasets model"), QString("%1 :\n %2").arg(name,e.what()), QMessageBox::Ok);
        return nullptr;
    }

    auto reader=m_project->openSeismicDataset(name);
    if(!reader){
        QMessageBox::warning(this, tr("build datasets model"), QString("Open %1 failed").arg(name), QMessageBox::Ok);
        return nullptr;
    }

    int column=0;
    model->setItem(row, column++, new QStandardItem(name) );
    model->setItem(row, column++, newAlignedItem(datasetDomainToString(info.domain()), Qt::AlignHCenter));// new QStandardItem(datasetDomainToString(info.domain())));
    model->setItem(row, column++, newAlignedItem(datasetModeToString(info.mode()), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(QString::number(info.dimensions())+"D", Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(reader->minInline(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(reader->maxInline(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(reader->minCrossline(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(reader->maxCrossline(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(1000*info.ft(), Qt::AlignRight));                              // sec -> msec
    model->setItem(row, column++, newAlignedItem(1000*(info.ft()+info.dt()*(info.nt()-1)), Qt::AlignRight));    // sec -> msec
    model->setItem(row, column++, newAlignedItem(1000*info.dt(), Qt::AlignRight));                              // sec -> msec
    model->setItem(row, column++, newAlignedItem(finfo.size()/1024/1024, Qt::AlignRight));                      // mb
    model->setItem(row, column++, newAlignedItem(((attached)?"attached":"project"), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(finfo.lastModified(), Qt::AlignRight));

    reader->close();     // close reader immedeately so that next open does not fail

    return model;
}

QStandardItemModel* ProjectViewer::buildDatesetsModel(){

    try{

    QStringList labels;
    labels<<"Name"<<"Domain"<<"Mode"<<"Dims"<<"Min IL"<<"Max IL"<<"Min XL"<<"Max XL"<<"Start"<<"End"<<"Interval"<<"Size(MB)"<<"Source"<<"Last Modified";

    auto names=m_project->seismicDatasetList();
    QStandardItemModel* model=new QStandardItemModel(names.size(), labels.size(), this);
    model->setHorizontalHeaderLabels(labels);

    int row=0;
    for( auto name : names ){

       if( buildDatasetModelItem( model, row, name) ) row++;
    }

    return model;

    }
    catch(std::exception& ex){
        QMessageBox::critical(this, tr("Build Datasets Model"), ex.what(), QMessageBox::Ok);
        return nullptr;
    }
}


QStandardItemModel* ProjectViewer::buildGridModelItem(GridType type, QStandardItemModel *model, int row, QString name){

    if( !model ) return model;

    QFileInfo finfo;
    Grid2DBounds bounds;

    try{
        finfo=m_project->getGridFileInfo(type, name);
        bounds=m_project->getGridBounds(type, name);
    }
    catch(std::exception& ex){
        QMessageBox::warning(this, tr("build grids model"), QString("%1 :\n %2").arg(name,ex.what()), QMessageBox::Ok);
        return nullptr;
    }

    int column=0;
    model->setItem(row, column++, new QStandardItem(name) );
    model->setItem(row, column++, newAlignedItem(bounds.i1(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(bounds.i2(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(bounds.j1(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(bounds.j2(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(finfo.size()/1024, Qt::AlignRight));
    //model->setItem(row, column++, newAlignedItem(finfo.lastModified().toString(tr("dd.MM.yyyy hh:mm:ss")), Qt::AlignRight));
    model->setItem(row, column++, newAlignedItem(finfo.lastModified().toString(tr("dd.MM.yyyy hh:mm:ss")), Qt::AlignRight));

    return model;
}

QStandardItemModel* ProjectViewer::buildGridsModel(GridType type){

    QStringList labels;
    labels<<"Name"<<"Min IL"<<"Max IL"<<"Min XL"<<"Max XL"<<"Size(KB)"<<"Last Modified";

    auto names=m_project->gridList( type );
    QStandardItemModel* model=new QStandardItemModel(names.size(), labels.size(), this);
    model->setHorizontalHeaderLabels(labels);

    int row=0;
    for( auto name : names ){

        if( buildGridModelItem( type, model, row, name)) row++;
    }

    return model;
}


QStandardItemModel* ProjectViewer::buildVolumeModelItem(QStandardItemModel *model, int row, QString name){

    if( !model ) return model;

    QFileInfo finfo;
    Grid3DBounds bounds;
    Domain domain;
    VolumeType type;

    try{
        finfo=m_project->getVolumeFileInfo(name);
        bounds=m_project->getVolumeBounds(name, &domain, &type);
    }
    catch(std::exception& ex){
        QMessageBox::warning(this, tr("build volumes model"), QString("%1 :\n %2").arg(name,ex.what()), QMessageBox::Ok);
        return nullptr;
    }

    int column=0;
    model->setItem(row, column++, new QStandardItem(name) );
    model->setItem(row, column++, newAlignedItem(toQString(domain), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(toQString(type), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(bounds.i1(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(bounds.i2(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(bounds.j1(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(bounds.j2(), Qt::AlignHCenter));
    model->setItem(row, column++, newAlignedItem(1000*bounds.ft(), Qt::AlignRight));        // sec -> msec
    model->setItem(row, column++, newAlignedItem(1000*bounds.lt(), Qt::AlignRight));       // sec -> msec
    model->setItem(row, column++, newAlignedItem(1000*bounds.dt(), Qt::AlignRight));       // sec -> msec
    model->setItem(row, column++, newAlignedItem(finfo.size()/1024/1024, Qt::AlignRight)); // mb
    //model->setItem(row, column++, newAlignedItem(finfo.lastModified().toString(tr("dd.MM.yyyy hh:mm:ss")), Qt::AlignRight));
    model->setItem(row, column++, newAlignedItem(finfo.lastModified(), Qt::AlignRight));

    return model;
}

QStandardItemModel* ProjectViewer::buildVolumesModel(){

    QStringList labels;
    labels<<"Name"<<"Domain"<<"Type"<<"Min IL"<<"Max IL"<<"Min XL"<<"Max XL"<<"Start"<<"End"<<"Interval"<<"Size(MB)"<<"Last Modified";

    auto names=m_project->volumeList();
    QStandardItemModel* model=new QStandardItemModel(names.size(), labels.size(), this);
    model->setHorizontalHeaderLabels(labels);

    int row=0;
    for( auto name : names ){

        if( buildVolumeModelItem(model, row, name)) row++;
    }

    return model;
}


QStandardItemModel* ProjectViewer::buildWellModelItem(QStandardItemModel* model, int row, QString wid){

    if( !model ) return model;

    WellInfo info;
    QFileInfo finfo;

    try{
        info=m_project->getWellInfo(wid);
        finfo=m_project->getWellFileInfo(wid);
    }
    catch(std::exception& ex){
        QMessageBox::warning(this, tr("build wells model"), QString("%1 :\n %2").arg(wid,ex.what()), QMessageBox::Ok);
        return nullptr;
    }

    int column=0;

    model->setItem(row, column++, new QStandardItem(info.uwi()) );
    model->setItem(row, column++, new QStandardItem(info.name()) );
    model->setItem(row, column++, newAlignedItem(info.x()));
    model->setItem(row, column++, newAlignedItem(info.y()));
    model->setItem(row, column++, newAlignedItem(info.z()));

    QTransform xy_to_ilxl, ilxl_to_xy;
    m_project->geometry().computeTransforms(xy_to_ilxl, ilxl_to_xy);
    auto ilxl=xy_to_ilxl.map(QPointF(info.x(), info.y()));
    model->setItem(row, column++, newAlignedItem(std::round(ilxl.x())));
    model->setItem(row, column++, newAlignedItem(std::round(ilxl.y())));

    auto logs=m_project->logList(wid);
    logs.sort();    // display in alphabetic order
    auto logstr=logs.join(';');
    QStandardItem* logitem=newAlignedItem( logstr, Qt::AlignLeft );
    if( logitem ) logitem->setData(logs, Qt::UserRole+1);
    model->setItem(row, column++, logitem);
    // works with logsitemdelegate nut this requires edit enabled

    //model->setItem(row, column++, new QStandardItem(finfo.lastModified().toString(tr("dd.MM.yyyy hh:mm:ss"))));
    model->setItem(row, column++, newAlignedItem(finfo.lastModified()));

    return model;
}

QStandardItemModel* ProjectViewer::buildWellsModel(){

    QStringList labels;
    labels<<"UWI"<<"Name"<<"X"<<"Y"<<"Z"<<"IL*"<<"XL*"<<"Logs"<<"Last Modified";

    auto wids=m_project->wellList();
    QStandardItemModel* model=new QStandardItemModel(wids.size(), labels.size(), this);
    model->setHorizontalHeaderLabels(labels);

    int row=0;
    for( auto wid : wids ){

        if( buildWellModelItem( model, row, wid) ) row++;
    }

    return model;
}

QStandardItemModel* ProjectViewer::buildTableModelItem(QStandardItemModel* model, int row, QString name){

    if( !model ) return model;

    QString key1;
    QString key2;
    bool multi;
    int size;

    try{
        auto info=m_project->getTableInfo(name);
        std::tie(key1,key2,multi,size)=info;
    }catch(std::exception& ex){
        QMessageBox::warning(this, tr("build picks model"), QString("%1 :\n %2").arg(name,ex.what()), QMessageBox::Ok);
        return nullptr;
    }

    int column=0;

    model->setItem(row, column++, new QStandardItem(name) );
    model->setItem(row, column++, newAlignedItem(key1, Qt::AlignRight));
    model->setItem(row, column++, newAlignedItem(key2, Qt::AlignRight));
    model->setItem(row, column++, newAlignedItem( (multi) ? "multi" : "single" , Qt::AlignRight));
    model->setItem(row, column++, newAlignedItem(size, Qt::AlignRight));
    auto finfo=m_project->getTableFileInfo(name);
    model->setItem(row, column++, newAlignedItem(finfo.lastModified()));

    return model;
}

QStandardItemModel* ProjectViewer::buildTablesModel(){

    QStringList labels;
    labels<<"Name"<<"Key1"<<"Key2"<<"Mode"<<"Size"<<"Last Modified";

    auto names=m_project->tableList();
    QStandardItemModel* model=new QStandardItemModel(names.size(), labels.size(), this);
    model->setHorizontalHeaderLabels(labels);

    int row=0;
    for( auto name :names ){

        if( buildTableModelItem( model, row, name) ) row++;
    }

    return model;
}

void ProjectViewer::updateDatasetsView(){

    if( !m_project ){
        ui->datasetsView->setModel(0);
        return;
    }

    ui->datasetsView->setModel(buildDatesetsModel() );
    ui->datasetsView->setEditTriggers(QAbstractItemView::NoEditTriggers);   // read only
    ui->datasetsView->resizeColumnsToContents();  // XXX 1.3.18
    QHeaderView *headerView = ui->datasetsView->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Fixed );
    headerView->setSectionResizeMode(0, QHeaderView::Stretch);  // first column uses all available space
}

void ProjectViewer::updateDatasetsView(QString name){

    auto model = dynamic_cast<QStandardItemModel*>(ui->datasetsView->model());
    if( !model) return;

    auto items=model->findItems(name);
    if( items.empty()) return;

    int row=items.front()->row();
    buildDatasetModelItem(model, row, name);
}

void ProjectViewer::updateGridsView(GridType type){

    QTableView* view=nullptr;
    switch( type ){
    case GridType::Horizon: view=ui->horizonsView; break;
    case GridType::Attribute: view=ui->attributesView; break;
    case GridType::Other: view=ui->gridsView; break;
    }
    if( !view ) return;

    if( !m_project ){
        view->setModel(0);
        return;
    }

    view->setModel(buildGridsModel(type) );
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);   // read only
    view->resizeColumnsToContents();
    auto headerView = view->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Fixed );
    headerView->setSectionResizeMode(0, QHeaderView::Stretch);  // first column uses all available space
}

void ProjectViewer::updateGridsView(GridType type, QString name){

    QTableView* view=nullptr;
    switch( type ){
    case GridType::Horizon: view=ui->horizonsView; break;
    case GridType::Attribute: view=ui->attributesView; break;
    case GridType::Other: view=ui->gridsView; break;
    }
    if( !view ) return;

    QStandardItemModel* model=dynamic_cast<QStandardItemModel*>(view->model());
    if( !model )  return;

    auto items=model->findItems(name);
    if( items.empty()) return;

    int row=items.front()->row();
    buildGridModelItem(type, model, row, name);
}

void ProjectViewer::updateVolumesView(){

    if( !m_project ){
        ui->volumesView->setModel(0);
        return;
    }

    ui->volumesView->setModel(buildVolumesModel() );
    ui->volumesView->setEditTriggers(QAbstractItemView::NoEditTriggers);   // read only
    ui->volumesView->resizeColumnsToContents();
    auto headerView = ui->gridsView->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Fixed );
    headerView->setSectionResizeMode(0, QHeaderView::Stretch);  // first column uses all available space
}

void ProjectViewer::updateVolumesView(QString name){

    auto model = dynamic_cast<QStandardItemModel*>(ui->volumesView->model());
    if( !model) return;

    auto items=model->findItems(name);
    if( items.empty()) return;

    int row=items.front()->row();
    buildVolumeModelItem(model, row, name);
}


void ProjectViewer::updateWellsView(){

    if( !m_project ){
        ui->wellsView->setModel(0);
        return;
    }

    ui->wellsView->setModel(buildWellsModel() );
    ui->wellsView->setEditTriggers(QAbstractItemView::NoEditTriggers);   // read only, for LogItemDelegate use AllEditTriggers
    ui->wellsView->resizeColumnsToContents();
    auto headerView = ui->wellsView->horizontalHeader();
    //headerView->setSectionResizeMode(QHeaderView::Fixed );
    headerView->setSectionResizeMode(7, QHeaderView::Stretch);  // logs column uses all available space
}


void ProjectViewer::updateWellsView(QString name){

    auto model = dynamic_cast<QStandardItemModel*>(ui->wellsView->model());
    if( !model) return;

    auto items=model->findItems(name);
    if( items.empty()) return;

    int row=items.front()->row();
    buildWellModelItem(model, row, name);
}


void ProjectViewer::updateTablesView(){

    if( !m_project ){
        ui->tablesView->setModel(0);
        return;
    }

    auto model=buildTablesModel();
    ui->tablesView->setModel(model);
    ui->tablesView->setEditTriggers(QAbstractItemView::NoEditTriggers);   // read only, for LogItemDelegate use AllEditTriggers
    ui->tablesView->resizeColumnsToContents();
    auto headerView = ui->tablesView->horizontalHeader();
    //headerView->setSectionResizeMode(QHeaderView::Fixed );
    headerView->setSectionResizeMode(0, QHeaderView::Stretch);
}


void ProjectViewer::updateTablesView(QString name){

    auto model = dynamic_cast<QStandardItemModel*>(ui->tablesView->model());
    if( !model) return;

    auto items=model->findItems(name);
    if( items.empty()) return;

    int row=items.front()->row();
    buildTableModelItem(model, row, name);
}


void ProjectViewer::updateProjectViews(){

    updateDatasetsView();
    updateGridsView(GridType::Horizon);
    updateGridsView(GridType::Attribute);
    updateGridsView(GridType::Other);
    updateVolumesView();
    updateWellsView();
    updateTablesView();
}

template<class PROCESS>
void ProjectViewer::runProcess( const QMap<QString, QString>& params)
{

    Q_ASSERT(m_project);

    try{

    auto process=new PROCESS(m_project, this);
    QDateTime start=QDateTime::currentDateTime();

    QString name=process->name();

    statusBar()->showMessage(QString("Started %1 at %2").arg(name).arg(start.time().toString() ) );
    auto res= process->init(params);
    if( res!=ProjectProcess::ResultCode::Ok){

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
        std::cout<<"ERROR"<<std::endl<<std::flush;
        QMessageBox::critical(this, process->name(), process->lastError());
    }
    else if( code== ProjectProcess::ResultCode::Canceled){
        std::cout<<"CANCELED"<<std::endl<<std::flush;
        QMessageBox::information(this, process->name(), "Process was canceled by user.");
    }

    process->deleteLater();
    progress->deleteLater();

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


/*
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
*/

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
    ui->actionImport_Attached_Datasets->setEnabled(isProject);
    ui->actionImportHorizon->setEnabled( isProject );
    ui->actionImportAttributeGrid->setEnabled(isProject);
    ui->actionImportOtherGrid->setEnabled(isProject);
    ui->actionImportVolume->setEnabled(isProject);
    ui->actionImportLogs->setEnabled(isProject);
    ui->action_Logs_Bulk_Mode->setEnabled(isProject);
    ui->actionWellpath->setEnabled(isProject);
    //ui->actionWellpaths_Bulk_Mode->setEnabled(isProject);
    ui->actionWell_Markers->setEnabled(isProject);
    ui->actionImportTable->setEnabled(isProject);
    ui->actionExportHorizon->setEnabled(isProject);
    ui->actionExportOtherGrid->setEnabled(isProject);
    ui->actionExportAttributeGrid->setEnabled(isProject);
    ui->actionExportVolume->setEnabled(isProject);
    ui->actionExportSeismic->setEnabled(isProject);
    ui->actionExportLog->setEnabled(isProject);
    ui->actionExport_Logs->setEnabled(isProject);
    ui->action_ExportTable->setEnabled(isProject);
    ui->actionExportProject->setEnabled(isProject);
    ui->actionCloseProject->setEnabled(isProject);

    ui->action_Geometry->setEnabled(isProject);
    ui->actionAxis_Orientation->setEnabled(isProject);
    ui->actionDisplay_Map->setEnabled(isProject);

    ui->action_Offset_Stack->setEnabled(isProject);
    ui->actionOffset_Computation->setEnabled(isProject);
    ui->actionStack_To_Gather->setEnabled(isProject);

    ui->actionReplace_Bad_Traces->setEnabled(isProject);
    ui->actionCrop_Dataset->setEnabled(isProject);
    ui->actionCreateTimeslice->setEnabled(isProject);
    ui->actionCreate_New_Grid->setEnabled(isProject);
    ui->actionCrop_Grid->setEnabled(isProject);
    ui->actionHorizon_Amplitudes->setEnabled(isProject);
    ui->actionHorizon_Frequencies->setEnabled(isProject);
    ui->actionHorizon_Semblance->setEnabled(isProject);
    ui->actionHorizon_Curvature->setEnabled(isProject);
    ui->actionCompute_Intercept_Gradient->setEnabled(isProject);
    ui->actionTrend_Based_Attribute_Grids->setEnabled(isProject);
    ui->actionSecondary_Attribute_Grids->setEnabled(isProject);
    ui->actionConvert_Grid->setEnabled(isProject);
    ui->actionSmooth_Grid->setEnabled(isProject);
    ui->actionGrid_Math->setEnabled(isProject);
    ui->actionRun_Grid_User_Script->setEnabled(isProject);

    ui->actionCreate_New_Volume->setEnabled(isProject);
    ui->action_Crop_Volume->setEnabled(isProject);
    ui->actionMerge_Volumes->setEnabled(isProject);
    ui->actionVolume_Math->setEnabled(isProject);
    ui->actionFlatten_Volume->setEnabled(isProject);
    ui->actionVelocity_Conversion->setEnabled(isProject);
    ui->actionVolume_Time_Depth_Conversion->setEnabled(isProject);
    ui->actionUnflatten_Volume->setEnabled(isProject);
    ui->actionExtract_Timeslice->setEnabled(isProject);
    ui->actionExtract_Dataset->setEnabled(isProject);
    ui->actionVolume_Amplitudes->setEnabled(isProject);
    ui->actionVolume_Dip->setEnabled(isProject);
    ui->actionVolume_Frequencies->setEnabled(isProject);
    ui->actionVolume_Semblance->setEnabled(isProject);
    ui->actionVolume_Similarity->setEnabled(isProject);
    ui->actionVolume_Variance->setEnabled(isProject);
    ui->actionVolume_Statistics->setEnabled(isProject);
    ui->actionCompute_Intercept_and_Gradient_Volumes->setEnabled(isProject);
    ui->actionVolume_Curvature->setEnabled(isProject);
    ui->actionInstantaneous_Attribute_Volumes->setEnabled(isProject);
    ui->actionTrend_Based_Attribute_Volumes->setEnabled(isProject);
    ui->actionSecondary_Attribute_Volumes->setEnabled(isProject);
    ui->actionPunch_Out_Volume->setEnabled(isProject);
    ui->actionVolume_Circle_Detection->setEnabled(isProject);
   // ui->actionNN_Volume_Trainer->setEnabled(isProject);
   // ui->actionNN_Volume_Classification->setEnabled(isProject);
    ui->actionRun_Volume_Script->setEnabled(isProject);

    ui->action_Smooth_Log->setEnabled(isProject);
    ui->actionLog_Math->setEnabled(isProject);
    ui->actionLog_Integration->setEnabled(isProject);
    ui->actionCopy_Log->setEnabled(isProject);
   // ui->actionNN_Log_old->setEnabled(isProject);
    ui->actionTrain_NN_Log->setEnabled(isProject);
    ui->actionNN_Log_Interpolation->setEnabled(isProject);
    ui->actionVShale_Computation->setEnabled(isProject);
    ui->actionCastagna_VS_Computation->setEnabled(isProject);
    ui->actionRock_Properties_Computation->setEnabled(isProject);
    ui->actionTops_from_Horizon->setEnabled(isProject);
    ui->actionRun_Log_Script->setEnabled(isProject);
   // ui->actionBuild_Volume->setEnabled(isProject);
    ui->actionEdit_Tops->setEnabled(isProject);

    ui->actionConvert_Picks_To_Horizon->setEnabled(isProject);

    ui->actionFD_CDP_Acoustic->setEnabled(isProject);

    ui->actionCrossplot_Grids->setEnabled(isProject);
    ui->actionCrossplot_Volumes->setEnabled(isProject);
    ui->actionCrossplot_Logs->setEnabled(isProject);
    ui->actionAmplitude_vs_Offset_Plot->setEnabled(isProject);
    ui->actionFrequency_Spectrum_Plot->setEnabled(isProject);
    ui->actionLog_TVD_Plot->setEnabled(isProject);
    ui->actionColor_Composite_Grids->setEnabled(isProject);
    ui->action_3D_Viewer->setEnabled(isProject);
    ui->actionOpen_2DViewer->setEnabled(isProject);
    ui->actionOpen_Slice_Viewer->setEnabled(isProject);
    ui->actionOpen_3D_Viewer->setEnabled(isProject);
    ui->actionOpen_Log_Viewer->setEnabled(isProject);

    ui->actionVolume_IO_Test->setEnabled(isProject);
    ui->actionIntercept_And_Gradient_Volume_old->setEnabled(isProject);
}





void ProjectViewer::on_actionVolume_IO_Test_triggered()
{
    Q_ASSERT( m_project );

    VolumeIOTestDialog dlg;
    dlg.setInputs(m_project->volumeList());

    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<VolumeIOTestProcess>( params );
}

void ProjectViewer::on_actionIntercept_And_Gradient_Volume_old_triggered()
{
    Q_ASSERT(m_project);
    InterceptGradientVolumeDialog_2 dlg;
    dlg.setDatasets( m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Prestack));
    dlg.setReservedVolumes(m_project->volumeList());
    if( dlg.exec()!=QDialog::Accepted) return;
    QMap<QString,QString> params=dlg.params();

    runProcess<InterceptGradientVolumeProcess_2>( params );
}







void ProjectViewer::on_actionOpen_test_log_viewer_triggered()
{
    auto viewer=new logviewer::MultiLogViewer();
    viewer->setAttribute(Qt::WA_DeleteOnClose);
    viewer->show();
}



