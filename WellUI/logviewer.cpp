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

QMap<ZMode,QString> zmodeNameLookup{
    {ZMode::MD, "MD"},
    {ZMode::TVD, "TVD"}
};


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

    setupZModeToolbar();
    setupFilterToolbar();
    setupFlattenToolbar();
}

LogViewer::~LogViewer()
{
    delete ui;
}


void LogViewer::setupZModeToolbar(){

    QToolBar* toolBar=new QToolBar( "Z-Mode", this);

    toolBar->addWidget(new QLabel("Z-Axis:", this));
    m_cbZMode=new QComboBox(this);
    for(auto m : zmodeNameLookup.keys()){
        m_cbZMode->addItem(zmodeNameLookup.value(m));
    }
    toolBar->addWidget(m_cbZMode);

    addToolBar(toolBar);

    connect( m_cbZMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(setZMode(QString)));
    connect( this, SIGNAL(zmodeChanged(QString)), m_cbZMode, SLOT(setCurrentText(QString)));
}

void LogViewer::setupFlattenToolbar(){

    QToolBar* toolBar=new QToolBar( "Flatten", this);

    m_cbFlattenMode=new QComboBox(this);
    m_cbFlattenMode->addItem("NO Flattening");
    m_cbFlattenMode->addItem("Flatten on Horizon");
    m_cbFlattenMode->addItem("Flatten on Top");
    m_cbFlattenMode->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    toolBar->addWidget(m_cbFlattenMode);
    connect( m_cbFlattenMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setFlattenMode(int)));

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
    connect( tl, SIGNAL(moved(QPoint)), this, SLOT(onTrackLabelMoved(QPoint)) );
    m_trackLabels.push_back(tl);

    QStyle *style = qApp->style();
    QIcon closeIcon = style->standardIcon(QStyle::SP_TitleBarCloseButton);
    QPushButton* tb=new QPushButton(closeIcon,"",this);
    m_closeButtons.push_back(tb);
    connect( tb, SIGNAL(clicked()), this, SLOT(onCloseTrack()));

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
    m_trackViews.push_back(tv);

    tv->setLog(l);
    tv->setWellPath(p);
    tv->setZMode(m_zmode);
    connect( this, SIGNAL(zmodeChanged(ZMode)), tv, SLOT(setZMode(ZMode)) );
    tv->setSelectionMode(AxisView::SELECTIONMODE::SinglePoint);
    connect( tv, SIGNAL(pointSelected(QPointF)), this, SLOT(onTrackPointSelected(QPointF)));

    tv->setFilterLen(m_filterLen);
    connect( this, SIGNAL(filterLenChanged(int)), tv, SLOT(setFilterLen(int)) );

    // adjust range to show at least 2 tick labels
    tv->xAxis()->adjustRange();

    HScaleView* hv=new HScaleView(this);
    hv->setXAxis(tv->xAxis());
    hv->setZoomMode(AxisView::ZOOMMODE::X);
    hv->setCursorMode(AxisView::CURSORMODE::X);
    hv->setFixedHeight(80);
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
        m_vscaleView->setFixedWidth(80);
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

   tv->setMinimumWidth(100);
   tv->setMinimumHeight(500);
   tv->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
   layoutLogs();

   updateZAxis();
}


void LogViewer::removeLog(int j){

    if( j<0 || j>m_trackViews.size()) return;
    m_wellInfos.remove(j);
    m_markers.remove(j);
    delete m_trackLabels[j]; m_trackLabels.remove(j);
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
    emit zmodeChanged(zmodeNameLookup.value(m));
}

void LogViewer::setZMode(QString str){
    auto m=zmodeNameLookup.key(str, ZMode::MD);
    setZMode(m);
}

void LogViewer::setFlattenMode(int m){

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

void LogViewer::layoutLogs(){

    QGridLayout* lo = new QGridLayout;

    lo->addWidget( m_vscaleView, 1, 0);

    for( int i = 0; i<m_trackViews.size(); i++){
        QHBoxLayout* caption = new QHBoxLayout;
        caption->addWidget( m_trackLabels[i]);//, 0, i+1);
        caption->addWidget(m_closeButtons[i]);
        caption->setStretch(0,1);   // only grow label not button
        caption->setStretch(1,0);   // only grow label not button
        lo->addLayout(caption, 0, i+1);
        lo->addWidget( m_trackViews[i], 1, i+1);
        lo->addWidget( m_trackScaleViews[i], 2, i+1);
        lo->addWidget( m_trackValueLabels[i], 3, i+1);
        lo->setColumnStretch(i+1, 1);
    }

    lo->addWidget(m_verticalScrollBar, 1, 1000);

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

void LogViewer::onZCursorChanged(qreal z){

    for( int i=0; i<m_trackViews.size(); i++){

        auto trackView=m_trackViews[i];
        auto md=trackView->view2log(z);
        auto log=m_trackViews[i]->log();
        auto value=(*log)(md);
        QString text;
        if( value!=log->NULL_VALUE){
            text=QString("%1 %2").arg(value).arg(log->unit().c_str());
        }

        //auto wellPath=m_trackViews[i]->wellPath();
        //qreal md=( m_zmode==ZMode::MD || !wellPath) ? z : m_trackViews[i]->wellPath()->mdAtTVD(z);
        //auto log=m_trackViews[i]->log();
        //int j = (md-log->z0())/log->dz();
        //QString text;
        //if( j>=0 && j<log->nz() ){
        //    auto value = (*log)[j];
        //    if( value!=log->NULL_VALUE){
        //        text=QString("%1 %2").arg(value).arg(log->unit().c_str());
        //    }
        //}

        m_trackValueLabels[i]->setText(text);
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
                m_closeButtons.move(sourceIndex, i);
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
    auto tmp_closeButtons=m_closeButtons;
    auto tmp_wellInfos=m_wellInfos;
    auto tmp_markers=m_markers;
    for( int i=0; i<m_trackViews.size(); i++){
        int j=std::get<2>(well_log_pos[i]);
        m_trackViews[i]=tmp_trackViews[j];
        m_trackScaleViews[i]=tmp_trackScaleViews[j];
        m_trackLabels[i]=tmp_trackLabels[j];
        m_trackValueLabels[i]=tmp_trackValuelabels[j];
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
    auto tmp_wellInfos=m_wellInfos;
    auto tmp_markers=m_markers;
    for( int i=0; i<m_trackViews.size(); i++){
        int j=std::get<2>(well_log_pos[i]);
        m_trackViews[i]=tmp_trackViews[j];
        m_trackScaleViews[i]=tmp_trackScaleViews[j];
        m_trackLabels[i]=tmp_trackLabels[j];
        m_trackValueLabels[i]=tmp_trackValuelabels[j];
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
