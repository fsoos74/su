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
#include "sectionscaledialog.h"
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
//#include<mousemodeselector.h>
#include<dynamicmousemodeselector.h>
#include <QActionGroup>
//#include<gridviewer.h>
#include <histogramdialog.h>
#include <QMessageBox>
#include<agc.h>
#include <multiitemselectiondialog.h>
#include <linedisplayoptionsdialog.h>

GatherViewer::GatherViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::GatherViewer)
{
    ui->setupUi(this);

    m_progressBar=new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setFixedWidth(100);
    ui->statusBar->addPermanentWidget(m_progressBar);

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

    connect( gatherView, SIGNAL(topRulerClicked(int)), this, SLOT(onTopRulerClicked(int)) );
    connect( gatherView, SIGNAL(leftRulerClicked(qreal)), this, SLOT(onLeftRulerClicked(qreal)) );
    //resize(sizeHint());
    //setMinimumWidth(ui->mainToolBar->width() + 50);

    //loadSettings();

    connect( ui->zoomFitWindowAct, SIGNAL(triggered(bool)), this, SLOT(zoomFitWindow()));
    connect( ui->zoomInAct, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect( ui->zoomOutAct, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));

    // deactivate zooming controls if scale is locked
    connect( gatherView, SIGNAL(fixedScaleChanged(bool)), ui->zoomInAct, SLOT(setDisabled(bool)) );
    connect( gatherView, SIGNAL(fixedScaleChanged(bool)), ui->zoomOutAct, SLOT(setDisabled(bool)) );
    connect( gatherView, SIGNAL(fixedScaleChanged(bool)), ui->zoomFitWindowAct, SLOT(setDisabled(bool)) );

    // allow display of intersection times with grid viewers
    connect( this, SIGNAL(intersectionTimesChanged(QVector<qreal>)), gatherView, SLOT(setIntersectionTimes(QVector<qreal>)) );


    createDockWidgets();
    populateWindowMenu();
    setupPickMenus();
    setupMouseModes();
    setupGainToolBar();

    gatherView->picker()->setPicks(std::make_shared<Table>());
    m_picksName = QString();
    adjustPickActions();

    resize( 1024 , 768);
}


void GatherViewer::populateWindowMenu(){

    ui->menu_Window->addAction(m_densityColorBarDock->toggleViewAction());
    ui->menu_Window->addAction(m_attributeColorBarDock->toggleViewAction());
    ui->menu_Window->addSeparator();
    ui->menu_Window->addAction( ui->mouseToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->zoomToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->mainToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->navigationToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->gainToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->pickToolBar->toggleViewAction());

}


void GatherViewer::setupPickMenus(){

    // need to group picker options here because designer does not support this
    QActionGroup* pickModeGroup=new QActionGroup(this);
    pickModeGroup->setExclusive(true);
    std::array<PickMode,5> modes{PickMode::Single, PickMode::Left, PickMode::Right, PickMode::Lines, PickMode::Outline};
    for( PickMode mode : modes){
        QAction* act = new QAction( toQString(mode), this);
        act->setCheckable(true);
        act->setIcon(QIcon(pickModePixmap(mode)));
        act->setChecked(gatherView->picker()->mode()==mode);
        ui->menuPick_Mode->addAction(act);
        ui->pickToolBar->addAction(act);
        pickModeGroup->addAction(act);
        m_pickModeActions.insert(mode,act);
    }

    QActionGroup* pickTypeGroup=new QActionGroup(this);
    pickTypeGroup->setExclusive(true);
    std::array<PickType,4> types{PickType::Free, PickType::Minimum, PickType::Maximum, PickType::Zero };
    for( PickType type : types){
        QAction* act = new QAction( toQString(type), this);
        act->setCheckable(true);
        act->setIcon(QIcon(pickTypePixmap(type)));
        act->setChecked( gatherView->picker()->type()==type );

        ui->menuPick_Type->addAction(act);
        ui->pickToolBar->addAction(act);
        pickTypeGroup->addAction(act);
        m_pickTypeActions.insert(type, act);
    }

    // need to group picker options here because designer does not support this
    QActionGroup* fillModeGroup=new QActionGroup(this);
    fillModeGroup->setExclusive(true);
    std::array<PickFillMode,2> fillModes{PickFillMode::Next, PickFillMode::Nearest };
    for( PickFillMode mode : fillModes){
        QAction* act = new QAction( toQString(mode), this);
        act->setCheckable(true);
        //act->setIcon(QIcon(pickModePixmap(mode)));
        act->setChecked(gatherView->picker()->fillMode()==mode);
        ui->menuFill_Method->addAction(act);
        //ui->pickToolBar->addAction(act);
        fillModeGroup->addAction(act);
    }

    connect( pickModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(pickModeSelected(QAction*)) );
    connect( pickTypeGroup, SIGNAL(triggered(QAction*)), this, SLOT(pickTypeSelected(QAction*)) );
    connect( fillModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(fillModeSelected(QAction*)) );

    connect( ui->actionConservative, SIGNAL(toggled(bool)), gatherView->picker(), SLOT(setConservative(bool)) );
    connect( gatherView->picker(), SIGNAL(conservativeChanged(bool)), ui->actionConservative, SLOT(setChecked(bool)) );
}

void GatherViewer::setupMouseModes(){

    DynamicMouseModeSelector* mm=new DynamicMouseModeSelector(this);
    connect( mm, SIGNAL(modeChanged(MouseMode)), gatherView, SLOT(setMouseMode(MouseMode)));
    mm->addMode(MouseMode::Explore);
    mm->addMode(MouseMode::Zoom);
    mm->addMode(MouseMode::Select);
    mm->addMode(MouseMode::Pick);
    mm->addMode(MouseMode::DeletePick);
    ui->mouseToolBar->addWidget( mm);

    // don't zoom on fixed scale
    connect( gatherView, SIGNAL(fixedScaleChanged(bool)), mm->button(MouseMode::Zoom), SLOT(setDisabled(bool)) );

}

void GatherViewer::setupGainToolBar(){
    auto widget=new QWidget(this);
    auto label2=new QLabel(tr("AGC:"));
    auto sbAGC=new QSpinBox;
    auto label1=new QLabel(tr("Gain:"));
    auto sbGain=new QDoubleSpinBox();
    auto layout=new QHBoxLayout;
    layout->addWidget(label1);
    layout->addWidget(sbGain);
    layout->addWidget(label2);
    layout->addWidget(sbAGC);
    widget->setLayout(layout);
    ui->gainToolBar->addWidget(widget);
    connect(sbGain, SIGNAL(valueChanged(double)), ui->gatherView->gatherScaler(), SLOT(setFixedScaleFactor(qreal)));
    connect(ui->gatherView->gatherScaler(), SIGNAL(fixedScaleFactorChanged(qreal)), sbGain, SLOT(setValue(double)));
    connect(sbAGC, SIGNAL(valueChanged(int)), this, SLOT(setAGCLen(int)) );

    sbGain->setRange(0,1000);
    sbGain->setValue(ui->gatherView->gatherScaler()->fixedScaleFactor());
    sbAGC->setRange(0, 9999);
    sbAGC->setValue(m_agcLen);
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

            int tno=gatherView->lookupTrace(point.iline, point.xline); // find trace number, -1 if not found

            QString msg=createStatusMessage(tno, point.time);
            statusBar()->showMessage(msg);
        }
        break;

    case PointCode::VIEWER_POINT_SELECTED:{
        if( !m_gather || m_gather->empty() ) break;
        const seismic::Trace& trace=m_gather->front();
        int iline=trace.header().at("iline").intValue();
        int xline=trace.header().at("xline").intValue();
        if( point.iline==SelectionPoint::NO_LINE) point.iline=iline;
        if( point.xline==SelectionPoint::NO_LINE) point.xline=xline;
        emit requestPoint( point.iline, point. xline);
        break;
    }
    default:
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

void GatherViewer::zoomIn(){
    gatherView->zoomBy(1.25);
}

void GatherViewer::zoomOut(){
    gatherView->zoomBy(0.8);
}


void GatherViewer::zoomFitWindow(){
    gatherView->normalize();
}


void GatherViewer::setDatasetInfo(const SeismicDatasetInfo & info){
    m_datasetInfo=info;
}

 void GatherViewer::setGather( std::shared_ptr<seismic::Gather> gather){

     if( gather==m_gather ) return;

     if( !gather ) return;

     m_gather=gather;

     applyAGC();

     gatherView->setIntersectionTraces( computeIntersections() );

     updateIntersections();

    emit gatherChanged();
 }

 void GatherViewer::setAGCLen(int len){

     if( len==m_agcLen ) return;

     m_agcLen=len;

     applyAGC();
 }

 void GatherViewer::applyAGC(){

     if( m_agcLen && m_gather){

         auto agcg=seismic::agc(*m_gather,m_agcLen);
         gatherView->setGather(agcg);
     }
     else{
         gatherView->setGather(m_gather);
     }
 }

 void GatherViewer::setProject( AVOProject* project){

     if( project==m_project ) return;

     m_project=project;

     //ui->action_New_Picks->trigger();   // create empty picks grid for this project

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

void GatherViewer::on_actionSet_Scale_triggered()
{
    if( !sectionScaleDialog){
        sectionScaleDialog=new SectionScaleDialog(this);
        sectionScaleDialog->setWindowTitle(tr("Configure Scale"));
        sectionScaleDialog->setDPIX(gatherView->gatherLabel()->physicalDpiX());
        sectionScaleDialog->setDPIY(gatherView->gatherLabel()->physicalDpiY());
        sectionScaleDialog->setPixelPerTrace( gatherView->pixelPerTrace() );
        sectionScaleDialog->setPixelPerSecond( gatherView->pixelPerSecond() );
        sectionScaleDialog->setLocked(gatherView->isFixedScale());
        connect( sectionScaleDialog, SIGNAL(pixelPerTraceChanged(qreal)), gatherView, SLOT(setPixelPerTrace(qreal)) );
        connect( sectionScaleDialog, SIGNAL(pixelPerSecondChanged(qreal)), gatherView, SLOT(setPixelPerSecond(qreal)) );
        connect( sectionScaleDialog, SIGNAL(lockedChanged(bool)), gatherView, SLOT(setFixedScale(bool)) );
        connect( gatherView, SIGNAL(pixelPerTraceChanged(qreal)), sectionScaleDialog, SLOT(setPixelPerTrace(qreal)) );
        connect( gatherView, SIGNAL(pixelPerSecondChanged(qreal)), sectionScaleDialog, SLOT(setPixelPerSecond(qreal)) );
        connect( gatherView, SIGNAL( fixedScaleChanged(bool)), sectionScaleDialog, SLOT(setLocked(bool)) );
    }

    sectionScaleDialog->show();
}

void GatherViewer::onMouseOver(int trace, qreal secs){


    if( !m_gather || trace<0 || trace>=static_cast<int>(m_gather->size() ) ) return;

    QString message=createStatusMessage( trace, secs );
    statusBar()->showMessage( message );

    if( ui->actionShare_Current_Position->isChecked()){

        const seismic::Trace& trc=(*m_gather)[trace];
        auto & header=trc.header();
        if( header.find("iline")!=header.end() && header.find("xline")!=header.end()){  // beware of incomplete headers
            int iline=trc.header().at("iline").intValue();
            int xline=trc.header().at("xline").intValue();
            SelectionPoint sp(iline, xline, secs);
            gatherView->setCursorPosition(sp);      // need to set this because the send point will not be received by this viewer
            sendPoint( sp, PointCode::VIEWER_CURRENT_CDP);
        }
    }

}


void GatherViewer::onTopRulerClicked( int traceno ){

    if( !m_gather || traceno<0 || traceno>=static_cast<int>(m_gather->size() ) ) return;

    const seismic::Trace& trace=(*m_gather)[traceno];

    int iline=trace.header().at("iline").intValue();
    int xline=trace.header().at("xline").intValue();

    emit requestPerpendicularLine(iline, xline);
}

void GatherViewer::onLeftRulerClicked(qreal secs){

    SelectionPoint sp(SelectionPoint::NO_LINE, SelectionPoint::NO_LINE, secs);
    sendPoint( sp, PointCode::VIEWER_TIME_SELECTED);
}

void GatherViewer::on_actionTrace_Scaling_triggered()
{
    if( !m_traceScalingDialog){

        //GatherLabel* gatherLabel=gatherView->gatherLabel();

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

void GatherViewer::on_actionDensity_Color_Table_triggered()
{
    ColorTable* colorTable=gatherView->gatherLabel()->colorTable();

    Q_ASSERT(colorTable);

    QVector<QRgb> oldColors=colorTable->colors();

    ColorTableDialog* dlg=new ColorTableDialog( colorTable->colors() );

    dlg->setWindowTitle(tr("Select Density Color Table"));

    connect( dlg, SIGNAL(colorsChanged(QVector<QRgb>)), colorTable , SLOT(setColors(QVector<QRgb>)) );

    if( dlg->exec()==QDialog::Accepted ){
        colorTable->setColors(dlg->colors());
    }
    else{
        colorTable->setColors(oldColors);
    }

    delete dlg;
}


void GatherViewer::on_actionVolume_Color_Table_triggered()
{
    ColorTable* colorTable=gatherView->gatherLabel()->volumeColorTable();

    Q_ASSERT(colorTable);

    QVector<QRgb> oldColors=colorTable->colors();

    ColorTableDialog* dlg=new ColorTableDialog( colorTable->colors() );

    dlg->setWindowTitle(tr("Select Volume Color Table"));

    connect( dlg, SIGNAL(colorsChanged(QVector<QRgb>)), colorTable , SLOT(setColors(QVector<QRgb>)) );

    if( dlg->exec()==QDialog::Accepted ){
        colorTable->setColors(dlg->colors());
    }
    else{
        colorTable->setColors(oldColors);
    }

    delete dlg;
}

void GatherViewer::on_actionVolume_Display_Range_triggered()
{
    if( ! gatherView->volume() ) return;

    Volume* pvolume=gatherView->volume().get();
    if(!pvolume) return;

    ColorTable* colorTable=gatherView->gatherLabel()->volumeColorTable();
    if(!colorTable) return;

    if(!m_volumeDisplayRangeDialog ){

        m_volumeDisplayRangeDialog=new HistogramRangeSelectionDialog(this);
        m_volumeDisplayRangeDialog->setHistogram(createHistogram( std::begin(*pvolume), std::end(*pvolume),
                                                          pvolume->NULL_VALUE, 100 ));
        m_volumeDisplayRangeDialog->setRange( pvolume->valueRange());
        m_volumeDisplayRangeDialog->setColorTable( colorTable );   // all updating through colortable

        m_volumeDisplayRangeDialog->setWindowTitle(QString("Overlay Volume") );
    }


    m_volumeDisplayRangeDialog->show();
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
        m_traceDisplayOptionsDialog->setEditColorTableAction(ui->actionDensity_Color_Table);
        //m_traceDisplayOptionsDialog->setColors(gatherLabel->colorTable()->colors() );
        //GatherScaler* scaler=gatherView->gatherScaler();
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
        m_volumeDisplayOptionsDialog->setEditColorTableAction(ui->actionVolume_Color_Table);
        m_volumeDisplayOptionsDialog->setVolumeDisplayRangeAction(ui->actionVolume_Display_Range);

        GatherLabel* gatherLabel=gatherView->gatherLabel();

        connect( m_volumeDisplayOptionsDialog, SIGNAL(opacityChanged(int)),
                 gatherLabel, SLOT(setVolumeOpacity(int)) );
    }

    m_volumeDisplayOptionsDialog->show();
}

void GatherViewer::on_actionVolume_Histogram_triggered()
{
    if( ! gatherView->volume() ) return;

    std::shared_ptr<Volume> volume=gatherView->volume();

    QVector<double> data;
    data.reserve(volume->size());
    for( auto it=volume->values().cbegin(); it!=volume->values().cend(); ++it){
        if( *it==volume->NULL_VALUE) continue;
        data.push_back(*it);
    }


    HistogramDialog* viewer=new HistogramDialog(this);      // need to make this a parent in order to allow qt to delete this when this is deleted
                                                            // this is important because otherwise the colortable will be deleted before this! CRASH!!!
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of volume overlay -  %1").arg(windowTitle() ) );
    viewer->setColorTable( gatherView->gatherLabel()->volumeColorTable());
    viewer->show();
}

void GatherViewer::on_actionGather_Histogram_triggered()
{
    if( ! m_gather ) return;

    QVector<double> data;

    for( const seismic::Trace& trace : *m_gather ){

        for( float sam : trace.samples() ){

            data.push_back(sam);
        }
    }

    HistogramDialog* viewer=new HistogramDialog(this);      // need to make this a parent in order to allow qt to delete this when this is deleted
                                                            // this is important because otherwise the colortable will be deleted before this! CRASH!!!
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1").arg(windowTitle() ) );
    viewer->setColorTable( gatherView->gatherLabel()->volumeColorTable());
    viewer->show();
}


void GatherViewer::on_action_Load_Picks_triggered()
{
    if( !picksSaved()) return;

    bool ok=false;
    QString name=QInputDialog::getItem(this, "Open Picks", "Please select a Table:",
                                           m_project->tableList(), 0, false, &ok);

    if( name.isEmpty() || !ok ) return;

    auto picks=m_project->loadTable(name );

    if( !picks){
        QMessageBox::critical(this, "Load Picks", "Loading Picks failed!", QMessageBox::Ok);
        return;
    }

    auto picker=gatherView->picker();
    picker->setPicks(picks);

    m_picksName = name;
    adjustPickActions();
}


void GatherViewer::on_action_Close_Picks_triggered()
{
    if( !m_project ) return;

    if( !picksSaved()) return;

    gatherView->picker()->setPicks(std::make_shared<Table>());
    m_picksName = QString();
    adjustPickActions();
}

void GatherViewer::on_action_Save_Picks_triggered()
{
    if( !m_project ) return;


    if( m_picksName.isEmpty()){

        // input non-existing name
        for(;;){

            QString name = QInputDialog::getText(this, tr("Save Picks"), tr("Pick Table Name:") );

            if( name.isNull() ) return; // canceled

            if( ! m_project->tableList().contains(name)){
                m_picksName=name;
                break;
            }

            QMessageBox::warning(this, tr("Save Grid"), QString("Grid \"%1\" already exists. Please enter a new name").arg(name));
        }
    }

    if(  m_project->tableList().contains(m_picksName)){
        if( m_project->saveTable(m_picksName, *(gatherView->picker()->picks()) ) ){
            gatherView->picker()->setDirty(false);
        }
        else{
            QMessageBox::critical(this, tr("Save Picks"), tr("Saving picks failed"));
        }
    }
    else{
        if( m_project->addTable(m_picksName, *(gatherView->picker()->picks()) ) ){
            gatherView->picker()->setDirty(false);
        }
        else{
            QMessageBox::critical(this, tr("Save Picks"), tr("Saving picks failed"));
        }
    }



}

void GatherViewer::on_action_New_Picks_triggered()
{
    if( !m_project ) return;

    if( !picksSaved()) return;

    bool ok=false;
    QStringList choices;
    choices<<"one pick per cdp"<<"multiple picks per cdp";
    QString choice=QInputDialog::getItem(this, "New Picks", "Select Table Type:", choices, 0, false, &ok);
    if( choice.isEmpty()  || !ok) return;

    bool multi= (choice == choices[1]);

    gatherView->picker()->newPicks("iline", "xline", multi);
    m_picksName=QString();  // empty name for new grid
    adjustPickActions();
}


void GatherViewer::adjustPickActions(){

    auto picker=gatherView->picker();
    if(!picker) return;
    if(!picker->picks()) return;
    auto multi=picker->picks()->isMulti();

    if(multi){
        m_pickModeActions.value(PickMode::Single)->trigger();
        m_pickTypeActions.value(PickType::Free)->trigger();
    }
    else{   // single pick per cdp
        m_pickModeActions.value(PickMode::Single)->trigger();
        m_pickTypeActions.value(PickType::Free)->trigger();
    }

    m_pickModeActions.value(PickMode::Left)->setEnabled(!multi);
    m_pickModeActions.value(PickMode::Right)->setEnabled(!multi);
    m_pickModeActions.value(PickMode::Outline)->setEnabled(multi);

    m_pickTypeActions.value(PickType::Maximum)->setEnabled(!multi);
    m_pickTypeActions.value(PickType::Minimum)->setEnabled(!multi);
    m_pickTypeActions.value(PickType::Zero)->setEnabled(!multi);

}

void GatherViewer::pickModeSelected(QAction * a){

    PickMode m = toPickMode( a->text() );
    gatherView->picker()->setMode(m);
}


void GatherViewer::pickTypeSelected(QAction * a){

    PickType t = toPickType(a->text());
    gatherView->picker()->setType(t);
}

void GatherViewer::fillModeSelected(QAction * a){

    PickFillMode m = toPickFillMode( a->text() );
    gatherView->picker()->setFillMode(m);
}

void GatherViewer::closeEvent(QCloseEvent *event)
{
    if( !picksSaved()){
        event->ignore();
        return;
    }

    saveSettings();

    sendPoint( SelectionPoint::NONE, PointCode::VIEWER_CURRENT_CDP );  // notify other viewers

    // remove intersections from other viewers
    m_gather=nullptr;
    updateIntersections();
}

void GatherViewer::leaveEvent(QEvent *){

    gatherView->setCursorPosition(SelectionPoint::NONE);
    QString msg=createStatusMessage(-1, 0);
    statusBar()->showMessage(msg);
    sendPoint( SelectionPoint::NONE, PointCode::VIEWER_CURRENT_CDP );
}

void GatherViewer::keyPressEvent(QKeyEvent * event){

    switch(event->key()){
    case Qt::Key_F: emit firstRequested();break;
    case Qt::Key_L: emit lastRequested();break;
    case Qt::Key_N: emit nextRequested();break;
    case Qt::Key_P: emit previousRequested();break;
    case Qt::Key_Plus: zoomIn();break;
    case Qt::Key_Minus: zoomOut();break;
    case Qt::Key_0: zoomFitWindow();break;
    case Qt::Key_Escape: ui->gatherView->picker()->clearBuffer(); ui->gatherView->update();break;       // a hack!!!
    default: BaseViewer::keyPressEvent(event) ;break;       // pass the event for handling it elsewhere
    }
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


QString GatherViewer::createStatusMessage( int tno, qreal secs){

    if( tno<0 || tno>=static_cast<int>(m_gather->size())) return QString();

    const seismic::Trace& trc=(*m_gather)[tno];

    QString message;

    /* old generic version
    for( auto anno : m_traceAnnotations ){

        //message += anno.second + "=" + toQString( trc.header().at( anno.first) ) + ", ";
        message += anno.second + "=" + QString::number( trc.header().at( anno.first).toFloat() ) + ", ";
    }
    */

    int iline=trc.header().at("iline").intValue();
    int xline=trc.header().at("xline").intValue();
    float offset=trc.header().at("offset").floatValue();

    message+=QString::asprintf("iline=%d, xline=%d, offset=%.1lf", iline, xline, offset);

    // compute x,y from inline/crossline rather than taking from header from header
    // possible improvement: poststack disp cdpx, cdpy; prestack: sx,sy,gx,gy from header, if not present use computed as fallback
    QTransform IlXlToXY;
    QTransform XYToIlXl;
    if( m_project && m_project->geometry().computeTransforms( XYToIlXl, IlXlToXY)){


            QPointF p=IlXlToXY.map( QPoint(iline, xline));
            qreal x=p.x();
            qreal y=p.y();

            message+=QString::asprintf(", x*=%.1lf, y*=%.1lf, ", x, y);
    }

    size_t i=trc.time_to_index(secs);
    QString amp=(i<trc.size()) ? QString::number(trc[i]) : QString("n/a");
    QString mills=QString::number( int(1000*secs) );

    message+=QString( " Time=") + mills + QString(", Amplitude= ")+ amp;
    if( gatherView->volume()){

        float attr=gatherView->volume()->value(iline, xline, secs);
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
        m_pointDisplayOptionsDialog->setOpacity( gatherLabel->highlightedPointOpacity());

        connect( m_pointDisplayOptionsDialog, SIGNAL( pointSizeChanged(int)),
                 gatherLabel, SLOT(setHighlightedPointSize(int)) );
        connect( m_pointDisplayOptionsDialog, SIGNAL(pointColorChanged(QColor)),
                 gatherLabel, SLOT(setHighlightedPointColor(QColor)) );
        connect( m_pointDisplayOptionsDialog, SIGNAL(opacityChanged(int)),
                 gatherLabel, SLOT(setHighlightedPointOpacity(int)));
    }

    m_pointDisplayOptionsDialog->show();
}


void GatherViewer::on_actionPick_Display_Options_triggered()
{
    if( !m_pickDisplayOptionsDialog){

        m_pickDisplayOptionsDialog=new PointDisplayOptionsDialog(this);
        m_pickDisplayOptionsDialog->setWindowTitle("Configure Pick Display");

        GatherLabel* gatherLabel=gatherView->gatherLabel();

        m_pickDisplayOptionsDialog->setPointSize( gatherLabel->pickSize());
        m_pickDisplayOptionsDialog->setPointColor( gatherLabel->pickColor());
        m_pickDisplayOptionsDialog->setOpacity( gatherLabel->pickOpacity());

        connect( m_pickDisplayOptionsDialog, SIGNAL( pointSizeChanged(int)),
                 gatherLabel, SLOT(setPickSize(int)) );
        connect( m_pickDisplayOptionsDialog, SIGNAL( opacityChanged(int)),
                 gatherLabel, SLOT(setPickOpacity(int)) );
        connect( m_pickDisplayOptionsDialog, SIGNAL(pointColorChanged(QColor)),
                 gatherLabel, SLOT(setPickColor(QColor)) );
    }

    m_pickDisplayOptionsDialog->show();
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
/*
        GridViewer* gv=dynamic_cast<GridViewer*>(v);

        if( gv ){

            if( gv->receptionEnabled() ){
                gv->gridView()->setIntersectionPoints(allTraces);
            }
        }
 */
        v->setIntersectionPoints(allTraces);
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




void GatherViewer::on_actionShare_Current_Position_toggled(bool)
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


bool GatherViewer::picksSaved(){

    if( !gatherView->picker()) return true;

    if( !gatherView->picker()->isDirty() ) return true;

    auto reply = QMessageBox::warning(this, tr("Discard Picks"),
                                      tr("Picks have unsaved changes. Continuing will delete them. Continue?"),
                                        QMessageBox::Yes | QMessageBox::No );
    return reply == QMessageBox::Yes;
}


void GatherViewer::on_actionSeismic_Dataset_Info_triggered()
{
    QString istr=tr("Name:       %1\n").arg(m_datasetInfo.name()) +
                 tr("Domain:     %1\n").arg(datasetDomainToString(m_datasetInfo.domain())) +
                 tr("Mode:       %1\n").arg(datasetModeToString(m_datasetInfo.mode())) +
                 tr("ft:         %1\n").arg(QString::number(m_datasetInfo.ft())) +
                 tr("dt:         %1\n").arg(QString::number(m_datasetInfo.dt())) +
                 tr("nt:         %1\n").arg(QString::number(m_datasetInfo.nt()));

    QMessageBox::information(this, "Dataset Info", istr, QMessageBox::Ok);
}

void GatherViewer::on_actionSetup_Horizons_triggered()
{
    Q_ASSERT(m_project);

    QStringList avail=m_project->gridList(GridType::Horizon);
    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Setup Horizons"), tr("Select Grids:"), avail, &ok, ui->gatherView->horizonList());
    if( !ok ) return;

    // first pass remove items
    for( auto name : ui->gatherView->horizonList() ){

        if( !names.contains(name)) ui->gatherView->removeHorizon(name);
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

        ui->gatherView->addHorizon(name, g);
    }
}

void GatherViewer::on_actionSetup_Volume_triggered()
{
    QStringList avail;
    avail<<"NONE";
    avail.append(m_project->volumeList());
    bool ok=false;
    auto current=0;
    QString name=QInputDialog::getItem(  this, "Setup Volume", "Select Volume:", avail, current, false, &ok);
    if(name.isEmpty() || !ok) return;
    if( name=="NONE"){
        gatherView->setVolume(std::shared_ptr<Volume>());
    }
    else{
        auto volume=m_project->loadVolume(name);
        if( !volume){
            QMessageBox::critical(0, "Load Volume", tr("Loading volume \"%1\" failed!").arg(name));
            return;
        }
        auto frange=volume->valueRange();   // XXX maybe put this into gatherview::setVolume
        gatherView->gatherLabel()->volumeColorTable()->setRange(frange.first, frange.second);
        m_attributeColorBarWidget->setLabel(name);

        // adjust the display range in volume options dialog if open
        if( m_volumeDisplayRangeDialog ){
            m_volumeDisplayRangeDialog->setRange(std::pair<double,double>(frange.first,frange.second));
        }
        gatherView->setVolume(volume);
    }
}

void GatherViewer::on_action_Horizon_Display_Options_triggered()
{
    auto list=gatherView->horizonList();
    QString name;
    if( list.size()<1 ) return;
    if( list.size()==1){
        name=list.front();
    }
    else{
        bool ok=false;
        name=QInputDialog::getItem(this, "Horizon Display Options", "Select Horizon:", list, 0, false, &ok);
        if( name.isEmpty() || !ok) return;
    }

    auto opts=gatherView->gatherLabel()->horizonDisplayOptions(name);
    LineDisplayOptionsDialog dlg(this);
    dlg.setWindowTitle(tr("Setup %1").arg(name));
    dlg.setLineWidth(opts.width);
    dlg.setColor(opts.color);
    dlg.setOpacity(opts.opacity);
    if( dlg.exec()==QDialog::Accepted){
        opts.width=dlg.lineWidth();
        opts.color=dlg.color();
        opts.opacity=dlg.opacity();
        gatherView->gatherLabel()->setHorizonDisplayOptions(name, opts);
    }
}
