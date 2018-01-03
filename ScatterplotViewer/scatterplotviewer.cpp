
#define _USE_MATH_DEFINES // need this for M_PI with mcvc !!!! must be before includes !!!

#include "scatterplotviewer.h"
#include "ui_scatterplotviewer.h"


#include<QMessageBox>
#include<QProgressDialog>
#include<QInputDialog>
#include<iostream>
#include<QGraphicsSceneMouseEvent>
#include<dynamicmousemodeselector.h>
#include<histogramdialog.h>
#include<multiitemselectiondialog.h>

#include<QGraphicsScene>
#include<QGraphicsEllipseItem>
#include<QGraphicsSimpleTextItem>

ScatterplotViewer::ScatterplotViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScatterplotViewer)
{
    ui->setupUi(this);

    ui->scatterplotView->setZoomMode(AxisView::ZOOMMODE::BOTH);
    ui->scatterplotView->setCursorMode(AxisView::CURSORMODE::BOTH);
    ui->scatterplotView->setDragMode(QGraphicsView::RubberBandDrag);
    ui->scatterplotView->setSelectionMode(AxisView::SELECTIONMODE::Polygon);
    ui->scatterplotView->setXMesh(true);
    ui->scatterplotView->setZMesh(true);
     //ui->ScatterplotView->setMouseTracking(true);  // also send mouse move events when no button is pressed
    connect( ui->scatterplotView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)) );

    connect( ui->actionZoom_In, SIGNAL(triggered(bool)), ui->scatterplotView, SLOT(zoomIn()) );
    connect( ui->actionZoom_Out, SIGNAL(triggered(bool)), ui->scatterplotView, SLOT(zoomOut()) );
    connect( ui->actionZoom_Fit_Window, SIGNAL(triggered(bool)), ui->scatterplotView, SLOT(zoomFitWindow()) );

    //connect( ui->scatterplotView, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()) );

    setupMouseModes();
    createDockWidgets();
    populateWindowMenu();
}


ScatterplotView* ScatterplotViewer::view(){
    return ui->scatterplotView;
}


void ScatterplotViewer::setupMouseModes(){

    DynamicMouseModeSelector* mm=new DynamicMouseModeSelector(this);
    connect( mm, SIGNAL(modeChanged(MouseMode)), ui->scatterplotView, SLOT(setMouseMode(MouseMode)));
    mm->addMode(MouseMode::Explore);
    mm->addMode(MouseMode::Zoom);
    mm->addMode(MouseMode::Select);
    ui->mouseToolBar->addWidget( mm);

}

void ScatterplotViewer::createDockWidgets(){

    m_legendDock = new QDockWidget(tr("Plot Legend"), this);
    m_legendDock->setAllowedAreas(Qt::RightDockWidgetArea);
    m_legendWidget=new QGraphicsView(m_legendDock);
    //m_legendDock->setContentsMargins(10, 5, 10, 5);
    m_legendDock->setWidget(m_legendWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_legendDock);

    //m_legendDock->close();
}


void ScatterplotViewer::updateLegend(){

    const int SymbolSize=10;
    const int LineHeight=20;
    const float Margins=0.2;
    int y = 0;
    QGraphicsScene* scene=new QGraphicsScene;
    for( auto key : ui->scatterplotView->keys()){


        auto symbol=new QGraphicsEllipseItem( -SymbolSize/2, -SymbolSize/2, SymbolSize, SymbolSize);
        symbol->setPen(Qt::NoPen);
        symbol->setBrush(ui->scatterplotView->pointColor(key));
        symbol->setPos( 10, y);
        scene->addItem(symbol);

        auto label = scene->addSimpleText(key);
        label->setPos(30, y - label->boundingRect().height()/2);

        y+=LineHeight;
    }

    m_legendWidget->setScene(scene);
    auto r = scene->itemsBoundingRect();
    r = r.marginsAdded( QMargins(Margins*r.width(), Margins*r.height(), Margins*r.width(), Margins*r.height()) );
    scene->setSceneRect(r);
    m_legendWidget->setFixedWidth(scene->width());
    m_legendWidget->setMaximumHeight(scene->height());
    //m_legendWidget->setSceneRect(scene->itemsBoundingRect());
}

void ScatterplotViewer::populateWindowMenu(){

    ui->menu_Window->addAction(m_legendDock->toggleViewAction());
}


ScatterplotViewer::~ScatterplotViewer()
{
    delete ui;
}

void ScatterplotViewer::setData( QVector<ScatterPlotPoint> data){
    ui->scatterplotView->setData(data);
    updateLegend();
}


void ScatterplotViewer::setAxisLabels( const QString& xAxisAnnotation, const QString& yAxisAnnotation ){
    ui->scatterplotView->xAxis()->setName(xAxisAnnotation);
    ui->scatterplotView->zAxis()->setName(yAxisAnnotation);
}

void ScatterplotViewer::onMouseOver(QPointF axisPos){

   QString message=QString("x=%1, z=%2").arg(axisPos.x()).arg(axisPos.y());

/*
   // THIS IS TOO SLOW FOR MANY ITEMS therefore only do it on user request
    if( ui->actionDetailed_Point_Information->isChecked() ){

        QPoint viewPos=ui->ScatterplotView->mapFromScene(scenePos);
        QGraphicsItem* item=ui->ScatterplotView->itemAt(viewPos);

        if( item ){

            DatapointItem* datapointItem=dynamic_cast<DatapointItem*>(item);
            int index=datapointItem->data(DATA_INDEX_KEY).toInt();
            if( index>0 && index<ui->ScatterplotView->data().size() ){
                Scatterplot::DataPoint p = ui->ScatterplotView->data()[index];

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

void ScatterplotViewer::sceneSelectionChanged(){
/*
    QVector<SelectionPoint> points;
    points.reserve( ui->ScatterplotView->scene()->selectedItems().size() );

    for( QGraphicsItem* item : ui->ScatterplotView->scene()->selectedItems() ){

        int idx=item->data( ui->ScatterplotView->DATA_INDEX_KEY ).toInt();
        const Scatterplot::DataPoint& d=ui->ScatterplotView->data().at(idx);
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
    */
}

void ScatterplotViewer::on_actionDisplay_Options_triggered()
{

    if( !displayOptionsDialog){

        displayOptionsDialog=new ScatterplotViewerDisplayOptionsDialog(this);
        displayOptionsDialog->setPointSize(ui->scatterplotView->pointSize());
        //displayOptionsDialog->setPointColor(ui->scatterplotView->pointColor());

        displayOptionsDialog->setWindowTitle("Scatterplot Display Options");

        connect( displayOptionsDialog, SIGNAL(pointSizeChanged(int)),
                 ui->scatterplotView, SLOT(setPointSize(int)) );
        connect( displayOptionsDialog, SIGNAL( pointOutlineChanged(bool)),
                 ui->scatterplotView, SLOT( setPointOutline(bool)) );
    }

    displayOptionsDialog->show();
}


void ScatterplotViewer::on_action_HistogramXAxis_triggered()
{
    QVector<double> data=collectHistogramData( [](const QPointF& p){ return p.x();} );
    if(data.empty()) return;

    HistogramDialog* viewer=new HistogramDialog;
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1 x-axis").arg(windowTitle() ) );

    viewer->show();
}


void ScatterplotViewer::on_action_HistogramYAxis_triggered()
{
    QVector<double> data=collectHistogramData( [](const QPointF& p){ return p.y();} );
    if(data.empty()) return;

    HistogramDialog* viewer=new HistogramDialog;
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1 y-axis").arg(windowTitle() ) );

    viewer->show();
}


void ScatterplotViewer::closeEvent(QCloseEvent *)
{
}


QVector<double> ScatterplotViewer::collectHistogramData( std::function<double(const QPointF&)> f ){

    QVector<double> data;
    auto displayed = ui->scatterplotView->displayedKeys();

    for( auto point : ui->scatterplotView->data() ){
        if( displayed.contains(point.key) ){
            data.push_back( f(point.coords));
        }
    }

    return data;
}

void ScatterplotViewer::on_actionSelect_Items_triggered()
{
    auto avail = ui->scatterplotView->keys();
    auto displayed = ui->scatterplotView->displayedKeys();
    auto ok=false;
    auto items=MultiItemSelectionDialog::getItems( nullptr, "Select Displayed Items", "Select Items:", avail, &ok, displayed );
    if( items.isEmpty() || !ok ) return;
    ui->scatterplotView->setDisplayedKeys(items);
}
