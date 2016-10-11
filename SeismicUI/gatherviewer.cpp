#include "gatherviewer.h"
#include "ui_gatherviewer.h"
#include <avoproject.h>
#include "gatherlabel.h"
#include "gatherruler.h"
#include "gatherview.h"
#include "axxislabelwidget.h"
#include "segyinputdialog.h"
#include "segyreader.h"
#include "headerui.h"
#include <header.h>
#include <headervalue.h>
#include <selectheaderwordsdialog.h>
#include <navigationwidget.h>
#include <segy_traceheader_def.h>
#include <colortable.h>
#include <colortabledialog.h>
#include<cmath>
#include<grid3d.h>
#include<QInputDialog>
#include <QSettings>
#include <pointdisplayoptionsdialog.h>

#include<gridviewer.h>

GatherViewer::GatherViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::GatherViewer)
{
    ui->setupUi(this);

    setAttribute( Qt::WA_DeleteOnClose);

    gatherView = ui->gatherView;
    gatherView->gatherLabel()->resize(10000, 2000);

    gatherView->leftRuler()->setAxxisLabels( QStringList( QString("Milliseconds") ) );
    gatherView->topRuler()->setAxxisLabels(QStringList()<<QString("inline:")<<QString("Xline:")<<QString("Offset:") );

    connect( gatherView, SIGNAL(mouseOver(int,qreal)), this, SLOT(onMouseOver(int,qreal)));
    connect( gatherView, SIGNAL(traceClicked(size_t)), this, SLOT(showTraceHeader(size_t)));
   // connect( gatherView, SIGNAL(traceSelected(size_t)), this, SLOT(onTraceSelected(size_t)) );
    connect( gatherView, SIGNAL(pointSelected(SelectionPoint)), this, SLOT(onViewPointSelected(SelectionPoint)) );

    connect( ui->action_Receive_CDPs, SIGNAL(toggled(bool)), this, SLOT(setReceptionEnabled(bool)) );
    connect( ui->action_Dispatch_CDPs, SIGNAL(toggled(bool)), this, SLOT(setBroadcastEnabled(bool)) );

    gatherView->gatherLabel()->setHighlightTrace( ui->actionShare_Current_Position->isChecked()  );
    connect( ui->actionShare_Current_Position, SIGNAL(toggled(bool)), gatherView->gatherLabel(), SLOT(setHighlightTrace(bool)) );


    //resize(sizeHint());
    //setMinimumWidth(ui->mainToolBar->width() + 50);

    //loadSettings();

    createDockWidgets();

    resize( 800, 700);
}




GatherViewer::~GatherViewer()
{
    delete ui;
}


QToolBar* GatherViewer::navigationToolBar()const{
    return ui->navigationToolBar;
}

bool GatherViewer::isShareCurrentPoint()const{
    return ui->actionShare_Current_Position->isChecked();
}

void GatherViewer::setShareCurrentPoint(bool on){
    ui->actionShare_Current_Position->setChecked(on);
}

void GatherViewer::receivePoint(SelectionPoint point, int code){

    //std::cout<<"GatherViewer receive point il="<<point.iline<<" xl="<<point.xline<<" code="<<code<<std::endl<<std::flush;
    switch( code ){

    case PointCode::VIEWER_CURRENT_CDP:
        if( ui->actionShare_Current_Position->isChecked()){
            gatherView->setCursorPosition(point);
            QString msg=createStatusMessage(point);
            statusBar()->showMessage(msg);
        }
        break;

    default:    // VIEWER_CURRENT_POUNT_SELECTED will be handled here
        emit requestPoint( point.iline, point. xline);
        break;
    }
}

void GatherViewer::receivePoints(QVector<SelectionPoint> points, int code){

    QVector<QPoint> rpoints;
    rpoints.reserve(points.size());
    for( SelectionPoint p : points){
        rpoints.push_back(QPoint(p.iline, p.xline));
    }
    if( code==CODE_POLYLINE ){

        emit requestPoints( rpoints );
        return;
    }
    if( code==CODE_SINGLE_POINTS){
        view()->setHighlightedPoints(points);
    }

}

void GatherViewer::zoomFitWindow(){
    ui->zoomFitWindowAct->trigger();
}

 void GatherViewer::setGather( std::shared_ptr<seismic::Gather> gather){

     if( gather==m_gather ) return;

     m_gather=gather;

     gatherView->setGather(m_gather);

     gatherView->setIntersectionTraces( computeIntersections() );

     updateIntersections();

    emit gatherChanged();
 }

 void GatherViewer::setProject( std::shared_ptr<AVOProject> project){

     if( project==m_project ) return;

     m_project=project;

    emit projectChanged();

 }



 void GatherViewer::setTraceAnnotations( const std::vector< std::pair< std::string, QString > >& annos ){

     m_traceAnnotations=annos;

     emit traceAnnotationsChanged();

    QStringList labels;
    std::vector<std::string> keys;
    for( auto anno : annos){
        keys.push_back( anno.first);
        labels.push_back(anno.second);
    }

    gatherView->setAnnotationKeys(keys);
    gatherView->axxisLabelWidget()->setLabels( labels );
 }


  void GatherViewer::setTraceHeaderDef(const std::vector<seismic::SEGYHeaderWordDef>& def){

      m_traceHeaderDef=def;

  }

std::shared_ptr<seismic::Gather> createTestGather(){

    std::shared_ptr<Gather> gt( new Gather() );

    float ft=0;
    float dt=0.002;
    int nt=2001;



    for( size_t i=0; i<1000; i++){

        float f= i;

        seismic::Trace trc( ft, dt, nt);
        seismic::Trace::Samples& sam=trc.samples();

        for( size_t j=0; j<sam.size(); j++){
            double t=dt * j;
            sam[j]=std::sin( 2* 3.1415927 * f * t);
        }

        gt->push_back( trc);

    }

    return gt;

}

void GatherViewer::showTraceHeader( size_t i)
{
    Q_ASSERT(m_gather);
    if( !m_TraceHeaderDialog ){
         m_TraceHeaderDialog=new HeaderDialog(this, true);

         m_TraceHeaderDialog->navigationWidget()->setRange(1, m_gather->size());
         connect( m_TraceHeaderDialog->navigationWidget(), SIGNAL( currentChanged(size_t)), this, SLOT(navigateToTraceHeader(size_t)));
         connect( m_TraceHeaderDialog, SIGNAL(finished()), this, SLOT(onTraceHeaderDialogFinished()));

    }

    m_TraceHeaderDialog->navigationWidget()->setCurrent(i+1);   // this is 1 based
     m_TraceHeaderDialog->setData((*m_gather)[i].header(), m_traceHeaderDef);
     m_TraceHeaderDialog->show();
    m_TraceHeaderDialog->setWindowTitle( QString("Trace Header #%1").arg(QString::number(i+1)));


    gatherView->gatherLabel()->setHighlightedTrace(i);
    gatherView->gatherLabel()->setHighlightTrace(true);
}



void GatherViewer::onTraceSelected(size_t i){

    gatherView->gatherLabel()->setHighlightedTrace(i);
    if( i<m_gather->size()){

        const seismic::Trace& trace=(*m_gather)[i];
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        sendPoint(SelectionPoint(iline, xline, SelectionPoint::NO_TIME), VIEWER_CURRENT_CDP);
    }
}


void GatherViewer::onViewPointSelected(SelectionPoint sp){
    sendPoint(sp, PointCode::VIEWER_POINT_SELECTED);
}

// this function was added because navigation widgets numbers are 1-based where as show trace header uses 0-based trace numbers
void GatherViewer::navigateToTraceHeader(size_t i){
    showTraceHeader(i-1);
}

void GatherViewer::onTraceHeaderDialogFinished(){

    Q_ASSERT( m_TraceHeaderDialog);

    //delete m_TraceHeaderDialog;
    //m_TraceHeaderDialog=nullptr;
    m_TraceHeaderDialog->hide();
    gatherView->gatherLabel()->setHighlightTrace(false);
}


void GatherViewer::on_zoomInAct_triggered()
{
    gatherView->zoomBy(1.25);
}

void GatherViewer::on_zoomOutAct_triggered()
{
    gatherView->zoomBy(0.8);
}

void GatherViewer::on_zoomFitWindowAct_triggered()
{
    gatherView->normalize();
}

void GatherViewer::onMouseOver(int trace, qreal secs){


    if( !m_gather || trace>=m_gather->size()) return;

    const seismic::Trace& trc=(*m_gather)[trace];

    int iline=trc.header().at("iline").intValue();
    int xline=trc.header().at("xline").intValue();
    SelectionPoint sp(iline, xline, secs);
    QString message=createStatusMessage( sp );

    statusBar()->showMessage( message );

    if( ui->actionShare_Current_Position->isChecked()){

        gatherView->setCursorPosition(sp);      // need to set this because the send point will not be received by this viewer
        sendPoint( sp, PointCode::VIEWER_CURRENT_CDP);
    }

}


void GatherViewer::on_actionTrace_Scaling_triggered()
{
    if( !m_traceScalingDialog){

        GatherLabel* gatherLabel=gatherView->gatherLabel();

        m_traceScalingDialog=new TraceScalingDialog(this);
        m_traceScalingDialog->setWindowTitle("Configure Trace Scaling");

        GatherScaler* scaler=gatherView->gatherScaler();
        m_traceScalingDialog->setScalingMode(scaler->mode());
        m_traceScalingDialog->setScalingFactor(scaler->fixedScaleFactor());

        connect( m_traceScalingDialog, SIGNAL( scalingModeChanged(GatherScaler::Mode)),
                 scaler, SLOT(setMode(GatherScaler::Mode)));
        connect( m_traceScalingDialog, SIGNAL( scalingFactorChanged(qreal)),
                 scaler, SLOT( setFixedScaleFactor(qreal)));
    }

    m_traceScalingDialog->show();
}

void GatherViewer::on_action_Trace_Options_triggered()
{
    if( !m_traceDisplayOptionsDialog){

        GatherLabel* gatherLabel=gatherView->gatherLabel();

        m_traceDisplayOptionsDialog=new TraceDisplayOptionsDialog(this);
        m_traceDisplayOptionsDialog->setWindowTitle("Configure Trace Display");
        m_traceDisplayOptionsDialog->setDisplayWiggles(gatherLabel->isDisplayWiggles());
        m_traceDisplayOptionsDialog->setDisplayVariableArea(gatherLabel->isDisplayVariableArea());
        m_traceDisplayOptionsDialog->setDisplayDensity(gatherLabel->isDisplayDensity());
        m_traceDisplayOptionsDialog->setColors(gatherLabel->colorTable()->colors() );
        GatherScaler* scaler=gatherView->gatherScaler();
        m_traceDisplayOptionsDialog->setTraceColor(gatherLabel->traceColor());
        m_traceDisplayOptionsDialog->setTraceOpacity(gatherLabel->traceOpacity());
        m_traceDisplayOptionsDialog->setDensityOpacity(gatherLabel->densityOpacity());

        connect( m_traceDisplayOptionsDialog, SIGNAL(displayWigglesChanged(bool)),
                 gatherLabel, SLOT(setDisplayWiggles(bool)) );
        connect( m_traceDisplayOptionsDialog, SIGNAL(displayVariableAreaChanged(bool)),
                 gatherLabel, SLOT( setDisplayVariableArea(bool)) );
        connect( m_traceDisplayOptionsDialog, SIGNAL(displayDensityChanged(bool)),
                 gatherLabel, SLOT(setDisplayDensity(bool)) );

        connect( m_traceDisplayOptionsDialog, SIGNAL( colorsChanged(QVector<QRgb>)),
                 gatherLabel->colorTable(), SLOT(setColors(QVector<QRgb>)) );

        connect( m_traceDisplayOptionsDialog, SIGNAL(traceColorChanged(QColor)),
                 gatherLabel, SLOT(setTraceColor(QColor)) );
        connect( m_traceDisplayOptionsDialog, SIGNAL(traceOpacityChanged(int)),
                 gatherLabel, SLOT(setTraceOpacity(int)) );
        connect( m_traceDisplayOptionsDialog, SIGNAL(densityOpacityChanged(int)),
                 gatherLabel, SLOT(setDensityOpacity(int)) );
    }

    m_traceDisplayOptionsDialog->show();
}

void GatherViewer::on_actionVolume_Options_triggered()
{
    if( !m_volumeDisplayOptionsDialog){
        m_volumeDisplayOptionsDialog=new VolumeDisplayOptionsDialog(this);
        m_volumeDisplayOptionsDialog->setWindowTitle("Configure Volume Overlay");
        if( gatherView->volume()){
            auto range=gatherView->volume()->valueRange();
            std::pair<double, double> drange(range.first, range.second);
            m_volumeDisplayOptionsDialog->setRange(drange);
        }

        GatherLabel* gatherLabel=gatherView->gatherLabel();

        m_volumeDisplayOptionsDialog->setColors(gatherLabel->volumeColorTable()->colors() );
        connect( m_volumeDisplayOptionsDialog, SIGNAL(rangeChanged(std::pair<double,double>)),
                 gatherLabel->volumeColorTable(), SLOT(setRange(std::pair<double,double>)) );
        connect( m_volumeDisplayOptionsDialog, SIGNAL(opacityChanged(int)),
                 gatherLabel, SLOT(setVolumeOpacity(int)) );
        connect( m_volumeDisplayOptionsDialog, SIGNAL( colorsChanged(QVector<QRgb>)),
                 gatherLabel->volumeColorTable(), SLOT(setColors(QVector<QRgb>)) );
    }

    m_volumeDisplayOptionsDialog->show();
}


void GatherViewer::on_openGridAct_triggered()
{
    static const QVector<QColor> HorizonColors{Qt::darkRed, Qt::darkGreen, Qt::darkBlue,
                Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow,
                Qt::red, Qt::green, Qt::blue,
                Qt::cyan, Qt::magenta, Qt::yellow};

    if( !m_project ) return;

    bool ok=false;
    QString gridName=QInputDialog::getItem(this, "Open Horizon", "Please select a horizon:",
                                           m_project->gridList(GridType::Horizon), 0, false, &ok);

    QColor color=HorizonColors[gatherView->horizonList().size() % HorizonColors.size()];

    if( !gridName.isEmpty() && ok ){

        std::shared_ptr<Grid2D<float> > grid=m_project->loadGrid(GridType::Horizon, gridName );

        gatherView->addHorizon( gridName, grid, color );
    }

}

void GatherViewer::on_closeGridAct_triggered()
{
    if( gatherView->horizonList().empty()) return;

    bool ok=false;
    QString name=QInputDialog::getItem(this, "Close Horizon", "Select horizon", gatherView->horizonList(), 0, false, &ok);

    if( ok && !name.isEmpty()){
        gatherView->removeHorizon(name);
    }
}

void GatherViewer::on_actionOpenVolume_triggered()
{
    if( !m_project ) return;

    bool ok=false;
    QString volumeName=QInputDialog::getItem(this, "Open Volume", "Please select a volume:",
                                           m_project->volumeList(), 0, false, &ok);

    if( !volumeName.isEmpty() && ok ){

        std::shared_ptr<Grid3D<float> > volume=m_project->loadVolume(volumeName);

        if( !volume ) return;

        auto frange=volume->valueRange();   // XXX maybe put this into gatherview::setVolume

        gatherView->setVolume( volume );


        gatherView->gatherLabel()->volumeColorTable()->setRange(frange.first, frange.second);

        m_attributeColorBarWidget->setLabel(volumeName);

        // adjust the display range in volume options dialog if open
        if( m_volumeDisplayOptionsDialog){           
            m_volumeDisplayOptionsDialog->setRange(std::pair<double,double>(frange.first,frange.second));
        }

    }

}


void GatherViewer::on_actionCloseVolume_triggered()
{
    gatherView->setVolume(std::shared_ptr<Grid3D<float>>());
}

void GatherViewer::closeEvent(QCloseEvent *)
{
    saveSettings();

    sendPoint( SelectionPoint::NONE, PointCode::VIEWER_CURRENT_CDP );  // notify other viewers
}

void GatherViewer::leaveEvent(QEvent *){

    gatherView->setCursorPosition(SelectionPoint::NONE);
    QString msg=createStatusMessage(SelectionPoint::NONE);
    statusBar()->showMessage(msg);
    sendPoint( SelectionPoint::NONE, PointCode::VIEWER_CURRENT_CDP );
}

void GatherViewer::saveSettings(){

     QSettings settings(COMPANY, "GatherViewer");

     settings.beginGroup("GatherViewer");
       settings.setValue("size", size());
       settings.setValue("position", pos() );
     settings.endGroup();

     settings.sync();
}


void GatherViewer::loadSettings(){

    QSettings settings(COMPANY, "GatherViewer");

    settings.beginGroup("GatherViewer");
        resize(settings.value("size", QSize(800, 700)).toSize());
        move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();


}


QString GatherViewer::createStatusMessage( SelectionPoint sp){

    if( sp==SelectionPoint::NONE) return QString();

    int tno=gatherView->lookupTrace(sp.iline, sp.xline); // find trace number, -1 if not found
    if( tno<0 ) return QString();

    const seismic::Trace& trc=(*m_gather)[tno];

    QString message;
    for( auto anno : m_traceAnnotations ){

        message += anno.second + "=" + toQString( trc.header().at( anno.first) ) + ", ";
    }

    int iline=trc.header().at("iline").intValue();
    int xline=trc.header().at("xline").intValue();

    // compute x,y from inline/crossline rather than taking from header from header
    // possible improvement: poststack disp cdpx, cdpy; prestack: sx,sy,gx,gy from header, if not present use computed as fallback
    QTransform IlXlToXY;
    QTransform XYToIlXl;
    if( m_project && m_project->geometry().computeTransforms( XYToIlXl, IlXlToXY)){


            QPointF p=IlXlToXY.map( QPoint(iline, xline));
            qreal x=p.x();
            qreal y=p.y();

            message+=QString::asprintf("x*=%.1lf, y*=%.1lf, ", x, y);
    }

    if( sp.time==SelectionPoint::NO_TIME ) return message;  // that's all we can do without valid time

    size_t i=trc.time_to_index(sp.time);
    QString amp=(i<trc.size()) ? QString::number(trc[i]) : QString("n/a");
    QString mills=QString::number( int(1000*sp.time) );

    message+=QString( " Time=") + mills + QString(", Amplitude= ")+ amp;
    if( gatherView->volume()){

        float attr=gatherView->volume()->value(iline,xline,sp.time);
        message+=QString(", Attibute=");
        if( attr!=gatherView->volume()->NULL_VALUE){
            message+=QString::number(attr);
        }
        else{
            message+="NULL";
        }
    }

    return message;
}


void GatherViewer::createDockWidgets(){

    m_densityColorBarDock = new QDockWidget(tr("Density Colorbar"), this);
    m_densityColorBarDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    m_densityColorBarWidget=new ColorBarWidget(m_densityColorBarDock);
    m_densityColorBarWidget->setScaleAlignment(ColorBarWidget::SCALE_RIGHT);
    m_densityColorBarDock->setContentsMargins(10, 5, 10, 5);
    m_densityColorBarDock->setWidget(m_densityColorBarWidget);
    addDockWidget(Qt::LeftDockWidgetArea, m_densityColorBarDock);
    m_densityColorBarWidget->setColorTable( gatherView->gatherLabel()->colorTable());
    m_densityColorBarWidget->setLabel("Density [Relative Amplitude]");
    m_attributeColorBarDock = new QDockWidget(tr("Attribute Colorbar"), this);
    m_attributeColorBarDock->setAllowedAreas(Qt::RightDockWidgetArea);
    m_attributeColorBarWidget=new ColorBarWidget(m_attributeColorBarDock);
    m_attributeColorBarWidget->setScaleAlignment(ColorBarWidget::SCALE_LEFT);
    m_attributeColorBarDock->setContentsMargins(10, 5, 10, 5);
    m_attributeColorBarDock->setWidget(m_attributeColorBarWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_attributeColorBarDock);
    m_attributeColorBarWidget->setColorTable( gatherView->gatherLabel()->volumeColorTable());


    ui->menu_Window->addAction(m_densityColorBarDock->toggleViewAction());
    ui->menu_Window->addAction(m_attributeColorBarDock->toggleViewAction());

    m_attributeColorBarDock->close();
    m_densityColorBarDock->close();
}


void GatherViewer::on_action_Point_Display_Options_triggered()
{
    if( !m_pointDisplayOptionsDialog){

        m_pointDisplayOptionsDialog=new PointDisplayOptionsDialog(this);
        m_pointDisplayOptionsDialog->setWindowTitle("Configure Point Display");

        GatherLabel* gatherLabel=gatherView->gatherLabel();

        m_pointDisplayOptionsDialog->setPointSize( gatherLabel->highlightedPointSize());
        m_pointDisplayOptionsDialog->setPointColor( gatherLabel->highlightedPointColor());

        connect( m_pointDisplayOptionsDialog, SIGNAL( pointSizeChanged(int)),
                 gatherLabel, SLOT(setHighlightedPointSize(int)) );
        connect( m_pointDisplayOptionsDialog, SIGNAL(pointColorChanged(QColor)),
                 gatherLabel, SLOT(setHighlightedPointColor(QColor)) );
    }

    m_pointDisplayOptionsDialog->show();
}


void GatherViewer::updateIntersections(){

    QVector<SelectionPoint> allTraces;

    for( BaseViewer* v : dispatcher()->viewers() ){

        GatherViewer* gv=dynamic_cast<GatherViewer*>(v);

        if( !gv || !gv->gather() ) continue;

        //if( gv->ui->actionShare_Current_Position->isChecked()){  // only add traces from viewers that share
        if( gv->broadcastEnabled()){
            for(auto trc : *(gv->gather())){
                int iline=trc.header().at("iline").intValue();
                int xline=trc.header().at("xline").intValue();
                allTraces.push_back(SelectionPoint(iline, xline, 0));
            }

        }

        gv->gatherView->setIntersectionTraces( gv->computeIntersections() );  // will set empty vector if no sharing

        if( gv==this ) continue;
    }

    // prosecc grid viewers
    for( BaseViewer* v : dispatcher()->viewers() ){

        GridViewer* gv=dynamic_cast<GridViewer*>(v);

        if( gv ){

            if( gv->receptionEnabled() ){
                gv->gridView()->setIntersectionPoints(allTraces);
            }
        }
    }
}

QVector<int> GatherViewer::computeIntersections(){

    if( ! dispatcher() ) return QVector<int>();

    if( !m_gather ) return QVector<int>();

    //if( !ui->actionShare_Current_Position->isChecked()) return QVector<int>();
    if( !receptionEnabled()) return QVector<int>();     // no intersection is reception os disabled

    QVector<int> res;

    for( BaseViewer* v : dispatcher()->viewers() ){

        GatherViewer* gv=dynamic_cast<GatherViewer*>(v);

        if( !gv || !gv->gather() || gv==this ) continue;

        if( !gv->broadcastEnabled() ) continue; // ignore viewers that don't share

        for( auto trc : *(gv->gather()) ){

            int iline=trc.header().at("iline").intValue();
            int xline=trc.header().at("xline").intValue();

            int i  = gatherView->lookupTrace(iline,xline);

            if( i!=-1 ) res.push_back(i);
        }

    }

    return res;
}




void GatherViewer::on_actionShare_Current_Position_toggled(bool on)
{
    // need to update all viewers accordingly
    updateIntersections();
}

void GatherViewer::on_action_Receive_CDPs_toggled(bool on)
{
    setReceptionEnabled(on);
    updateIntersections();
}

void GatherViewer::on_action_Dispatch_CDPs_toggled(bool on)
{
    setBroadcastEnabled(on);
    updateIntersections();
}
