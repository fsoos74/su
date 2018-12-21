
#define _USE_MATH_DEFINES // need this for M_PI with mcvc !!!! must be before includes !!!

#include "crossplotviewer.h"
#include "ui_crossplotviewer.h"


#include<QGraphicsScene>
#include<QGraphicsEllipseItem>
#include<QMessageBox>
#include<QGraphicsSceneHoverEvent>
#include<QProgressDialog>
#include<QSettings>
#include<QInputDialog>
#include<iostream>

#include<linearregression.h>
#include<linerangeselectiondialog.h>
#include<QGraphicsItem>
#include<QGraphicsSceneMouseEvent>
#include<datapointitem.h>

#include<dynamicmousemodeselector.h>

#include <volumedataselectiondialog.h>
#include <colortabledialog.h>

#include<histogram.h>
#include<histogramdialog.h>

#include "edittrenddialog.h"
#include "crossplotfilterdialog.h"


#include<cmath>


CrossplotViewer::CrossplotViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::CrossplotViewer)
{
    ui->setupUi(this);

    ui->crossplotView->setZoomMode(AxisView::ZOOMMODE::BOTH);
    ui->crossplotView->setCursorMode(AxisView::CURSORMODE::BOTH);
    ui->crossplotView->setDragMode(QGraphicsView::RubberBandDrag);
    ui->crossplotView->setSelectionMode(AxisView::SELECTIONMODE::Polygon);
    ui->crossplotView->setXMesh(true);
    ui->crossplotView->setZMesh(true);
     //ui->crossplotView->setMouseTracking(true);  // also send mouse move events when no button is pressed
    connect( ui->crossplotView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)) );

    connect( ui->action_Receive_CDPs, SIGNAL(toggled(bool)), this, SLOT(setReceptionEnabled(bool)) );
    connect( ui->action_Dispatch_CDPs, SIGNAL(toggled(bool)), this, SLOT(setBroadcastEnabled(bool)) );

    connect( ui->actionDisplay_Trend_Line, SIGNAL(toggled(bool)), ui->crossplotView, SLOT(setDisplayTrendLine(bool)) );
    connect( ui->action_Flatten_Trend, SIGNAL(toggled(bool)), ui->crossplotView, SLOT(setFlattenTrend(bool)) );

    connect( ui->actionZoom_In, SIGNAL(triggered(bool)), ui->crossplotView, SLOT(zoomIn()) );
    connect( ui->actionZoom_Out, SIGNAL(triggered(bool)), ui->crossplotView, SLOT(zoomOut()) );
    connect( ui->actionZoom_Fit_Window, SIGNAL(triggered(bool)), ui->crossplotView, SLOT(zoomFitWindow()) );

    connect( ui->crossplotView, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()) );

    connect( ui->crossplotView, SIGNAL(lineSelected(QLineF)), this, SLOT(onTrendLineSelected(QLineF)) );

    setupMouseModes();
    createDockWidgets();
    populateWindowMenu();

    loadSettings();
}


CrossplotView* CrossplotViewer::view(){
    return ui->crossplotView;
}

void CrossplotViewer::populateWindowMenu(){

    ui->menu_Window->addAction(m_attributeColorBarDock->toggleViewAction());
    ui->menu_Window->addSeparator();
    ui->menu_Window->addAction( ui->mouseToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->zoomToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->mainToolBar->toggleViewAction());
}

void CrossplotViewer::setupMouseModes(){

    DynamicMouseModeSelector* mm=new DynamicMouseModeSelector(this);
    connect( mm, SIGNAL(modeChanged(MouseMode)), ui->crossplotView, SLOT(setMouseMode(MouseMode)));
    mm->addMode(MouseMode::Explore);
    mm->addMode(MouseMode::Zoom);
    mm->addMode(MouseMode::Select);
    ui->mouseToolBar->addWidget( mm);

}

CrossplotViewer::~CrossplotViewer()
{
    delete ui;
}

bool CrossplotViewer::isDetailedPointInformation(){
    return ui->actionDetailed_Point_Information->isChecked();
}

void CrossplotViewer::receivePoint( SelectionPoint pt, int code ){

    if( code!=PointCode::VIEWER_POINT_SELECTED) return; // only handle this for now

    if( pt.iline==SelectionPoint::NO_LINE || pt.xline==SelectionPoint::NO_LINE) return;  // only proceed on valid inline and crossline
                                                                                         // this is for selecting lines on sidelabels

    for( QGraphicsItem* item : ui->crossplotView->scene()->items()){

        DatapointItem* datapointItem=dynamic_cast<DatapointItem*>(item);
        if( datapointItem){
            int idx=datapointItem->data( ui->crossplotView->DATA_INDEX_KEY ).toInt();
            const crossplot::DataPoint& d=ui->crossplotView->data().at(idx);
            int iline=d.iline;
            int xline=d.xline;

            if( iline==pt.iline && xline==pt.xline ){

                item->setZValue(1);
                item->setSelected(true);
            }
            else{

                item->setZValue(0);
                item->setSelected(false);
            }
        }

    }

    // clear selection polygon
    ui->crossplotView->setSelectionPolygon(QPolygonF());
}

void CrossplotViewer::receivePoints( QVector<SelectionPoint> points, int code){

    if( code != CODE_SINGLE_POINTS) return;

    // only use first point now because current algorithm is n**2 !!!

    if( !points.empty()){
        receivePoint(points.front(), code);
    }

}


void CrossplotViewer::setData( crossplot::Data data){

    const int ASK_SELECT_LINES_LIMIT=200000;

    if( data.size() > ASK_SELECT_LINES_LIMIT ){
        int ret=QMessageBox::question(this, "Crossplot Attributes",
            QString("Number of datapoints is %1. This will significantly slow down operation.\n"
                    "Select inline/crossline ranges to reduce the number of plotted points?").arg(ui->crossplotView->data().size()),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

        if( ret==QMessageBox::Yes ){
           ui->actionSelect_By_Inline_Crossline_Ranges->trigger();
        }
    }

    ui->crossplotView->setData(data);
}

void CrossplotViewer::setDetailedPointInformation(bool on){

    ui->actionDetailed_Point_Information->setChecked(on);
}

void CrossplotViewer::setInlinesSelectable(bool on){
    m_inlinesSelectable=on;
}

void CrossplotViewer::setCrosslinesSelectable(bool on){
    m_crosslinesSelectable=on;
}

void CrossplotViewer::setMSecsSelectable(bool on){
    m_msecsSelectable=on;
}

void CrossplotViewer::setAxisLabels( const QString& xAxisAnnotation, const QString& yAxisAnnotation ){
    ui->crossplotView->xAxis()->setName(xAxisAnnotation);
    ui->crossplotView->zAxis()->setName(yAxisAnnotation);
}


void CrossplotViewer::onTrendLineSelected(QLineF axisline){

    QVector<QPointF> points;
    points<<axisline.p1();
    points<<axisline.p2();
    auto trend=linearRegression(points);
    ui->crossplotView->setTrend(trend);
    ui->crossplotView->setSelectionMode(AxisView::SELECTIONMODE::Polygon);  // only select one line, after switch back to polygon selection!
}

void CrossplotViewer::onMouseOver(QPointF axisPos){

   QString message=QString("x=%1, z=%2").arg(axisPos.x()).arg(axisPos.y());
   //QString message=QString("%1=%2,  %3=%4").arg(ui->crossplotView->topRuler()->label()).arg(scenePos.x()).
   //        arg(ui->crossplotView->leftRuler()->label()).arg(scenePos.y());

/*
   // THIS IS TOO SLOW FOR MANY ITEMS therefore only do it on user request
    if( ui->actionDetailed_Point_Information->isChecked() ){

        QPoint viewPos=ui->crossplotView->mapFromScene(scenePos);
        QGraphicsItem* item=ui->crossplotView->itemAt(viewPos);

        if( item ){

            DatapointItem* datapointItem=dynamic_cast<DatapointItem*>(item);
            int index=datapointItem->data(DATA_INDEX_KEY).toInt();
            if( index>0 && index<ui->crossplotView->data().size() ){
                crossplot::DataPoint p = ui->crossplotView->data()[index];

                message.append(QString(",  Inline=%1,  Crossline=%2,  Time[ms]=%3,  Attribute=%4").
                               arg(p.iline).arg(p.xline).arg(1000*p.time).arg(p.attribute));

                SelectionPoint sp(p.iline, p.xline, p.time);
                sendPoint( sp, PointCode::VIEWER_CURRENT_CDP);
            }
        }

    }
*/
    statusBar()->showMessage(message);
}

template<class RECT>
void dumpRect( const RECT& rect ){
    std::cout<<"Rect: x="<<rect.x()<<" y="<<rect.y()<<" w="<<rect.width()<<" h="<<rect.height()<<std::endl;
}


void CrossplotViewer::on_actionCompute_Trend_From_Loaded_Data_triggered()
{
    QVector<QPointF > points;

    for( crossplot::DataPoint point : ui->crossplotView->data()){

        points.append(QPointF( point.x, point.y));
    }

    auto trend=linearRegression(points);

    ui->crossplotView->setTrend(trend);
}

void CrossplotViewer::on_actionCompute_Trend_From_Displayed_Data_triggered()
{

    QVector<QPointF > points;

    for( QGraphicsItem* item : ui->crossplotView->scene()->items() ){
        int idx=item->data( ui->crossplotView->DATA_INDEX_KEY ).toInt();
        const crossplot::DataPoint& d=ui->crossplotView->data().at(idx);

        points.push_back(QPointF(d.x,d.y));
    }

    auto trend=linearRegression(points);

    ui->crossplotView->setTrend(trend);
}

void CrossplotViewer::on_actionCompute_Trend_From_Selected_Data_triggered()
{

    QVector<QPointF > points;

    for( QGraphicsItem* item : ui->crossplotView->scene()->selectedItems() ){
        int idx=item->data( ui->crossplotView->DATA_INDEX_KEY ).toInt();
        const crossplot::DataPoint& d=ui->crossplotView->data().at(idx);

        points.push_back(QPointF(d.x, d.y));
    }

    auto trend=linearRegression(points);

    ui->crossplotView->setTrend(trend);
}

void CrossplotViewer::on_action_Pick_Trend_triggered()
{
    ui->crossplotView->setMouseMode(MouseMode::Select);
    ui->crossplotView->setSelectionMode(AxisView::SELECTIONMODE::Line);
}

void CrossplotViewer::on_actionSet_Angle_triggered()
{
    auto t=ui->crossplotView->trend();
    //double phi=std::fabs(180.*std::atan( ui->crossplotView->trend().y())/M_PI);  // in degrees, must be positive
    double phi=180.*std::atan( ui->crossplotView->trend().y())/M_PI;  // in degrees, [WHY???must be positive]

    EditTrendDialog dlg(this);
    dlg.setAngle(phi);
    dlg.setIntercept(ui->crossplotView->trend().x());
    dlg.setWindowTitle(tr("Edit Trend"));

    if( dlg.exec()==QDialog::Accepted){
        //phi=-std::tan(M_PI*dlg.angle()/180.);    // negative, angle is clockwise x-axxis to trend
        phi=std::tan(M_PI*dlg.angle()/180.);    // WHY negative?? negative, angle is clockwise x-axxis to trend
        QPointF t(dlg.intercept(), phi);
        ui->crossplotView->setTrend(t);
        //ui->crossplotView->setTrend(QPointF(dlg.intercept(), phi));
    }
}

void CrossplotViewer::sceneSelectionChanged(){

    QVector<SelectionPoint> points;
    points.reserve( ui->crossplotView->scene()->selectedItems().size() );

    for( QGraphicsItem* item : ui->crossplotView->scene()->selectedItems() ){

        int idx=item->data( ui->crossplotView->DATA_INDEX_KEY ).toInt();
        const crossplot::DataPoint& d=ui->crossplotView->data().at(idx);
        int iline=d.iline;
        int xline=d.xline;
        float time=d.time;

        points.push_back(SelectionPoint(iline, xline, time));
    }

    // this is for single selected point via double click
    if( points.size()==1){
        sendPoint(points.front(), PointCode::VIEWER_POINT_SELECTED);
    }

    sendPoints(points, CODE_SINGLE_POINTS);
}

void CrossplotViewer::on_actionSelect_By_Inline_Crossline_Ranges_triggered()
{

    auto dims=ui->crossplotView->dimensions();
    auto arange=ui->crossplotView->attributeRange();

    CrossplotFilterDialog dlg;
    auto f=ui->crossplotView->filter();

    dlg.setWindowTitle("Select Displayed Points");
    dlg.setMinInline(std::max(dims.inline1, f.minIL));
    dlg.setMaxInline(std::min(dims.inline2,f.maxIL));
    dlg.setMinCrossline(std::max(dims.crossline1,f.minXL));
    dlg.setMaxCrossline(std::min(dims.crossline2,f.maxXL));
    dlg.setMinZ(std::max(dims.msec1,f.minZ));
    dlg.setMaxZ(std::min(dims.msec2,f.maxZ));
    dlg.setMinAttribute(std::max(static_cast<double>(arange.minimum), f.minA));
    dlg.setMaxAttribute(std::min(static_cast<double>(arange.maximum), f.maxA));
    dlg.setMinDataset(f.minDS);
    dlg.setMaxDataset(f.maxDS);

    if( dlg.exec()){

        ui->crossplotView->setFilter(

                    CrossplotView::Filter{
                        dlg.minInline(), dlg.maxInline(),
                        dlg.minCrossline(), dlg.maxCrossline(),
                        dlg.minZ(), dlg.maxZ(),
                        dlg.minAttribute(), dlg.maxAttribute(),
                        dlg.minDataset(), dlg.maxDataset()
                    }
                    );

    }

}

void CrossplotViewer::on_actionDisplay_Options_triggered()
{
    if( !displayOptionsDialog){

        displayOptionsDialog=new CrossplotViewerDisplayOptionsDialog(this);
        displayOptionsDialog->setPointSize(ui->crossplotView->pointSize());
        displayOptionsDialog->setPointSymbol(ui->crossplotView->pointSymbol());
        displayOptionsDialog->setColorStyle(ui->crossplotView->colorStyle());
        displayOptionsDialog->setPointColor(ui->crossplotView->pointColor());
        displayOptionsDialog->setTrendlineColor(ui->crossplotView->trendlineColor());

        displayOptionsDialog->setWindowTitle("Crossplot Display Options");

        connect( displayOptionsDialog, SIGNAL(pointSizeChanged(int)),
                 ui->crossplotView, SLOT(setPointSize(int)) );
        connect( displayOptionsDialog, SIGNAL(pointSymbolChanged(CrossplotView::Symbol)),
                 ui->crossplotView, SLOT(setPointSymbol(CrossplotView::Symbol)) );
        connect( displayOptionsDialog, SIGNAL(colorStyleChanged(CrossplotView::ColorStyle)),
                 ui->crossplotView, SLOT(setColorStyle(CrossplotView::ColorStyle)));
        connect( displayOptionsDialog, SIGNAL( pointColorChanged(QColor)),
                 ui->crossplotView, SLOT( setPointColor(QColor)) );
        connect( displayOptionsDialog, SIGNAL(trendlineColorChanged(QColor)),
                ui->crossplotView, SLOT(setTrendlineColor(QColor)) );
    }

    displayOptionsDialog->show();
}

void CrossplotViewer::on_actionAttribute_Colortable_triggered()
{
    QVector<QRgb> oldColors=ui->crossplotView->colorTable()->colors();

    ColorTableDialog* colorTableDialog=new ColorTableDialog( oldColors);

    Q_ASSERT(colorTableDialog);

    connect( colorTableDialog, SIGNAL(colorsChanged(QVector<QRgb>)),
             ui->crossplotView->colorTable(), SLOT(setColors(QVector<QRgb>)));

    if( colorTableDialog->exec()==QDialog::Accepted ){
        ui->crossplotView->colorTable()->setColors( colorTableDialog->colors());
    }else{
        ui->crossplotView->colorTable()->setColors( oldColors );
    }

    delete colorTableDialog;
}

void CrossplotViewer::on_actionAttribute_Range_triggered()
{
    if(!displayRangeDialog){

        std::vector< float > tmp;
        tmp.reserve(ui->crossplotView->data().size());
        for( auto p : ui->crossplotView->data() ){
            if( p.attribute!=Grid2D<float>::NULL_VALUE) tmp.push_back(p.attribute);
        }

        displayRangeDialog=new HistogramRangeSelectionDialog(this);

        displayRangeDialog->setInstantUpdates(false);

        displayRangeDialog->setHistogram(createHistogram( std::begin(tmp), std::end(tmp),
                                                          Grid2D<float>::NULL_VALUE, 100 ));
        auto mm = std::minmax_element(tmp.begin(), tmp.end() );

        displayRangeDialog->setColorTable(ui->crossplotView->colorTable() );   // all updating through colortable

        displayRangeDialog->setWindowTitle("Configure Volume Display Range" );
    }

    displayRangeDialog->show();
}



QVector<double> CrossplotViewer::collectHistogramData( std::function<double(const crossplot::DataPoint&)> f ){

    QVector<double> data;

    if( ui->crossplotView->scene()->selectedItems().empty()){
        for( crossplot::DataPoint point : ui->crossplotView->data()){
            // should be no NULL values here, they must be filterd out before
            data.push_back( f(point));
         }
    }
    else{
        for( QGraphicsItem* item : ui->crossplotView->scene()->selectedItems() ){
            int idx=item->data( ui->crossplotView->DATA_INDEX_KEY ).toInt();
            const crossplot::DataPoint& point=ui->crossplotView->data().at(idx);
            data.push_back(f(point));
        }
    }

    return data;
}

void CrossplotViewer::on_action_HistogramXAxis_triggered()
{
    QVector<double> data=collectHistogramData( [](const crossplot::DataPoint& p){ return p.x;} );
    if(data.empty()) return;

    HistogramDialog* viewer=new HistogramDialog;
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1 x-axis").arg(windowTitle() ) );

    viewer->show();

}


void CrossplotViewer::on_action_HistogramYAxis_triggered()
{

    QVector<double> data=collectHistogramData( [](const crossplot::DataPoint& p){ return p.y;} );
    if(data.empty()) return;

    HistogramDialog* viewer=new HistogramDialog;
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1 y-axis").arg(windowTitle() ) );

    viewer->show();

}


void CrossplotViewer::on_action_HistogramAttribute_triggered()
{
    QVector<double> data=collectHistogramData( [](const crossplot::DataPoint& p){ return p.attribute;} );
    if(data.empty()) return;

    HistogramDialog* viewer=new HistogramDialog(this);      // need to make this a parent in order to allow qt to delete this when this is deleted
                                                            // this is important because otherwise the colortable will be deleted before this! CRASH!!!
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1 attribute").arg(windowTitle() ) );

    viewer->setColorTable( ui->crossplotView->colorTable() );        // the colortable must have same parent as viewer, maybe used shared_ptr!!!
    viewer->show();
}


void CrossplotViewer::createDockWidgets(){

    m_attributeColorBarDock = new QDockWidget(tr("Attribute Colorbar"), this);
    m_attributeColorBarDock->setAllowedAreas(Qt::RightDockWidgetArea);
    m_attributeColorBarWidget=new ColorBarWidget(m_attributeColorBarDock);
    m_attributeColorBarWidget->setScaleAlignment(ColorBarWidget::SCALE_LEFT);
    m_attributeColorBarDock->setContentsMargins(10, 5, 10, 5);
    m_attributeColorBarDock->setWidget(m_attributeColorBarWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_attributeColorBarDock);
    m_attributeColorBarWidget->setColorTable( ui->crossplotView->colorTable() );

    m_attributeColorBarDock->close();
}

void CrossplotViewer::closeEvent(QCloseEvent *)
{
    saveSettings();
}

void CrossplotViewer::saveSettings(){

     QSettings settings(COMPANY, "CrossplotViewer");

     settings.beginGroup("CrossplotViewer");
       settings.setValue("size", size());
       //settings.setValue("position", pos() );
     settings.endGroup();

     settings.beginGroup("items");

        settings.setValue("datapoint-size", ui->crossplotView->pointSize());

     settings.endGroup();

     settings.sync();
}


void CrossplotViewer::loadSettings(){

    QSettings settings(COMPANY, "CrossplotViewer");

    settings.beginGroup("CrossplotViewer");
        resize(settings.value("size", QSize(600, 400)).toSize());
        //move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();


    settings.beginGroup("items");

        ui->crossplotView->setPointSize( settings.value("datapoint-size", 7 ).toInt());

    settings.endGroup();
}

