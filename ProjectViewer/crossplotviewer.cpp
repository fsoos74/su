#include "crossplotviewer.h"
#include "ui_crossplotviewer.h"

#include<QGraphicsScene>
#include<QGraphicsEllipseItem>
#include<QMessageBox>
#include<QGraphicsSceneHoverEvent>
#include<QSettings>
#include<iostream>

#include<linearregression.h>
#include<linerangeselectiondialog.h>
#include<QGraphicsItem>
#include<QGraphicsSceneMouseEvent>

#include<datapointitem.h>


const int INLINE_DATA_KEY=1;
const int CROSSLINE_DATA_KEY=2;



CrossplotViewer::CrossplotViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::CrossplotViewer)
{
    ui->setupUi(this);

    updateScene();

    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);

    ui->graphicsView->topRuler()->setAnnotationFunction( [](qreal value){return QString::number(value, 'g', 6);} );
    ui->graphicsView->topRuler()->setMinimumPixelIncrement(100);

    ui->graphicsView->setSelectionMode(RulerGraphicsView::SelectionMode::Polygon);

     ui->graphicsView->scale(1,-1); // flip vertical axxis, smallest at bottom, greatest top

    connect( ui->graphicsView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)) );

    connect( ui->action_Receive_CDPs, SIGNAL(toggled(bool)), this, SLOT(setReceptionEnabled(bool)) );
    connect( ui->action_Dispatch_CDPs, SIGNAL(toggled(bool)), this, SLOT(setBroadcastEnabled(bool)) );

    loadSettings();
}

CrossplotViewer::~CrossplotViewer()
{
    delete ui;
}

void CrossplotViewer::receivePoint( QPoint pt ){

    for( QGraphicsItem* item : m_scene->items()){

        DatapointItem* datapointItem=dynamic_cast<DatapointItem*>(item);
        if( datapointItem){
            int iline=datapointItem->data( INLINE_DATA_KEY).toInt();
            int xline=datapointItem->data( CROSSLINE_DATA_KEY).toInt();
            if( iline==pt.x() && xline==pt.y() ){

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

void CrossplotViewer::receivePoints( QVector<QPoint> points, int code){

    if( code != CODE_SINGLE_POINTS) return;

    // only use first point now because current algorithm is n**2 !!!

    if( !points.empty()){
        receivePoint(points.front());
    }

}


void CrossplotViewer::setData( std::shared_ptr<Grid2D<QPointF> > data){

    if( data==m_data) return;

    m_data=data;
    m_geometryBounds=m_data->bounds();
    computeTrend();

    emit dataChanged();

    updateScene();
}

void CrossplotViewer::setAxisLabels( const QString& xAxisAnnotation, const QString& yAxisAnnotation ){

    ui->graphicsView->topRuler()->setLabel(xAxisAnnotation);
    ui->graphicsView->leftRuler()->setLabel(yAxisAnnotation);
}


void CrossplotViewer::setGeometryBounds(Grid2DBounds bounds){

    if( bounds==m_geometryBounds) return;

    m_geometryBounds=bounds;

    updateScene();
}

void CrossplotViewer::setFlattenTrend(bool on){

    if( on==m_flattenTrend) return;

    m_flattenTrend=on;

    ui->graphicsView->setSelectionPolygon(QPolygonF()); // clear selection

    updateScene();
}

void CrossplotViewer::setDatapointSize(int size){

    if( size==m_datapointSize) return;

    m_datapointSize=size;

    ui->graphicsView->setSelectionPolygon(QPolygonF()); // clear selection

    updateScene();
}

void CrossplotViewer::onMouseOver(QPointF scenePos){

    QString message=QString("x=%1, y=%2").arg(scenePos.x()).arg(scenePos.y());

    QPoint viewPos=ui->graphicsView->mapFromScene(scenePos);
    QGraphicsItem* item=ui->graphicsView->itemAt(viewPos);
    if( item ){

        DatapointItem* datapointItem=dynamic_cast<DatapointItem*>(item);
        if( datapointItem){
            int inlineNumber=datapointItem->data(INLINE_DATA_KEY).toInt();
            int crosslineNumber=datapointItem->data(CROSSLINE_DATA_KEY).toInt();
            message.append(QString(", Inline=%1, Crossline=%2").arg(inlineNumber).arg(crosslineNumber));
        }

    }

    statusBar()->showMessage(message);
}

template<class RECT>
void dumpRect( const RECT& rect ){
    std::cout<<"Rect: x="<<rect.x()<<" y="<<rect.y()<<" w="<<rect.width()<<" h="<<rect.height()<<std::endl;
}

void CrossplotViewer::updateScene(){

    const qreal ACTIVE_SIZE_FACTOR=2;           // datapoints grow by this factor on hoover enter events

    if( !m_data ) return;

    QGraphicsScene* scene=new QGraphicsScene(this);

    // ellipse size in pixel, we are scaled to pixel on init and forbit further scaling of items
    int w=12;
    int h=12;
    QPen thePen(Qt::black,0);

    qreal minX=std::numeric_limits<qreal>::max();
    qreal maxX=std::numeric_limits<qreal>::lowest();
    qreal minY=std::numeric_limits<qreal>::max();
    qreal maxY=std::numeric_limits<qreal>::lowest();

    for( int i=m_geometryBounds.i1(); i<=m_geometryBounds.i2();i++ ){

        for( int j=m_geometryBounds.j1(); j<=m_geometryBounds.j2(); j++){

            if( !m_data->bounds().isInside(i,j)) continue;

            QPointF pt=(*m_data)(i,j);
            if( pt==m_data->NULL_VALUE) continue;

            DatapointItem* item=new DatapointItem( m_datapointSize, ACTIVE_SIZE_FACTOR);
            item->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
            item->setFlag(QGraphicsItem::ItemIsSelectable, true);

            if( m_flattenTrend){

                qreal dy=m_trend.x() + pt.x()*m_trend.y();
                pt.setY(pt.y()-dy);
            }

            item->setPos( pt );


            // add user data inline and crossline
            item->setData( INLINE_DATA_KEY, i);
            item->setData( CROSSLINE_DATA_KEY, j);

            scene->addItem(item);

            if(pt.x()<minX) minX=pt.x();
            if(pt.x()>maxX) maxX=pt.x();
            if(pt.y()<minY) minY=pt.y();
            if(pt.y()>maxY) maxY=pt.y();
        }
    }

    QRectF sceneRect=QRectF(minX,minY, maxX-minX, maxY-minY);
    // scene->itemsBoundingRect() does not work, maybe because items ignore transformations and not connected to view?

    //std::cout<<"SceneRect: ";dumpRect(sceneRect);

    qreal xMargin=sceneRect.width()*X_PADDING_FACTOR;
    qreal yMargin=sceneRect.height()*Y_PADDING_FACTOR;
    sceneRect=sceneRect.marginsAdded(QMarginsF(xMargin, yMargin, xMargin, yMargin));
    scene->setSceneRect(sceneRect);
    //std::cout<<"SceneRect width margins: ";dumpRect(sceneRect);

    m_scene=scene;
    connect( m_scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()) );
    ui->graphicsView->setScene(m_scene);

    ui->graphicsView->zoomFitWindow(); // need this, otherwise rulers are not updated!!!
}


void CrossplotViewer::computeTrend(){

    if( !m_data ){
        m_trend=QPointF(0,0);
        return;
    }

    QVector<QPointF > points;

    for( int i=m_geometryBounds.i1(); i<=m_geometryBounds.i2();i++ ){

        for( int j=m_geometryBounds.j1(); j<=m_geometryBounds.j2(); j++){

            if( !m_data->bounds().isInside(i,j)) continue;

            QPointF pt=(*m_data)(i,j);
            if( pt==m_data->NULL_VALUE) continue;

            points.push_back(pt);
        }
    }

    m_trend=linearRegression(points);
}

void CrossplotViewer::sceneSelectionChanged(){

    QVector<QPoint> points;
    points.reserve( m_scene->selectedItems().size() );

    for( QGraphicsItem* item : m_scene->selectedItems() ){

        int iline=item->data( INLINE_DATA_KEY).toInt();
        int xline=item->data( CROSSLINE_DATA_KEY).toInt();
        points.push_back(QPoint(iline, xline));
    }

    // this is for single selected point via double click
    if( points.size()==1){
        sendPoint(points.front());
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



void CrossplotViewer::on_actionSelect_Data_by_Geometry_triggered()
{
    LineRangeSelectionDialog dlg(this);

    dlg.setMinInline(m_geometryBounds.i1());
    dlg.setMaxInline(m_geometryBounds.i2());
    dlg.setMinCrossline(m_geometryBounds.j1());
    dlg.setMaxCrossline(m_geometryBounds.j2());

    if( dlg.exec()==QDialog::Accepted){

        Grid2DBounds bounds( dlg.minInline(), dlg.minCrossline(), dlg.maxInline(), dlg.maxCrossline() );
        setGeometryBounds(bounds);
    }
}

void CrossplotViewer::on_actionDisplay_Options_triggered()
{
    if( !displayOptionsDialog){

        displayOptionsDialog=new CrossplotViewerDisplayOptionsDialog(this);
        displayOptionsDialog->setDatapointSize(m_datapointSize);
        displayOptionsDialog->setFlattenTrend(m_flattenTrend);
        displayOptionsDialog->setWindowTitle("Crossplot Display Options");
        connect( displayOptionsDialog, SIGNAL(datapointSizeChanged(int)), this, SLOT(setDatapointSize(int)) );
        connect( displayOptionsDialog, SIGNAL(flattenTrendChanged(bool)), this, SLOT(setFlattenTrend(bool)) );
    }

    displayOptionsDialog->show();
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
