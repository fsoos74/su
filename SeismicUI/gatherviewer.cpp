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
    connect( gatherView, SIGNAL(traceSelected(size_t)), this, SLOT(onTraceSelected(size_t)) );

    connect( ui->action_Receive_CDPs, SIGNAL(toggled(bool)), this, SLOT(setReceptionEnabled(bool)) );
    connect( ui->action_Dispatch_CDPs, SIGNAL(toggled(bool)), this, SLOT(setBroadcastEnabled(bool)) );
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


QToolBar* GatherViewer::mainToolBar()const{
    return ui->mainToolBar;
}

void GatherViewer::receivePoint(QPoint point){

    int iline=point.x();
    int xline=point.y();
    emit requestPoint(iline, xline);
}

void GatherViewer::receivePoints(QVector<QPoint> points, int code){

    if( code==CODE_POLYLINE ){
        emit requestPoints( points );
    }
}

void GatherViewer::zoomFitWindow(){
    ui->zoomFitWindowAct->trigger();
}

 void GatherViewer::setGather( std::shared_ptr<seismic::Gather> gather){

     if( gather==m_gather ) return;

     m_gather=gather;

     gatherView->setGather(m_gather);

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
        sendPoint(QPoint(iline, xline));
    }
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
    size_t i=trc.time_to_index(secs);
    QString amp=(i<trc.size()) ? QString::number(trc[i]) : QString("n/a");
    QString mills=QString::number( int(1000*secs) );

    QString message;
    for( auto anno : m_traceAnnotations ){

        message += anno.second + "=" + toQString( trc.header().at( anno.first) ) + ", ";
    }
    message+=QString( " Time=") + mills + QString(", Amplitude= ")+ amp;

    // this MUST changed to time instead of samples because sampling of volume can be different from trace!!!
    if( gatherView->volume()){

        int iline=trc.header().at("iline").intValue();
        int xline=trc.header().at("xline").intValue();
        float attr=gatherView->volume()->value(iline,xline,secs);
        message+=QString(", Attibute=");
        if( attr!=gatherView->volume()->NULL_VALUE){
            message+=QString::number(attr);
        }
        else{
            message+="NULL";
        }
    }

    statusBar()->showMessage( message );

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

    if( !m_project ) return;

    bool ok=false;
    QString gridName=QInputDialog::getItem(this, "Open Horizon", "Please select a horizon:",
                                           m_project->gridList(GridType::Horizon), 0, false, &ok);

    if( !gridName.isEmpty() && ok ){

        std::shared_ptr<Grid2D<double> > grid=m_project->loadGrid(GridType::Horizon, gridName);

        gatherView->addHorizon( gridName, grid );
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

