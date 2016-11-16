
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

#include<mousemodeselector.h>

#include <volumedataselectiondialog.h>
#include <colortabledialog.h>

#include<histogram.h>
#include<histogramdialog.h>

#include "edittrenddialog.h"

#include<cmath>

const int DATA_INDEX_KEY=1;

CrossplotViewer::CrossplotViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::CrossplotViewer),
    m_colorTable(new ColorTable(this))
{
    ui->setupUi(this);

    updateScene();


    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);

    ui->graphicsView->topRuler()->setAnnotationFunction( [](qreal value){return QString::number(value, 'g', 6);} );
    ui->graphicsView->topRuler()->setMinimumPixelIncrement(100);

    ui->graphicsView->setSelectionMode(RulerGraphicsView::SelectionMode::Polygon);

     ui->graphicsView->scale(1,-1); // flip vertical axxis, smallest at bottom, greatest top

     ui->graphicsView->setMouseTracking(true);  // also send mouse move events when no button is pressed
    connect( ui->graphicsView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)) );

    connect( ui->action_Receive_CDPs, SIGNAL(toggled(bool)), this, SLOT(setReceptionEnabled(bool)) );
    connect( ui->action_Dispatch_CDPs, SIGNAL(toggled(bool)), this, SLOT(setBroadcastEnabled(bool)) );

    //connect( ui->action_Flatten_Trend, SIGNAL(toggled(bool)), this, SLOT(updateScene()) );
    connect( ui->actionDisplay_Trend_Line, SIGNAL(toggled(bool)), this, SLOT(updateScene()) );

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(updateScene()) );
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)),
             this, SLOT(updateScene()) );


    m_colorTable->setColors(ColorTable::defaultColors());

    MouseModeSelector* mm=new MouseModeSelector(this);
    connect( mm, SIGNAL(modeChanged(MouseMode)), ui->graphicsView, SLOT(setMouseMode(MouseMode)));
    //QToolBar* mouseToolBar=new QToolBar(this);
    //mouseToolBar->setWindowTitle("Mouse Mode Toolbar");
    ui->mouseToolBar->addWidget( mm);
    //insertToolBar( ui->toolBar, mouseToolBar);

    createDockWidgets();
    populateWindowMenu();

    loadSettings();
}

void CrossplotViewer::populateWindowMenu(){

    ui->menu_Window->addAction(m_attributeColorBarDock->toggleViewAction());
    ui->menu_Window->addSeparator();
    ui->menu_Window->addAction( ui->mouseToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->zoomToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->mainToolBar->toggleViewAction());
}

CrossplotViewer::~CrossplotViewer()
{
    delete ui;
}

bool CrossplotViewer::isFlattenTrend(){
    return ui->action_Flatten_Trend->isChecked();
}

bool CrossplotViewer::isDisplayTrendLine(){
    return ui->actionDisplay_Trend_Line->isChecked();
}

bool CrossplotViewer::isDetailedPointInformation(){
    return ui->actionDetailed_Point_Information->isChecked();
}

void CrossplotViewer::receivePoint( SelectionPoint pt, int code ){

    if( code!=PointCode::VIEWER_POINT_SELECTED) return; // only handle this for now

    if( pt.iline==SelectionPoint::NO_LINE || pt.xline==SelectionPoint::NO_LINE) return;  // only proceed on valid inline and crossline
                                                                                         // this is for selecting lines on sidelabels

    for( QGraphicsItem* item : m_scene->items()){

        DatapointItem* datapointItem=dynamic_cast<DatapointItem*>(item);
        if( datapointItem){
            int idx=datapointItem->data( DATA_INDEX_KEY ).toInt();
            const crossplot::DataPoint& d=m_data.at(idx);
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
    ui->graphicsView->setSelectionPolygon(QPolygonF());
}

void CrossplotViewer::receivePoints( QVector<SelectionPoint> points, int code){

    if( code != CODE_SINGLE_POINTS) return;

    // only use first point now because current algorithm is n**2 !!!

    if( !points.empty()){
        receivePoint(points.front(), code);
    }

}

void CrossplotViewer::scanBounds(){

    // better add progress dialog

    int minil=std::numeric_limits<int>::max();
    int maxil=std::numeric_limits<int>::min();
    int minxl=std::numeric_limits<int>::max();
    int maxxl=std::numeric_limits<int>::min();
    float mint=std::numeric_limits<float>::max();
    float maxt=std::numeric_limits<float>::lowest();

    for( crossplot::DataPoint point:m_data){
        if(point.iline<minil) minil=point.iline;
        if(point.iline>maxil) maxil=point.iline;
        if(point.xline<minxl) minxl=point.xline;
        if(point.xline>maxxl) maxxl=point.xline;
        if(point.time<mint) mint=point.time;
        if(point.time>maxt) maxt=point.time;
    }

    m_geometryBounds=Grid2DBounds(minil, minxl, maxil, maxxl);
    m_timeRange=Range<float>(mint, maxt);
}

void CrossplotViewer::scanAttribute(){

    if(m_data.empty()) return;

    float mina=m_data[0].attribute;
    float maxa=mina;

    for( int i=1; i<m_data.size(); i++ ){
            float &a=m_data[i].attribute;
            if(a<mina) mina=a;
            if(a>maxa) maxa=a;
    }

    m_attributeRange=Range<float>(mina, maxa);
}

void CrossplotViewer::setData( crossplot::Data data){

    const int ASK_SELECT_LINES_LIMIT=200000;

    m_data=data;

    scanBounds();

    scanAttribute();

    if( m_data.size() > ASK_SELECT_LINES_LIMIT ){
        int ret=QMessageBox::question(this, "Crossplot Attributes",
            QString("Number of datapoints is %1. This will significantly slow down operation.\n"
                    "Select inline/crossline ranges to reduce the number of plotted points?").arg(m_data.size()),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

        if( ret==QMessageBox::Yes ){
           ui->actionSelect_By_Inline_Crossline_Ranges->trigger();
        }

    }



    m_colorTable->setRange(m_attributeRange.minimum, m_attributeRange.maximum);  // this triggers updateScene by emitting rangeChanged

    emit dataChanged();

    //updateScene();
}


void CrossplotViewer::setDetailedPointInformation(bool on){

    ui->actionDetailed_Point_Information->setChecked(on);
}

void CrossplotViewer::setTrend(QPointF t){

    if( t==m_trend) return;

    m_trend=t;

    // remove selection polygon because it refers to other transform
    // XXX better transform the polygon
    if( isFlattenTrend() ){
        ui->graphicsView->setSelectionPolygon(QPolygonF());
    }

    updateScene();
}

void CrossplotViewer::setAxisLabels( const QString& xAxisAnnotation, const QString& yAxisAnnotation ){

    ui->graphicsView->topRuler()->setLabel(xAxisAnnotation);
    ui->graphicsView->leftRuler()->setLabel(yAxisAnnotation);
}


void CrossplotViewer::setRegion(Grid2DBounds bounds, Range<float> timerange){

    if( bounds==m_geometryBounds && timerange==m_timeRange) return;

    m_geometryBounds=bounds;
    m_timeRange=timerange;

    updateScene();
}

void CrossplotViewer::setFlattenTrend(bool on){

    if( on==isFlattenTrend()) return;

    ui->graphicsView->setSelectionPolygon(QPolygonF());

    ui->action_Flatten_Trend->setChecked(on);
}

void CrossplotViewer::setDisplayTrendLine(bool on){

    if( on==isDisplayTrendLine()) return;

    ui->actionDisplay_Trend_Line->setChecked(on);
}

void CrossplotViewer::setDatapointSize(int size){

    if( size==m_datapointSize) return;

    m_datapointSize=size;

    ui->graphicsView->setSelectionPolygon(QPolygonF()); // clear selection

    updateScene();
}

void CrossplotViewer::setFixedColor(bool fixed){

    if( fixed==m_fixedColor) return;

    m_fixedColor=fixed;

    emit fixedColorChanged(m_fixedColor);

    updateScene();
}

void CrossplotViewer::setPointColor(QColor color){

    if( color==m_pointColor) return;

    m_pointColor=color;

    emit pointColorChanged(m_pointColor);

    updateScene();
}

void CrossplotViewer::setColorMapping( const std::pair<double,double>& m){
    Q_ASSERT( m_colorTable);
    m_colorTable->setRange(m);
}

void CrossplotViewer::setColors(const QVector<QRgb>& c){

    Q_ASSERT( m_colorTable);
    m_colorTable->setColors(c);
}

void CrossplotViewer::setTrendlineColor(QColor color){

    if( color==m_trendlineColor) return;

    m_trendlineColor=color;

    emit trendlineColorChanged(m_trendlineColor);

    updateScene();
}

void CrossplotViewer::onMouseOver(QPointF scenePos){

   // QString message=QString("x=%1, y=%2").arg(scenePos.x()).arg(scenePos.y());
   QString message=QString("%1=%2,  %3=%4").arg(ui->graphicsView->topRuler()->label()).arg(scenePos.x()).
           arg(ui->graphicsView->leftRuler()->label()).arg(scenePos.y());


   // THIS IS TOO SLOW FOR MANY ITEMS therefore only do it on user request
    if( ui->actionDetailed_Point_Information->isChecked() ){

        QPoint viewPos=ui->graphicsView->mapFromScene(scenePos);
        QGraphicsItem* item=ui->graphicsView->itemAt(viewPos);

        if( item ){

            DatapointItem* datapointItem=dynamic_cast<DatapointItem*>(item);
            int index=datapointItem->data(DATA_INDEX_KEY).toInt();
            if( index>0 && index<m_data.size() ){
                crossplot::DataPoint p = m_data[index];

                message.append(QString(",  Inline=%1,  Crossline=%2,  Time[ms]=%3,  Attribute=%4").
                               arg(p.iline).arg(p.xline).arg(1000*p.time).arg(p.attribute));

                SelectionPoint sp(p.iline, p.xline, p.time);
                sendPoint( sp, PointCode::VIEWER_CURRENT_CDP);
            }
        }

    }

    statusBar()->showMessage(message);
}

template<class RECT>
void dumpRect( const RECT& rect ){
    std::cout<<"Rect: x="<<rect.x()<<" y="<<rect.y()<<" w="<<rect.width()<<" h="<<rect.height()<<std::endl;
}



void CrossplotViewer::updateScene(){

    if( m_data.size()<1){
        return;
    }

    const qreal ACTIVE_SIZE_FACTOR=2;           // datapoints grow by this factor on hoover enter events

    QGraphicsScene* scene=new QGraphicsScene(this);


    // ellipse size in pixel, we are scaled to pixel on init and forbit further scaling of items
    int w=12;
    int h=12;
    QPen thePen(Qt::black,0);

    qreal minX=std::numeric_limits<qreal>::max();
    qreal maxX=std::numeric_limits<qreal>::lowest();
    qreal minY=std::numeric_limits<qreal>::max();
    qreal maxY=std::numeric_limits<qreal>::lowest();

    QProgressDialog progress(this);
    progress.setWindowTitle("Volume Crossplot");
    progress.setRange(0,100);
    progress.setLabelText("Populating Crossplot");
    progress.show();
    int perc=0;

    bool flattenTrend=isFlattenTrend();



    for( int i=0; i<m_data.size(); i++){

        crossplot::DataPoint p = m_data[i];

        // check if point is within the inline/crossline selection for display
        if( !m_geometryBounds.isInside(p.iline,p.xline)) continue;
        // check if point is within time range
        if( !m_timeRange.isInside(p.time)) continue;

        // NULL values are filtered before the data is set, thus they are not checked for

        QPointF pt(p.x, p.y);

        DatapointItem* item=new DatapointItem( m_datapointSize, ACTIVE_SIZE_FACTOR);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
        item->setFlag(QGraphicsItem::ItemIsSelectable, true);

        if( flattenTrend){

            qreal dy=m_trend.x() + pt.x()*m_trend.y();
            pt.setY(pt.y()-dy);

        }

        item->setPos( pt );

        if(m_fixedColor){
            item->setRegularColor(m_pointColor);
        }
        else{
            QRgb rgb=m_colorTable->map(p.attribute);
            item->setRegularColor(rgb);
        }

        // add user data inline and crossline
        //item->setData( INLINE_DATA_KEY, p.iline);
        //item->setData( CROSSLINE_DATA_KEY, p.xline);
        //item->setData( TIME_DATA_KEY, p.time);
        item->setData( DATA_INDEX_KEY, i);
        scene->addItem(item);

        //std::cout<<item->x()<<","<<item->y()<<std::endl;

        if(pt.x()<minX) minX=pt.x();
        if(pt.x()>maxX) maxX=pt.x();
        if(pt.y()<minY) minY=pt.y();
        if(pt.y()>maxY) maxY=pt.y();

        int pp=100*i/m_data.size();
        if(pp>perc){
            perc=pp;
            progress.setValue(perc);
            qApp->processEvents();
            if(progress.wasCanceled()){
                delete scene;
                return;
            }
        }
    }


    QRectF sceneRect=QRectF(minX,minY, maxX-minX, maxY-minY);
    // scene->itemsBoundingRect() does not work, maybe because items ignore transformations and not connected to view?

    std::cout<<"SceneRect: ";dumpRect(sceneRect);

    qreal xMargin=sceneRect.width()*X_PADDING_FACTOR;
    qreal yMargin=sceneRect.height()*Y_PADDING_FACTOR;
    sceneRect=sceneRect.marginsAdded(QMarginsF(xMargin, yMargin, xMargin, yMargin));
    scene->setSceneRect(sceneRect);
    //std::cout<<"SceneRect width margins: ";dumpRect(sceneRect);

    m_scene=scene;
    connect( m_scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()) );
    ui->graphicsView->setScene(m_scene);

    ui->graphicsView->zoomFitWindow(); // need this, otherwise rulers are not updated!!!

    if( isDisplayTrendLine()){
        double x1=sceneRect.left();
        double x2=sceneRect.right();
        double y1=m_trend.x() + x1*m_trend.y();
        double y2=m_trend.x() + x2*m_trend.y();

        if( flattenTrend){

            y1=y1 - ( m_trend.x() + x1 *m_trend.y() );
            y2=y2 - ( m_trend.x() + x2 *m_trend.y() );
        }
        m_scene->addLine(x1,y1,x2,y2,QPen(m_trendlineColor, 0));
    }
}


void CrossplotViewer::on_actionCompute_Trend_From_Loaded_Data_triggered()
{

    if( ! m_scene ){
        setTrend(QPointF(0,0));
        return;
    }

    QVector<QPointF > points;

    for( crossplot::DataPoint point : m_data){

        points.append(QPointF( point.x, point.y));
    }

    setTrend(linearRegression(points));
}

void CrossplotViewer::on_actionCompute_Trend_From_Displayed_Data_triggered()
{

    QVector<QPointF > points;

    for( QGraphicsItem* item : m_scene->items() ){
        int idx=item->data( DATA_INDEX_KEY ).toInt();
        const crossplot::DataPoint& d=m_data.at(idx);
        //int iline=d.iline;
        //int xline=d.xline;

        points.push_back(QPointF(d.x,d.y));
    }

    setTrend( linearRegression(points) );
}

void CrossplotViewer::on_actionCompute_Trend_From_Selected_Data_triggered()
{

    QVector<QPointF > points;

    for( QGraphicsItem* item : m_scene->selectedItems() ){
        int idx=item->data( DATA_INDEX_KEY ).toInt();
        const crossplot::DataPoint& d=m_data.at(idx);

        points.push_back(QPointF(d.x, d.y));
    }

    setTrend( linearRegression(points) );
}

void CrossplotViewer::on_action_Pick_Trend_triggered()
{
    QPolygonF poly=ui->graphicsView->selectionPolygon();

    if( poly.size()<2 ) return;

    ui->graphicsView->setSelectionPolygon(QPolygonF());
    QVector<QPointF > points;
    for( QPointF p : poly ){
         points.push_back(p);
    }

    setTrend( linearRegression(points) );

}

void CrossplotViewer::on_actionSet_Angle_triggered()
{
    double phi=std::fabs(180.*std::atan( m_trend.y())/M_PI);  // in degrees, must be positive

    EditTrendDialog dlg(this);
    dlg.setAngle(phi);
    dlg.setIntercept(m_trend.x());
    dlg.setWindowTitle(tr("Edit Trend"));

    if( dlg.exec()==QDialog::Accepted){
        phi=-std::tan(M_PI*dlg.angle()/180.);    // negative, angle is clockwise x-axxis to trend
        setTrend(QPointF(dlg.intercept(), phi));
    }
    /*
    bool ok=false;
    double d=QInputDialog::getDouble(this, "Crossplot Angle", "angle [degrees]", phi, 0, 90, 4, &ok );

    if( ok && d!=phi ){
        double y=-std::tan(M_PI*d/180.);    // negative, angle is clockwise x-axxis to trend
        setTrend(QPointF(m_trend.x(), y));
    }
    */
}


void CrossplotViewer::on_action_Flatten_Trend_toggled(bool arg1)
{
    // remove selection polygon because it refers to other transform
    // XXX better transform the polygon
    ui->graphicsView->setSelectionPolygon(QPolygonF());

    updateScene();
}


void CrossplotViewer::sceneSelectionChanged(){

    QVector<SelectionPoint> points;
    points.reserve( m_scene->selectedItems().size() );

    for( QGraphicsItem* item : m_scene->selectedItems() ){

        int idx=item->data( DATA_INDEX_KEY ).toInt();
        const crossplot::DataPoint& d=m_data.at(idx);
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

void CrossplotViewer::on_actionZoom_In_triggered()
{

    ui->graphicsView->zoomBy(ZOOM_IN_FACTOR);

}

void CrossplotViewer::on_actionZoom_Out_triggered()
{
    ui->graphicsView->zoomBy(ZOOM_OUT_FACTOR);

}



void CrossplotViewer::on_actionZoom_Fit_Window_triggered()
{

    ui->graphicsView->zoomFitWindow();
}


void CrossplotViewer::on_actionSelect_By_Inline_Crossline_Ranges_triggered()
{
    VolumeDataSelectionDialog dlg(this);

    dlg.setWindowTitle( "Select Line Ranges and Time Range");

    dlg.setMinInline(m_geometryBounds.i1());
    dlg.setMaxInline(m_geometryBounds.i2());
    dlg.setMinCrossline(m_geometryBounds.j1());
    dlg.setMaxCrossline(m_geometryBounds.j2());
    dlg.setMinTime(m_timeRange.minimum);
    dlg.setMaxTime(m_timeRange.maximum);

    if( dlg.exec()==QDialog::Accepted){

        Grid2DBounds bounds( dlg.minInline(), dlg.minCrossline(),
                             dlg.maxInline(), dlg.maxCrossline() );
        Range<float> timerange=Range<float>( dlg.minTime(), dlg.maxTime() );

        setRegion( bounds, timerange);
    }
}

void CrossplotViewer::on_actionDisplay_Options_triggered()
{
    if( !displayOptionsDialog){

        displayOptionsDialog=new CrossplotViewerDisplayOptionsDialog(this);
        displayOptionsDialog->setDatapointSize(m_datapointSize);
        displayOptionsDialog->setFixedColor(m_fixedColor);
        displayOptionsDialog->setPointColor(m_pointColor);
        displayOptionsDialog->setTrendlineColor(m_trendlineColor);

        displayOptionsDialog->setWindowTitle("Crossplot Display Options");

        connect( displayOptionsDialog, SIGNAL(datapointSizeChanged(int)),
                 this, SLOT(setDatapointSize(int)) );
        connect( displayOptionsDialog, SIGNAL(fixedColorChanged(bool)),
                 this, SLOT(setFixedColor(bool)) );
        connect( displayOptionsDialog, SIGNAL( pointColorChanged(QColor)),
                 this, SLOT( setPointColor(QColor)) );
        connect( displayOptionsDialog, SIGNAL(trendlineColorChanged(QColor)),
                this, SLOT(setTrendlineColor(QColor)) );
    }

    displayOptionsDialog->show();
}

void CrossplotViewer::on_actionAttribute_Colortable_triggered()
{
    QVector<QRgb> oldColors=m_colorTable->colors();

    ColorTableDialog* colorTableDialog=new ColorTableDialog( oldColors);

    Q_ASSERT(colorTableDialog);

    connect( colorTableDialog, SIGNAL(colorsChanged(QVector<QRgb>)),
             m_colorTable, SLOT(setColors(QVector<QRgb>)));

    if( colorTableDialog->exec()==QDialog::Accepted ){
        m_colorTable->setColors( colorTableDialog->colors());
    }else{
        m_colorTable->setColors( oldColors );
    }

    delete colorTableDialog;
}

void CrossplotViewer::on_actionAttribute_Range_triggered()
{
    if(!displayRangeDialog){

        displayRangeDialog=new DisplayRangeDialog(this);

        displayRangeDialog->setPower(m_colorTable->power());
        displayRangeDialog->setRange(m_colorTable->range());
        connect( displayRangeDialog, SIGNAL(rangeChanged(std::pair<double,double>)),
                 m_colorTable, SLOT(setRange(std::pair<double,double>)) );
        connect( displayRangeDialog, SIGNAL(powerChanged(double)),
                 m_colorTable, SLOT( setPower(double)) );
    }

    displayRangeDialog->show();
}



QVector<double> CrossplotViewer::collectHistogramData( std::function<double(const crossplot::DataPoint&)> f ){

    QVector<double> data;

    if( m_scene->selectedItems().empty()){
        for( crossplot::DataPoint point : m_data){
            // should be no NULL values here, they must be filterd out before
            data.push_back( f(point));
         }
    }
    else{
        for( QGraphicsItem* item : m_scene->selectedItems() ){
            int idx=item->data( DATA_INDEX_KEY ).toInt();
            const crossplot::DataPoint& point=m_data.at(idx);
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

    viewer->setColorTable( m_colorTable );        // the colortable must have same parent as viewer, maybe used shared_ptr!!!
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
    m_attributeColorBarWidget->setColorTable( m_colorTable );

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

        settings.setValue("datapoint-size", m_datapointSize);

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

          setDatapointSize( settings.value("datapoint-size", 7 ).toInt());

    settings.endGroup();
}


