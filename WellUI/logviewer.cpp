#include "logviewer.h"
#include "ui_logviewer.h"

#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include<QStandardItem>
#include<QStandardItemModel>
#include<QColorDialog>
#include<QActionGroup>
#include<QInputDialog>
#include<QLabel>
#include<QBoxLayout>
#include<QGridLayout>
#include<QResizeEvent>
#include<QScrollBar>
#include<QApplication>
#include<QIcon>
#include<QPushButton>
#include<QStyle>
#include<QHBoxLayout>
#include<cmath>
#include<selectionpoint.h>
#include <axisconfigdialog.h>
#include <multiitemselectiondialog.h>
#include <selectwellsbylogdialog.h>
#include <QToolBar>
#include <QComboBox>
#include <QSpinBox>

#include<vector>
#include<tuple>
#include<algorithm>
#include<wellmarkers.h>
#include<topsdbmanager.h>


const int MINIMUM_TRACK_WIDTH=200;
const int MINIMUM_TRACK_HEIGHT=500;
const int VSCALE_WIDTH=80;
const int HSCALE_HEIGHT=80;


namespace{
QMap<LogViewer::FlattenMode,QString> flattenModeLookup{
    {LogViewer::FlattenMode::FLATTEN_NONE, "NO Flattening"},
    {LogViewer::FlattenMode::FLATTEN_HORIZON, "Flatten on Horizon"},
    {LogViewer::FlattenMode::FLATTEN_TOP,"Flatten on Top"}
};

QString toQString(LogViewer::FlattenMode m){
    return flattenModeLookup.value(m, "NO Flattening");
}

LogViewer::FlattenMode toFlattenMode(const QString& str){
    return flattenModeLookup.key(str, LogViewer::FlattenMode::FLATTEN_NONE);
}

QList<LogViewer::FlattenMode> flattenModes(){
    return flattenModeLookup.keys();
}

QStringList flattenModeNames(){
    return flattenModeLookup.values();
}

}

namespace{
QMap<LogViewer::PickMode, QString> pickModeLookup{
    {LogViewer::PickMode::PICK_LOG, "Pick Log"},
    {LogViewer::PickMode::PICK_TOPS, "Pick Tops"}

};

QString toQString(LogViewer::PickMode m){
    return pickModeLookup.value(m, "Pick Log");
}

LogViewer::PickMode toPickMode(const QString& str){
    return pickModeLookup.key(str, LogViewer::PickMode::PICK_LOG);
}

QList<LogViewer::PickMode> pickModes(){
    return pickModeLookup.keys();
}

QStringList pickModeNames(){
    return pickModeLookup.values();
}

}

LogViewer::LogViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::LogViewer)
{
    ui->setupUi(this);

    m_tracksAreaWidget=new QWidget(this);
    ui->tracksArea->setWidget(m_tracksAreaWidget);

    setAttribute( Qt::WA_DeleteOnClose);

    connect( ui->action_Receive , SIGNAL(toggled(bool)), this, SLOT(setReceptionEnabled(bool)) );
    connect( ui->action_Send, SIGNAL(toggled(bool)), this, SLOT(setBroadcastEnabled(bool)) );

    setupMouseModes();
    setupZModeToolbar();
    setupFilterToolbar();
    setupFlattenToolbar();
    setupPickingToolbar();
}

LogViewer::~LogViewer()
{
    delete ui;
}

void LogViewer::setupMouseModes(){

    DynamicMouseModeSelector* mm=new DynamicMouseModeSelector(this);
    mm->addMode(MouseMode::Explore);
    mm->addMode(MouseMode::Zoom);
    //mm->addMode(MouseMode::Select);
    mm->addMode(MouseMode::Pick);
    mm->addMode(MouseMode::DeletePick);
    ui->mouseToolBar->addWidget( mm);
    m_mousemodeSelector=mm;
}

void LogViewer::setupZModeToolbar(){

    QToolBar* toolBar=new QToolBar( "Z-Mode", this);

    toolBar->addWidget(new QLabel("Z-Axis:", this));
    m_cbZMode=new QComboBox(this);
    m_cbZMode->addItems(zmodeNames());
    m_cbZMode->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    toolBar->addWidget(m_cbZMode);
    addToolBar(toolBar);
    connect( m_cbZMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(setZMode(QString)));
    connect( this, SIGNAL(zmodeChanged(QString)), m_cbZMode, SLOT(setCurrentText(QString)));
}

void LogViewer::setupFlattenToolbar(){

    QToolBar* toolBar=new QToolBar( "Flatten", this);

    m_cbFlattenMode=new QComboBox(this);
    m_cbFlattenMode->addItems(flattenModeNames());
    m_cbFlattenMode->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    toolBar->addWidget(m_cbFlattenMode);
    connect( m_cbFlattenMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(setFlattenMode(QString)));

    m_cbFlattenSource=new QComboBox(this);
    m_cbFlattenSource->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    toolBar->addWidget(m_cbFlattenSource);
    connect( m_cbFlattenSource, SIGNAL(currentIndexChanged(QString)), this,SLOT(setFlattenSource(QString)));

    addToolBar(toolBar);
}

void LogViewer::setupFilterToolbar(){

    QToolBar* toolBar=new QToolBar("Filter", this);

    toolBar->addWidget(new QLabel("Filter:", this) );
    m_sbFilterLen=new QSpinBox(this);
    m_sbFilterLen->setRange(0,1000);
    m_sbFilterLen->setValue(0);
    m_sbFilterLen->setSingleStep(10);
    toolBar->addWidget(m_sbFilterLen);

    addToolBar(toolBar);

    connect( m_sbFilterLen, SIGNAL(valueChanged(int)), this, SLOT(setFilterLen(int)) );
}

void LogViewer::setupPickingToolbar(){

    QToolBar* toolBar=new QToolBar( "Picking", this);
    m_cbPickMode=new QComboBox(this);
    m_cbPickMode->addItems(pickModeNames());
    m_cbPickMode->setCurrentText(toQString(PickMode::PICK_LOG));
    m_cbPickMode->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    toolBar->addWidget(m_cbPickMode);
    connect( m_cbPickMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(setPickMode(QString)));
    addToolBar(toolBar);
}


void LogViewer::receivePoint(SelectionPoint, int code){
    //QMessageBox::information(this,tr("Receive"), tr("Receive point"), QMessageBox::Ok);
}

 void LogViewer::receivePoints(QVector<SelectionPoint> points, int code){

     if( code==CODE_SINGLE_POINTS ){
         QVector<qreal> zhs;
         zhs.reserve(points.size());
         for( auto p : points ){
             zhs.push_back(1000 * p.time);
         }
         for( auto tv : m_trackViews ){
             tv->setHighlighted(zhs);
         }
     }
 }

void LogViewer::resizeEvent(QResizeEvent *){
    ui->centralwidget->layout()->update();
}

void LogViewer::setProject( AVOProject* p){

    if( p==m_project ) return;

    m_project=p;
}

/*
void LogViewer::setUWI(const QString & s){
    m_uwi=s;
}
*/

void LogViewer::addLog(QString uwi, QString name){

    if( !m_project->existsWell(uwi)){
        QMessageBox::critical(this, tr("Add Log"), QString("Well \"%1\" does not exist!").arg(uwi), QMessageBox::Ok);
        return;
    }
    auto wi=m_project->getWellInfo(uwi);

    auto log=m_project->loadLog(uwi, name);
    if( !log ){
        QMessageBox::critical(this, tr("Add Log"), QString("Loading log \"%1 - %2\" failed!").arg(uwi).arg(name));
        return;
    }

    auto wp=m_project->loadWellPath(uwi);
    if( !wp){
        // silently do this for now:
        //QMessageBox::warning(this, tr("Add Log"), QString("No Wellpath for well \"%1\"\n,Using generic path").arg(uwi));
        wp=std::make_shared<WellPath>(WellPath::fromLog(*log));
    }

    auto wm=m_project->loadWellMarkersByWell(uwi);

    addLog( wi, wp, log, wm);
}

void LogViewer::addLog( WellInfo wi, std::shared_ptr<WellPath> p, std::shared_ptr<Log> l, std::shared_ptr<WellMarkers> wm){

    if( !l) return;

    m_wellInfos.push_back(wi);
    m_markers.push_back(wm);

    if( !m_zAxis){
        m_zAxis=new Axis(Axis::Type::Linear, l->z0(), l->lz(), this );
        m_zAxis->setViewRange(m_zAxis->min(), m_zAxis->max());
        //m_zAxis->setLabelSubTicks(true);
        connect(m_zAxis, SIGNAL(cursorPositionChanged(qreal)), this, SLOT( onZCursorChanged(qreal)));
    }
    else{
        qreal min=std::min(m_zAxis->min(), l->z0());
        qreal max=std::max(m_zAxis->max(), l->lz());
        m_zAxis->setRange(min,max);
        m_zAxis->setViewRange(min,max);
    }

    TrackLabel* tl=new TrackLabel(trackLabelText(wi, *l), this);
    tl->setFont(m_labelFont);
    tl->setWordWrap(true);
    connect( tl, SIGNAL(moved(QPoint)), this, SLOT(onTrackLabelMoved(QPoint)) );
    m_trackLabels.push_back(tl);

    QPushButton* tb=new QPushButton(QIcon(QPixmap(":images/x-16x16.png")),"",this);
    m_closeButtons.push_back(tb);
    connect( tb, SIGNAL(clicked()), this, SLOT(onCloseTrack()));

    tb=new QPushButton(QIcon(QPixmap(":images/shrink-x-24x24.png")),"",this);
    m_shrinkButtons.push_back(tb);
    connect( tb, SIGNAL(clicked()), this, SLOT(onShrinkTrack()));

    tb=new QPushButton(QIcon(QPixmap(":images/grow-x-24x24.png")),"",this);
    m_growButtons.push_back(tb);
    connect( tb, SIGNAL(clicked()), this, SLOT(onGrowTrack()));

    tb=new QPushButton(QIcon(QPixmap(":images/plus_16x16.png")),"",this);
    m_addButtons.push_back(tb);
    connect( tb, SIGNAL(clicked()), this, SLOT(onAddTrackLog()));

    tb=new QPushButton(QIcon(QPixmap(":images/minus_16x16.png")),"",this);
    m_removeButtons.push_back(tb);
    connect( tb, SIGNAL(clicked()), this, SLOT(onRemoveTrackLog()));

    tb=new QPushButton(QIcon(QPixmap(":images/colors-16x16.png")),"",this);
    m_colorButtons.push_back(tb);
    connect( tb, SIGNAL(clicked()), this, SLOT(onTrackColors()));

    QLabel* vl=new QLabel(QString(), this);
    vl->setFont(m_trackValueFont);
    m_trackValueLabels.push_back(vl);

    TrackView* tv=new TrackView(this);
    auto xAxis=tv->xAxis();
    auto range=l->range();
    xAxis->setRange(range.first, range.second);
    xAxis->setViewRange(range.first, range.second);
    tv->setZAxis(m_zAxis);
    tv->setZoomMode(AxisView::ZOOMMODE::BOTH); // zooming on z-axis enabled
    tv->setCursorMode(AxisView::CURSORMODE::BOTH);  // show x and z cursor
    tv->setXMesh(true);
    tv->setZMesh(true);
    updateTrackPickMode(tv);
    m_trackViews.push_back(tv);
    tv->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( tv, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(runTrackViewContextMenu(QPoint)));
    tv->addLog(l);
    tv->setWellPath(p);
    tv->setZMode(m_zmode);
    connect( this, SIGNAL(zmodeChanged(ZMode)), tv, SLOT(setZMode(ZMode)) );

    // mouse modes
    if(m_mousemodeSelector){
        connect( m_mousemodeSelector, SIGNAL(modeChanged(MouseMode)), tv, SLOT(setMouseMode(MouseMode)));
        tv->setMouseMode(m_mousemodeSelector->mode());
    }

    tv->setFilterLen(m_filterLen);
    connect( this, SIGNAL(filterLenChanged(int)), tv, SLOT(setFilterLen(int)) );

    // adjust range to show at least 2 tick labels
    tv->xAxis()->adjustRange();

    HScaleView* hv=new HScaleView(this);
    hv->setXAxis(tv->xAxis());
    hv->setZoomMode(AxisView::ZOOMMODE::X);
    hv->setCursorMode(AxisView::CURSORMODE::X);
    hv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_trackScaleViews.push_back(hv);

    connect( hv->horizontalScrollBar(), SIGNAL(valueChanged(int)), tv->horizontalScrollBar(), SLOT(setValue(int)) );
    connect( tv->horizontalScrollBar(), SIGNAL(valueChanged(int)), hv->horizontalScrollBar(), SLOT(setValue(int)) );
    connect( hv->xAxis(), SIGNAL(cursorPositionChanged(qreal)), this, SLOT(onXCursorChanged(qreal)) );

    if( !m_vscaleView ){

        m_vscaleView = new VScaleView(this);
        m_vscaleView->setZAxis(m_zAxis);
        m_vscaleView->setZoomMode(AxisView::ZOOMMODE::Z); // zooming on z-axis enabled
        m_vscaleView->setCursorMode(AxisView::CURSORMODE::Z);  // show z cursor position

        m_vscaleView->setFixedWidth(VSCALE_WIDTH);
   }

    if( !m_verticalScrollBar){

        m_verticalScrollBar=new QScrollBar(Qt::Vertical, this);
        m_verticalScrollBar->setRange(tv->verticalScrollBar()->minimum(), tv->verticalScrollBar()->maximum());
        m_verticalScrollBar->setValue(tv->verticalScrollBar()->value());
        connect(tv->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), m_verticalScrollBar, SLOT(setRange(int,int)) );
        connect(m_vscaleView->verticalScrollBar(), SIGNAL(valueChanged(int)), m_verticalScrollBar, SLOT(setValue(int)) );
        connect(m_verticalScrollBar, SIGNAL(valueChanged(int)), m_vscaleView->verticalScrollBar(), SLOT(setValue(int)) );
    }

   connect(tv->verticalScrollBar(), SIGNAL(valueChanged(int)), m_verticalScrollBar, SLOT(setValue(int)) );
   connect(m_verticalScrollBar, SIGNAL(valueChanged(int)), tv->verticalScrollBar(), SLOT(setValue(int)) );


   // setup geometry

   tl->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
   tl->setMinimumWidth(MINIMUM_TRACK_WIDTH);

   tv->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
   tv->setMinimumWidth(MINIMUM_TRACK_WIDTH);
   tv->setMinimumHeight(MINIMUM_TRACK_HEIGHT);

   hv->setMinimumWidth(MINIMUM_TRACK_WIDTH);
   hv->setFixedHeight(HSCALE_HEIGHT);

   vl->setMinimumWidth(MINIMUM_TRACK_WIDTH);


   layoutLogs();

   updateZAxis();
}


void LogViewer::removeLog(int j){

    if( j<0 || j>m_trackViews.size()) return;
    m_wellInfos.remove(j);
    m_markers.remove(j);
    delete m_trackLabels[j]; m_trackLabels.remove(j);
    delete m_addButtons[j];m_addButtons.remove(j);
    delete m_removeButtons[j];m_removeButtons.remove(j);
    delete m_colorButtons[j];m_colorButtons.remove(j);
    delete m_shrinkButtons[j];m_shrinkButtons.remove(j);
    delete m_growButtons[j];m_growButtons.remove(j);
    delete m_closeButtons[j];m_closeButtons.remove(j);
    delete m_trackValueLabels[j]; m_trackValueLabels.remove(j);
    delete m_trackViews[j]; m_trackViews.remove(j);
    delete m_trackScaleViews[j]; m_trackScaleViews.remove(j);
    layoutLogs();
}

void LogViewer::removeLog(QString name){

    // find log and remove it
    // do this in opposite order!!!
    for( int j=m_trackViews.size()-1; j>=0; j-- ){
        if(m_trackViews[j]->log()->name()==name.toStdString()){ // found it, now remove it
           removeLog(j);
        }
    }
}

QString LogViewer::trackLabelText(const WellInfo& wi, const Log& l){

    QString wstr=(m_trackLabelMode==TrackLabelMode::UWI)?wi.uwi():wi.name();
    return QString("%1\n%2 [%3] ").arg(wstr, l.name().c_str(), l.unit().c_str());
}

void LogViewer::updateTrackLabels(){

    for( int i=0; i<m_trackLabels.size(); i++){
        m_trackLabels[i]->setText(trackLabelText(m_wellInfos[i], *m_trackViews[i]->log()));
    }
}

void LogViewer::setZMode( ZMode m){

    if( m==m_zmode) return;

    m_zmode=m;

    updateZAxis();

    emit zmodeChanged(m);
    emit zmodeChanged(toQString(m));
}

void LogViewer::setZMode(QString str){
    auto m=toZMode(str);
    setZMode(m);
}

void LogViewer::setFlattenMode(FlattenMode m){

    if( m==m_flattenMode) return;

    m_flattenMode=static_cast<FlattenMode>(m);

    m_cbFlattenSource->clear();

    switch( m_flattenMode){
        case FLATTEN_HORIZON:{
            auto names=m_project->gridList(GridType::Horizon);
            std::sort(names.begin(), names.end());
            m_cbFlattenSource->addItems(names);
            break;
        }
        case FLATTEN_TOP:{
            auto p=m_project->openTopsDatabase();
            auto names=p->names();
            std::sort(names.begin(), names.end());
            m_cbFlattenSource->addItems(names);
            break;
            }
        default: break;
    }

    updateTrackFlatten();
}

void LogViewer::setFlattenMode(QString str){
    setFlattenMode(toFlattenMode(str));
}

void LogViewer::setFlattenSource(QString name){

    if( m_flattenSource==name) return;
    m_flattenSource=name;
    updateTrackFlatten();
}

void LogViewer::setFilterLen(int l){
    if( l==m_filterLen ) return;
    m_filterLen=l;
    emit filterLenChanged(l);
}

void LogViewer::setTrackLabelMode(int m){
    if( m==m_trackLabelMode) return;
    m_trackLabelMode=m;
    updateTrackLabels();
}

void LogViewer::setPickMode(PickMode pm){
    if(pm==m_PickMode) return;
    m_PickMode=pm;
    for( auto tv : m_trackViews){
        updateTrackPickMode(tv);
    }
}

void LogViewer::setPickMode(QString str){
    setPickMode(toPickMode(str));
}

void LogViewer::layoutLogs(){

    QGridLayout* lo = new QGridLayout;

    lo->addWidget( m_vscaleView, 1, 0);

    for( int i = 0; i<m_trackViews.size(); i++){
        auto c1=new QHBoxLayout;
        //c1->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Ignored));
        c1->addWidget(m_addButtons[i]);
        c1->addWidget(m_removeButtons[i]);
        c1->addWidget(m_colorButtons[i]);
        c1->addWidget(m_shrinkButtons[i]);
        c1->addWidget(m_growButtons[i]);
        c1->addWidget(m_closeButtons[i]);

        auto clo=new QVBoxLayout;
        clo->addWidget( m_trackLabels[i]);
        clo->addLayout(c1);
        lo->addLayout(clo, 0, i+1);

        lo->addWidget( m_trackViews[i], 1, i+1);
        lo->addWidget( m_trackScaleViews[i], 2, i+1);
        lo->addWidget( m_trackValueLabels[i], 3, i+1);
        //lo->setColumnStretch(i+1, 1);
    }

    lo->addWidget(m_verticalScrollBar, 1, 1000);        // rightmost

    lo->setHorizontalSpacing(4);
    lo->setVerticalSpacing(0);

    //delete ui->centralwidget->layout();
    //ui->centralwidget->setLayout(lo);
    delete m_tracksAreaWidget->layout();
    m_tracksAreaWidget->setLayout(lo);

    lo->update();

}

void LogViewer::on_action_Add_Log_triggered()
{
    if( !m_project) return;

    bool ok=false;
    QString uwi=QInputDialog::getItem(this, tr("Add Log"), tr("Select Well:"), m_project->wellList(), 0, false, &ok);
    if( uwi.isEmpty() || !ok) return;

    QStringList avail=m_project->logList(uwi);
    /*
    for( auto l : m_logs ){
        if( avail.contains(l->name().c_str())) avail.removeAll(l->name().c_str());
    }
    */
    std::sort( avail.begin(), avail.end());
    QStringList names = MultiItemSelectionDialog::getItems(this, tr("Add Logs"), tr("Select Logs:"), avail);

    auto wp=m_project->loadWellPath(uwi);
    if( !wp){
        QMessageBox::critical(this, tr("Add Log"), QString("No Wellpath for well \"%1\"").arg(uwi));
        return;
    }

    for( auto name : names ){

        addLog( uwi, name );
    }

}


void LogViewer::on_actionAdd_Logs_by_Log_triggered()
{
    if( !m_project) return;

    SelectWellsByLogDialog dlg;
    dlg.setProject(m_project);
    dlg.setWindowTitle(tr("Select Log/Wells"));
    if( dlg.exec()!=QDialog::Accepted) return;

    auto name=dlg.log();
    for( auto uwi : dlg.wells()){
        addLog(uwi, name);
    }
}


void LogViewer::on_action_Remove_Log_triggered()
{
    QSet<QString> avail;
    for( auto tv : m_trackViews ){
        avail<<tv->log()->name().c_str();
    }
    QStringList names = MultiItemSelectionDialog::getItems(this, tr("Remove Logs"), tr("Select Logs:"), avail.toList());

    for( auto name:names){
        removeLog(name);
    }

    if( m_trackViews.isEmpty()){    // close viewer when last log is removed
        close();
    }
}


void LogViewer::on_actionConfigure_Z_Axis_triggered()
{
    AxisConfigDialog::configAxis( m_zAxis, tr("Configure Z-Axis") );
}

void LogViewer::on_actionConfigure_X_Axis_2_triggered()
{
    QSet<QString> avail;
    for( auto tv : m_trackViews ){
        avail<<tv->log()->name().c_str();
    }

    bool ok=false;
    QString name=QInputDialog::getItem(this, tr("Configure X-Axis"), tr("Select Log:"), avail.toList(), 0, false, &ok);
    if( name.isEmpty() || !ok) return;

    Axis* ref_axis=nullptr;
    // find first
    for( auto tv : m_trackViews ){
        if( name == tv->log()->name().c_str()){
            ref_axis=tv->xAxis();
            break;
        }
    }
    if(!ref_axis) return;

    if( !AxisConfigDialog::configAxis(ref_axis, QString("Configure %1 Axes").arg(name)) ) return;

    for( auto tv : m_trackViews ){
        if( name == tv->log()->name().c_str()){
            auto axis=tv->xAxis();
            if(!axis) continue;
            axis->setName(ref_axis->name());
            axis->setUnit(ref_axis->unit());
            axis->setAutomaticInterval(ref_axis->isAutomatic());
            axis->setViewRange( ref_axis->min(), ref_axis->max());
            axis->setRange( ref_axis->min(), ref_axis->max());
            axis->setType(ref_axis->type());
            axis->setReversed(ref_axis->isReversed());

            axis->setInterval(ref_axis->interval());
            axis->setSubTickCount(ref_axis->subTickCount());
            axis->setLabelSubTicks(ref_axis->labelSubTicks());
            axis->setLabelPrecision(ref_axis->labelPrecision());
        }
    }
}


void LogViewer::on_actionConfigure_Track_Labels_triggered()
{
    bool ok=false;
    QString str=QInputDialog::getItem(this, tr("Configure Track Labels"), tr("Well Info:"), QStringList{"UWI", "Name"}, m_trackLabelMode, false, &ok);
    if( !ok || str.isEmpty()) return;
    int m = (str=="UWI") ? TrackLabelMode::UWI : TrackLabelMode::WELL_NAME;
    setTrackLabelMode(m);
}


void LogViewer::onXCursorChanged(qreal x){
    if(!m_zAxis) return;
    QString msg=QString("x=%1, z=%2").arg(x).arg(m_zAxis->cursorPosition());
    statusBar()->showMessage(msg);
 }

QString colorstr(QColor c){
    return QString().sprintf("#%02x%02x%02x", c.red(), c.green(), c.blue());
}

void LogViewer::onZCursorChanged(qreal z){

    for( int i=0; i<m_trackViews.size(); i++){

        auto tv=m_trackViews[i];
        auto md=tv->view2log(z);
        QString text;

        for( int j=0; j<tv->count(); j++){
            auto log=tv->log(j);
            auto value=(*log)(md);
            QString valuestr;
            if(value!=log->NULL_VALUE) valuestr=QString::number(value);
            auto color=tv->color(j);

            text+=QString("<span style=\"color:%1;\"> %2: %3</span><br>").arg(colorstr(color), log->name().c_str(), valuestr);
        }

        std::cout<<text.toStdString()<<std::endl<<std::flush;

        m_trackValueLabels[i]->setText(text);
        //m_trackValueLabels[i]->updateGeometry();
    }
}

void LogViewer::onTrackPointSelected(QPointF p){

    static QString prevname;

    auto sourceTV = dynamic_cast<TrackView*>(sender());
    if( !sourceTV) return;

    // find number of this log in log list
    auto idx = m_trackViews.indexOf(sourceTV);

    // convert pick to md
    auto md = sourceTV->view2log(p.y());
    auto uwi = m_wellInfos[idx].uwi();

    auto tmgr=m_project->openTopsDatabase();
    auto avail=tmgr->names();
    std::sort(avail.begin(), avail.end());
    bool ok=false;
    int previdx=avail.indexOf(prevname);
    auto name=QInputDialog::getItem( this, "Pick Top", "Select Top:", avail, previdx, false, &ok);
    if( name.isEmpty() || !ok ) return;
    prevname=name;

    tmgr->set( WellMarker( name, uwi, md) );

    m_markers[idx]=m_project->loadWellMarkersByWell(uwi);
    updateTrackTops();
    //sourceTV->refreshScene();       // maybe better have db changed signal connected
}

void LogViewer::onTrackLabelMoved(QPoint pos){

    std::cout<<"TrackLabel move to x="<<pos.x()<<" y="<<pos.y()<<std::endl<<std::flush;

    // find tracklabel to be moved
    TrackLabel* sourceTL = dynamic_cast<TrackLabel*>(sender());
    if( !sourceTL) return;

    // find number of this log in log list
    auto sourceIndex = m_trackLabels.indexOf(sourceTL); // XXX check for not FOUND

    // find number in list to put it
    pos=sourceTL->mapToParent(pos); // we are parent

    std::cout<<"parent pos: x="<<pos.x()<<" y="<<pos.y()<<std::endl<<std::flush;

    for( int i = 0; i<m_trackLabels.size(); i++){   // find destination trackLabel

        if( m_trackLabels[i]->geometry().contains(pos) ){   // found destination, now swap them

            if( i!=sourceIndex){

                m_trackLabels.move(sourceIndex, i);
                m_trackViews.move(sourceIndex, i);
                m_trackScaleViews.move(sourceIndex, i);
                m_trackValueLabels.move(sourceIndex, i);
                m_addButtons.move(sourceIndex, i);
                m_removeButtons.move(sourceIndex, i);
                m_shrinkButtons.move(sourceIndex, i);
                m_growButtons.move(sourceIndex, i);
                m_closeButtons.move(sourceIndex, i);
                m_colorButtons.move(sourceIndex, i);
                layoutLogs();
            }

            break;  // only 1 replace
        }
    }

}

void LogViewer::onCloseTrack(){
    QPushButton* pb=dynamic_cast<QPushButton*>(sender());
    if( !pb) return;
    auto index=m_closeButtons.indexOf(pb);
    removeLog(index);

    // close viewer when last log is closed
    if( m_trackViews.empty()) close();
}

void LogViewer::onShrinkTrack(){
    QPushButton* pb=dynamic_cast<QPushButton*>(sender());

    if( !pb) return;

    auto index=m_shrinkButtons.indexOf(pb);
    auto tv=m_trackViews[index];
    auto tl=m_trackLabels[index];
    auto hv=m_trackScaleViews[index];
    auto vl=m_trackValueLabels[index];

    int w=std::max(MINIMUM_TRACK_WIDTH, static_cast<int>(tv->width()/1.5));

    tv->setMinimumWidth(w);
    hv->setMinimumWidth(w);
    tl->setMinimumWidth(w);
    vl->setMinimumWidth(w);

    tv->resize(w,tv->height());
    hv->resize(w,hv->height());
    tl->resize(w,tl->height());
    vl->resize(w,vl->height());

    layoutLogs();
}


void LogViewer::onGrowTrack(){
    QPushButton* pb=dynamic_cast<QPushButton*>(sender());
    if( !pb) return;
    auto index=m_growButtons.indexOf(pb);
    auto tv=m_trackViews[index];

    int w=tv->width()*1.5;
    m_trackLabels[index]->setMinimumWidth(w);
    m_trackScaleViews[index]->setMinimumWidth(w);
    m_trackValueLabels[index]->setMinimumWidth(w);
    tv->setMinimumWidth(w);
    /*
    m_trackLabels[index]->setFixedWidth(w);
    m_trackScaleViews[index]->setFixedWidth(w);
    m_trackValueLabels[index]->setFixedWidth(w);
    tv->setFixedWidth(w);
    */
    layoutLogs();
}

void LogViewer::onAddTrackLog(){
    QPushButton* pb=dynamic_cast<QPushButton*>(sender());
    if( !pb) return;
    auto index=m_addButtons.indexOf(pb);
    auto tv=m_trackViews[index];
    auto uwi=m_wellInfos[index].uwi();

    QStringList avail=m_project->logList(uwi);
    std::sort( avail.begin(), avail.end());
    QStringList names = MultiItemSelectionDialog::getItems(this, tr("Add Logs"), tr("Select Logs:"), avail);

    for( auto name : names ){

        auto l=m_project->loadLog(uwi, name);
        if( !l ){
            QMessageBox::critical(this, "Add Logs", "Loading log failed!", QMessageBox::Ok);
            return;
        }

        tv->addLog(l);
    }
}


void LogViewer::onRemoveTrackLog(){
    QPushButton* pb=dynamic_cast<QPushButton*>(sender());
    if( !pb) return;
    auto index=m_removeButtons.indexOf(pb);
    auto tv=m_trackViews[index];
    auto uwi=m_wellInfos[index].uwi();

    QStringList avail;
    for(int i=1; i<tv->count(); i++){       // cannot remove first log, it is reference for track
        auto l=tv->log(i);
        if(l) avail<<l->name().c_str();
    }

    QStringList names = MultiItemSelectionDialog::getItems(this, tr("Remove Logs"), tr("Select Logs:"), avail);
    QVector<int> indices;

    for( auto name : names ){
        indices.push_back(names.indexOf(name));
    }

    // now remove in reverse order
    for( auto it = indices.rbegin(); it!=indices.rend(); it++ ){
        auto index=1 + *it;         // first log always kept
        tv->removeLog(index);
    }
}


void LogViewer::onTrackColors(){

    QPushButton* pb=dynamic_cast<QPushButton*>(sender());
    if( !pb) return;
    auto index=m_colorButtons.indexOf(pb);
    auto tv=m_trackViews[index];

    int l=0;
    if(tv->count()>1){
        QStringList items;
        for(int i=0; i<tv->count(); i++){
            items<<tv->log(i)->name().c_str();
        }
        bool ok=false;
        auto item=QInputDialog::getItem(nullptr, "Set Log Color", "Select Log", items, 0, false, &ok);
        if( !ok || item.isEmpty()) return;
        l=items.indexOf(item);
    }

    auto color=QColorDialog::getColor(tv->color(l), this, "Select Log Color");

    if( !color.isValid()) return;

    tv->setColor(l, color);
}

/*
void LogViewer::on_action_Open_Horizon_triggered()
{
    bool ok=false;
    auto avail=m_project->gridList(GridType::Horizon);
    for(auto h : m_horizons.keys()){    // dont allow selection of already loaded horizons
        avail.removeAll(h);
    }

    QString name=QInputDialog::getItem(this, tr("Open Horizon"), tr("Select Horizon:"), avail, 0, false, &ok);
    if( name.isEmpty() || !ok) return;
    auto hrz=m_project->loadGrid(GridType::Horizon, name);
    if( !hrz ){
        QMessageBox::critical(this, tr("Load Horizon"), QString("Loading horizon \"%1\" failed!").arg(name), QMessageBox::Ok);
        return;
    }
    m_horizons.insert(name, hrz);
    m_cbFlattenHorizon->addItem(name);
    updateTrackHorizons();
}

void LogViewer::on_action_Close_Horizon_triggered()
{
    bool ok=false;
    QString name=QInputDialog::getItem(this, tr("Close Horizon"), tr("Select Horizon:"), m_horizons.keys(), 0, false, &ok);
    if( name.isEmpty() || !ok) return;
    m_horizons.remove(name);
    int idx=m_cbFlattenHorizon->findText(name);
    m_cbFlattenHorizon->removeItem(idx);
    updateTrackHorizons();
}
*/

// NEED TO USE BISECTION
qreal LogViewer::intersect(const Grid2D<float>& h, const WellPath& wp){

    QTransform xy_to_ilxl, ilxl_to_xy;
    m_project->geometry().computeTransforms(xy_to_ilxl, ilxl_to_xy);

    for( int j=1; j<wp.size();j++){
        auto xyz=wp[j];       // use top coords
        auto ilxl=xy_to_ilxl.map(QPointF(xyz[0], xyz[1]));
        auto zh=h.valueAt(ilxl.x(), ilxl.y());
        //if( zh>-xyz[2]) // z==elevation
        if( zh==h.NULL_VALUE) continue;
        auto z1=xyz[2];
        if( zh<z1 ) continue;  // we are still above horizon
        // intersection with horizon is between j and j-1, do linear interpolation
        auto z0=wp[j-1][2];
        auto w1=(zh-z0)/(z1-z0);
        zh=(1.-w1)*z0+w1*z1;
        zh=wp.mdAtTVD(zh); // store md to be conform with logs
        return zh;
    }

    return h.NULL_VALUE;
}

void LogViewer::updateTrackHorizons(){

    QTransform xy_to_ilxl, ilxl_to_xy;
    m_project->geometry().computeTransforms(xy_to_ilxl, ilxl_to_xy);

    for( int i = 0; i<m_trackViews.size(); i++){
        auto tv=m_trackViews[i];
        auto wp=m_trackViews[i]->wellPath();
        tv->removeMarkers(TrackView::Marker::Type::Horizon);

        for( auto name:m_horizons.keys()){
            auto h=m_horizons.value(name);
            // find intersection of wellpath and horizon
            auto zh=intersect(*h, *wp);
            if( zh==h->NULL_VALUE) continue;
            tv->addMarker(TrackView::Marker::Type::Horizon, name, zh);

            /*
            for( int j=1; j<wp->size();j++){
                auto xyz=(*wp)[j];       // use top coords
                auto ilxl=xy_to_ilxl.map(QPointF(xyz[0], xyz[1]));
                auto zh=h->valueAt(ilxl.x(), ilxl.y());
                if( zh>-xyz[2]) // z==elevation
                if( zh==h->NULL_VALUE) continue;
                zh=wp->mdAtTVD(zh); // store md to be conform with logs
                tv->addMarker(name, zh);
            }
            */
        }
    }
    //std::cout<<std::flush;
}

void LogViewer::updateTrackTops(){

    for( int i = 0; i<m_trackViews.size(); i++){
        auto tv=m_trackViews[i];
        tv->removeMarkers(TrackView::Marker::Type::Top);
        if( !m_markers[i]) continue;    // no markers for this track

        for( auto top : *m_markers[i]){

            if( !m_selectedTops.contains(top.name())) continue;
            tv->addMarker(TrackView::Marker::Type::Top, top.name(), top.md());
        }
    }
}

void LogViewer::updateTrackFlatten(){

    switch (m_flattenMode) {

    case FLATTEN_NONE:{
        for( int i = 0; i<m_trackViews.size(); i++){
            m_trackViews[i]->setZShift(0);
        }

        break;
    }

    case FLATTEN_HORIZON:{

        auto flattenHorizon=m_project->loadGrid(GridType::Horizon, m_flattenSource);
        if( !flattenHorizon){
            QMessageBox::critical(this, "Flatten Horizon", "Loading Horizon failed!", QMessageBox::Ok);
            return;
        }

        for( int i = 0; i<m_trackViews.size(); i++){
            auto tv=m_trackViews[i];
            qreal zs=0;
            auto wp=m_trackViews[i]->wellPath();
            auto zh=intersect(*flattenHorizon, *wp);
            if( zh!=flattenHorizon->NULL_VALUE) zs=-zh;
            tv->setZShift(zs);
        }

        break;
    }

    case FLATTEN_TOP:{

        auto tmgr=m_project->openTopsDatabase();
        if( !tmgr ){
            QMessageBox::critical( this, "Flatten Top", "Open tops database failed!", QMessageBox::Ok);
            return;
        }

        for( int i = 0; i<m_trackViews.size(); i++){
            auto uwi=m_wellInfos[i].uwi();
            auto name=m_flattenSource;
            if( !tmgr->exists(uwi, name)) continue;

            auto md=tmgr->value(uwi, name);
            auto tv=m_trackViews[i];
            tv->setZShift(-md);
        }
    }

    }



    updateZAxis();
}

/*
void LogViewer::updateTrackFlatten(){

    for( int i = 0; i<m_trackViews.size(); i++){
        auto tv=m_trackViews[i];
        qreal zs=0;
        if(m_flattenHorizon){
            auto wp=m_trackViews[i]->wellPath();
            auto zh=intersect(*m_flattenHorizon, *wp);
            if( zh!=m_flattenHorizon->NULL_VALUE) zs=-zh;
        }
        tv->setZShift(zs);
    }

    updateZAxis();
}
*/
void LogViewer::updateZAxis(){

    qreal zmin=std::numeric_limits<qreal>::max();
    qreal zmax=std::numeric_limits<qreal>::lowest();
    for( auto tv : m_trackViews){
        auto min=tv->minZ();
        if(min<zmin) zmin=min;
        auto max=tv->maxZ();
        if(max>zmax) zmax=max;
    }

    if( zmin<zmax){
        m_zAxis->setViewRange(zmin,zmax);
        m_zAxis->setRange(zmin,zmax);
    }
}

void LogViewer::on_actionSort_By_Log_triggered()
{
    std::vector<std::tuple<QString,QString, int>> well_log_pos;
    for( int i=0; i<m_trackViews.size(); i++){
        well_log_pos.push_back(std::make_tuple(m_wellInfos[i].uwi(), m_trackViews[i]->log()->name().c_str(), i));
    }
    std::sort( well_log_pos.begin(), well_log_pos.end(),
               [](std::tuple<QString,QString, int> a, std::tuple<QString,QString, int> b){
                    return (std::get<1>(a)<std::get<1>(b))? true : (( std::get<1>(b)<std::get<1>(a))? false : std::get<0>(a) < std::get<0>(b));  });

    auto tmp_trackViews=m_trackViews;
    auto tmp_trackScaleViews=m_trackScaleViews;
    auto tmp_trackLabels=m_trackLabels;
    auto tmp_trackValuelabels=m_trackValueLabels;
    auto tmp_addButtons=m_addButtons;
    auto tmp_removeButtons=m_removeButtons;
    auto tmp_shrinkButtons=m_shrinkButtons;
    auto tmp_growButtons=m_growButtons;
    auto tmp_colorButtons=m_colorButtons;
    auto tmp_closeButtons=m_closeButtons;
    auto tmp_wellInfos=m_wellInfos;
    auto tmp_markers=m_markers;
    for( int i=0; i<m_trackViews.size(); i++){
        int j=std::get<2>(well_log_pos[i]);
        m_trackViews[i]=tmp_trackViews[j];
        m_trackScaleViews[i]=tmp_trackScaleViews[j];
        m_trackLabels[i]=tmp_trackLabels[j];
        m_trackValueLabels[i]=tmp_trackValuelabels[j];
        m_addButtons[i]=tmp_addButtons[j];
        m_removeButtons[i]=tmp_removeButtons[j];
        m_colorButtons[i]=tmp_colorButtons[j];
        m_shrinkButtons[i]=tmp_shrinkButtons[j];
        m_growButtons[i]=tmp_growButtons[j];
        m_closeButtons[i]=tmp_closeButtons[j];
        m_wellInfos[i]=tmp_wellInfos[j];
        m_markers[i]=tmp_markers[j];
    }

      layoutLogs();
}

void LogViewer::on_actionSort_By_Well_triggered()
{
    std::vector<std::tuple<QString,QString, int>> well_log_pos;
    for( int i=0; i<m_trackViews.size(); i++){
        well_log_pos.push_back(std::make_tuple(m_wellInfos[i].uwi(), m_trackViews[i]->log()->name().c_str(), i));
    }
    std::sort( well_log_pos.begin(), well_log_pos.end(),
               [](std::tuple<QString,QString, int> a, std::tuple<QString,QString, int> b){
                    return (std::get<0>(a)<std::get<0>(b))? true : (( std::get<0>(b)<std::get<0>(a))? false : std::get<1>(a) < std::get<1>(b));  });
    auto tmp_trackViews=m_trackViews;
    auto tmp_trackScaleViews=m_trackScaleViews;
    auto tmp_trackLabels=m_trackLabels;
    auto tmp_trackValuelabels=m_trackValueLabels;
    auto tmp_closeButtons=m_closeButtons;
    auto tmp_shrinkButtons=m_shrinkButtons;
    auto tmp_growButtons=m_growButtons;
    auto tmp_addButtons=m_addButtons;
    auto tmp_removeButtons=m_removeButtons;
    auto tmp_colorButtons=m_colorButtons;
    auto tmp_wellInfos=m_wellInfos;
    auto tmp_markers=m_markers;
    for( int i=0; i<m_trackViews.size(); i++){
        int j=std::get<2>(well_log_pos[i]);
        m_trackViews[i]=tmp_trackViews[j];
        m_trackScaleViews[i]=tmp_trackScaleViews[j];
        m_trackLabels[i]=tmp_trackLabels[j];
        m_trackValueLabels[i]=tmp_trackValuelabels[j];
        m_addButtons[i]=tmp_addButtons[j];
        m_removeButtons[i]=tmp_removeButtons[j];
        m_colorButtons[i]=tmp_colorButtons[j];
        m_shrinkButtons[i]=tmp_shrinkButtons[j];
        m_growButtons[i]=tmp_growButtons[j];
        m_closeButtons[i]=tmp_closeButtons[j];
        m_wellInfos[i]=tmp_wellInfos[j];
        m_markers[i]=tmp_markers[j];
    }

    layoutLogs();
}



void LogViewer::on_actionSetup_Horizons_triggered()
{
    Q_ASSERT(m_project);

    QStringList avail=m_project->gridList(GridType::Horizon);
    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Setup Horizons"), tr("Select Grids:"), avail, &ok, m_horizons.keys());
    if( !ok ) return;

    // first pass remove items
    for( auto name : m_horizons.keys() ){

        if( !names.contains(name)){
            m_horizons.remove(name);
        }
        else names.removeAll(name);
    }

    // now we only have names that need to be added
    // second pass add items
    for( auto name : names){

        auto g = m_project->loadGrid(GridType::Horizon, name);

        if( !g ){
            QMessageBox::critical(this, tr("Add Grid"), QString("Loading grid \"%1\" failed!").arg(name) );
            break;
        }

        m_horizons.insert(name, g);
    }

    updateTrackHorizons();
}




void LogViewer::on_actionSetup_Tops_triggered()
{

    QSet<QString> savail;
    for( auto pmks : m_markers){
        if( !pmks ) continue;
        for( auto m : *pmks){
            std::cout<<m.name().toStdString()<<std::endl;
            savail.insert(m.name());
        }
    }
    QStringList avail=QStringList::fromSet(savail);
    std::sort( avail.begin(), avail.end());

    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Setup Tops"), tr("Select Tops:"), avail, &ok, m_selectedTops);
    if( !ok ) return;

    m_selectedTops=names;

    updateTrackTops();
}


void picksToLog(Log& log, const QVector<QPointF>& points){

    for( int i=1; i<points.size(); i++){
        auto p0=points[i-1];
        auto p1=points[i];
        auto iz0=log.z2index(p0.y());
        auto iz1=log.z2index(p1.y());
        for( int iz=iz0; iz<iz1; iz++ ){
            auto z = log.index2z(iz);
            auto x = p0.x() + (z-p0.y())*(p1.x()-p0.x())/(p1.y()-p0.y());
            log[iz]=x;
        }
    }

}

void LogViewer::runTrackViewContextMenu(QPoint pos){

    TrackView* tv = dynamic_cast<TrackView*>(sender());
    if( !tv) return;

    // find number of this log in log list
    auto index = m_trackViews.indexOf(tv);
    if( index<0) return;
    auto well=m_wellInfos[index].name();
    auto uwi=m_wellInfos[index].uwi();

    if(m_PickMode==PickMode::PICK_LOG){

        QPoint globalPos = tv->mapToGlobal(pos);

        QMenu menu;
        menu.addAction("remove picks");
        menu.addAction("convert picks to new log");
        menu.addAction("add picks to loaded log");
        menu.addAction("save log");
        QAction* selectedAction = menu.exec(globalPos);
        if (!selectedAction) return;

        if( selectedAction->text()=="remove picks" ){
            tv->clearSelection();
        }
        else if(selectedAction->text()=="convert picks to new log"){
            QString name="picks";
            while(1){
                bool ok=false;
                name=QInputDialog::getText(this,"New Log","Enter Name:",QLineEdit::Normal, name, &ok);
                if( name.isEmpty() || !ok) return;
                if( !m_project->existsLog(uwi, name)) break;
            }

            auto log=std::make_shared<Log>(name.toStdString(),"","picked with logviewer",
                                           tv->log()->z0(), tv->log()->dz(), tv->log()->nz());
            auto points=tv->selectedPoints();
            picksToLog(*log, points);
            /*
            // fill log from points, XXX PUT THIS IN FUNCTION
            for( int i=1; i<points.size(); i++){
                auto p0=points[i-1];
                auto p1=points[i];
                auto iz0=log->z2index(p0.y());
                auto iz1=log->z2index(p1.y());
                for( int iz=iz0; iz<iz1; iz++ ){
                    auto z = log->index2z(iz);
                    auto x = p0.x() + (z-p0.y())*(p1.x()-p0.x())/(p1.y()-p0.y());
                    (*log)[iz]=x;
                }
            }
            */
            tv->addLog(log);
            tv->clearSelection();
        }
        else if(selectedAction->text()=="add picks to loaded log"){
            QStringList avail;
            for(int i=0; i<tv->count(); i++){
                avail<<tv->log(i)->name().c_str();
            }
            int idx=0;
            if(avail.size()>1){
                bool ok=false;
                QString name=QInputDialog::getItem(this, "add picks to log", "Select log:", avail, 0, false, &ok);
                if( name.isEmpty() || !ok ) return;
                idx=avail.indexOf(name);
            }
            auto log=tv->log(idx);
            auto points=tv->selectedPoints();
            picksToLog(*log, points);
            tv->clearSelection();
            tv->logChanged(idx);
        }
        else if(selectedAction->text()=="save log"){
            QStringList avail;
            for(int i=0; i<tv->count(); i++){
                auto l=tv->log(i);
                if(l) avail<<l->name().c_str();
            }

            QStringList names;
            if(avail.size()==1){
                names=avail;
            }
            else{
               names = MultiItemSelectionDialog::getItems(this, tr("Save Logs"), tr("Select Logs:"), avail);
            }

            for( auto name : names ){
                auto lindex=avail.indexOf(name);
                auto log=tv->log(lindex);

                bool ok=false;
                auto oname=QInputDialog::getText(this, "Save Log", "Name:", QLineEdit::Normal, name, &ok);
                if(oname.isEmpty() || !ok) return;
                log->setName(oname.toStdString());

                if(!m_project->saveLog(uwi, oname, *log )){
                    QMessageBox::critical(this, "Save Log", tr("Saving Log \"%1-%2\" failed!").arg(well,name), QMessageBox::Ok);
                    return;
                }
            }

        }
    }

}

void LogViewer::updateTrackPickMode(TrackView* tv){

    if( !tv ) return;

    switch(m_PickMode){
    case PickMode::PICK_TOPS:
        tv->setSelectionMode(AxisView::SELECTIONMODE::SinglePoint);
        connect( tv, SIGNAL(pointSelected(QPointF)), this, SLOT(onTrackPointSelected(QPointF)));
        break;
    case PickMode::PICK_LOG:
         tv->setSelectionMode(AxisView::SELECTIONMODE::LinesVOrdered);
           // XXX connect
        break;
    default:
        tv->setSelectionMode(AxisView::SELECTIONMODE::None);
        break;
    }
}
