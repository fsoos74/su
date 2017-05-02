#include "colorcompositeviewer.h"
#include "ui_colorcompositeviewer.h"

#include<QGraphicsPixmapItem>
#include<QMessageBox>
#include<QToolBar>
#include<cmath>
#include<dynamicmousemodeselector.h>

const QString NoneString("NONE");

ColorCompositeViewer::ColorCompositeViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::ColorCompositeViewer)
{
    ui->setupUi(this);

    setupMouseModes();

    // prevent overlapping of tick labels, need to make this automatic
    ui->graphicsView->topRuler()->setMinimumPixelIncrement(100);

    ui->graphicsView->setAspectRatioMode(Qt::KeepAspectRatio);

    ui->graphicsView->setMouseTracking(true);  // also send mouse move events when no button is pressed
    connect( ui->graphicsView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)) );

    connect(ui->hwRed, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(updateScene() ) );
    connect( ui->hwRed, SIGNAL(powerChanged(double)), this, SLOT(updateScene() ) );
    connect(ui->hwGreen, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(updateScene()  ) );
    connect( ui->hwGreen, SIGNAL(powerChanged(double)), this, SLOT(updateScene() ) );
    connect(ui->hwBlue, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(updateScene() ) );
    connect( ui->hwBlue, SIGNAL(powerChanged(double)), this, SLOT(updateScene() ) );

    setupColorTables();

    updateScene();
}

ColorCompositeViewer::~ColorCompositeViewer()
{
    delete ui;
}


void ColorCompositeViewer::setupMouseModes(){

    DynamicMouseModeSelector* mm=new DynamicMouseModeSelector(this);
    connect( mm, SIGNAL(modeChanged(MouseMode)), ui->graphicsView, SLOT(setMouseMode(MouseMode)));
    mm->addMode(MouseMode::Explore);
    mm->addMode(MouseMode::Zoom);
    mm->addMode(MouseMode::Select);

    QToolBar* mouseToolBar=addToolBar("mouse toolbar");
    mouseToolBar->addWidget( mm);
    mouseToolBar->show();
}

void ColorCompositeViewer::setupColorTables(){

    // build color arrays
    QVector<QRgb> reds, greens, blues;
    for(int i=0; i<=255; i++){
        reds.push_back(qRgb(i,0,0));
        greens.push_back(qRgb(0,i,0));
        blues.push_back(qRgb(0,0,i));
    }

    // create colortables and assign color arrays, that way mapping of valuea to colors can be used as usual
    ColorTable* ctRed=new ColorTable(this);
    ctRed->setColors( reds  );
    ui->hwRed->setColorTable(ctRed);

    ColorTable* ctGreen=new ColorTable(this);
    ctGreen->setColors( greens );
    ui->hwGreen->setColorTable(ctGreen);

    ColorTable* ctBlue=new ColorTable(this);
    ctBlue->setColors( blues );
    ui->hwBlue->setColorTable(ctBlue);
}


void ColorCompositeViewer::setProject(AVOProject* project ){
    if( project==m_project) return;

    m_project=project;

    // adjust graphicsview geometry
    auto bbox=m_project->geometry().bboxCoords();
    ui->graphicsView->setSceneRect(bbox);
    //ui->graphicsView->fitInView(bbox, ui->graphicsView->aspectRatioMode() );
    ui->graphicsView->zoomFitWindow();

    QTransform stf=ui->graphicsView->transform();
    // invert y-axis
    stf.scale( 1, -1);
    stf.translate( 0, -bbox.bottom() );

    ui->graphicsView->setTransform(stf);

    // reset grids because they are from other project
    m_red.reset();
    m_green.reset();
    m_blue.reset();
    updateInputGrids();
    updateScene();
}


void ColorCompositeViewer::receivePoint( SelectionPoint pt, int code ){

    return; // NOP
}

void ColorCompositeViewer::receivePoints( QVector<SelectionPoint> points, int code){

    return; // NOP
}

void ColorCompositeViewer::onMouseOver(QPointF scenePos){

   QString message=QString("x=%1,  y=%2").arg(scenePos.x()).arg(scenePos.y());

   QTransform ilxl_to_xy, xy_to_ilxl;
   if( m_project && m_project->geometry().computeTransforms( xy_to_ilxl, ilxl_to_xy ) ){
       auto ilxl=xy_to_ilxl.map(scenePos);
       int il=std::round(ilxl.x());
       int xl=std::round(ilxl.y());
       message+=QString(", iline*=%1, xline*=%2").arg(il).arg(xl);

       if(m_red){
           float r=m_red->valueAt(il,xl);
           message+=QString(", red=");
           message+=(r==m_red->NULL_VALUE)?QString("NULL"):QString::number(r);
       }
       if(m_green){
           float g=m_green->valueAt(il,xl);
           message+=QString(", green=");
           message+=(g==m_green->NULL_VALUE)?QString("NULL"):QString::number(g);
       }
       if(m_blue){
           float b=m_blue->valueAt(il,xl);
           message+=QString(", blue=");
           message+=(b==m_blue->NULL_VALUE)?QString("NULL"):QString::number(b);
       }
   }

   statusBar()->showMessage(message);
}

void ColorCompositeViewer::setRed(std::shared_ptr<Grid2D<float>> g){
    if( g==m_red) return;

    m_red=g;

    if( g ){
        ui->hwRed->setRange(valueRange( *g ) );
        ui->hwRed->setHistogram(
            createHistogram( std::begin(*g), std::end(*g), g->NULL_VALUE, 100 )
            );
    }
    else{
        ui->hwRed->setHistogram(Histogram());
    }

    updateScene();
}

void ColorCompositeViewer::setGreen(std::shared_ptr<Grid2D<float>> g){
    if( g==m_green ) return;

    m_green=g;

    if(g){
        ui->hwGreen->setRange(valueRange( *g ) );
        ui->hwGreen->setHistogram(
            createHistogram( std::begin(*g), std::end(*g), g->NULL_VALUE, 100 )
            );
    }
    else{
        ui->hwGreen->setHistogram(Histogram());
    }

    updateScene();
}

void ColorCompositeViewer::setBlue(std::shared_ptr<Grid2D<float>> g){
    if( g==m_blue ) return;

    m_blue=g;

    if(g){
        ui->hwBlue->setRange(valueRange( *g ) );
        ui->hwBlue->setHistogram(
            createHistogram( std::begin(*g), std::end(*g), g->NULL_VALUE, 100 )
            );
    }
    else{
        ui->hwBlue->setHistogram(Histogram());
    }

    updateScene();
}


std::pair<Grid2DBounds,  QImage>  ColorCompositeViewer::buildImage( Grid2D<float>* red, Grid2D<float>* green, Grid2D<float>* blue){

    if( !m_red && !m_green && !m_blue ) return std::make_pair(Grid2DBounds(), QImage() );

    Grid2DBounds bounds;
    if( m_red ) bounds=m_red->bounds();
    else if( m_green ) bounds=m_green->bounds();
    else if( m_blue ) bounds=m_blue->bounds();

    if( (m_red && m_red->bounds()!=bounds) || (m_green&&m_green->bounds()!=bounds) || (m_blue&&m_blue->bounds()!=bounds) ){
        QMessageBox::critical(this, tr("Color Composite"), tr("Grids have different bounds!"));
        return std::make_pair(Grid2DBounds(), QImage() );
    }

    QImage img(bounds.height(), bounds.width(), QImage::Format_RGB32);
    img.fill(Qt::black);

    for( auto i = bounds.i1(); i<=bounds.i2(); i++){
        for( auto j=bounds.j1(); j<=bounds.j2(); j++){

            int rc=0;
            if( red ){
                auto r = red->valueAt(i,j);
                if( r!=red->NULL_VALUE )
                {
                    rc = qRed( ui->hwRed->colorTable()->map(r) );
                }
            }

            int gc=0;
            if( green ){
                auto g = green->valueAt(i,j);
                if( g!=green->NULL_VALUE){
                    gc = qGreen( ui->hwGreen->colorTable()->map(g) );
                }
            }

            int bc=0;
            if( blue){
                auto b = blue->valueAt(i,j);
                if( b!=blue->NULL_VALUE){
                    bc = qBlue( ui->hwBlue->colorTable()->map(b) );
                }
            }

            img.setPixel( i - bounds.i1(), j - bounds.j1(), qRgb( rc, gc, bc ) );
        }
    }

    return std::make_pair(bounds, img );
}

void ColorCompositeViewer::updateScene(){

    if( !m_project ) return;

    auto bounds_and_image=buildImage(m_red.get(), m_green.get(), m_blue.get() );
    auto bounds=bounds_and_image.first;
    auto img=bounds_and_image.second;

    if( img.isNull() ){
        if(m_pixmapItem ) m_pixmapItem->setPixmap(QPixmap());
        return;
    }

    if( !m_pixmapItem ){

        QGraphicsScene* scene=new QGraphicsScene(this);

        QGraphicsPixmapItem* item=new QGraphicsPixmapItem(QPixmap::fromImage(img));

        item->setTransformationMode(Qt::SmoothTransformation);


        QTransform tf;
        tf.translate( bounds.i1(), bounds.j1());

        tf.scale(qreal(bounds.height()-1)/bounds.height(),
                qreal(bounds.width()-1)/bounds.width() );

        QTransform ilxl_to_xy, xy_to_ilxl;
        m_project->geometry().computeTransforms( xy_to_ilxl, ilxl_to_xy );
        tf=tf*ilxl_to_xy;
        item->setTransform(tf);

        scene->addItem(item);
        m_pixmapItem=item;

        ui->graphicsView->setScene(scene);
        ui->graphicsView->zoomFitWindow();
        //ui->graphicsView->fitInView(m_project->geometry().bboxCoords(), Qt::KeepAspectRatio);
        //ui->graphicsView->fitInView(item->boundingRect());

    }
    else{
        m_pixmapItem->setPixmap(QPixmap::fromImage(img));
    }
}


void ColorCompositeViewer::updateInputGrids(){

    ui->cbRed->clear();
    ui->cbRed->addItem(NoneString);
    ui->cbGreen->clear();
    ui->cbGreen->addItem(NoneString);
    ui->cbBlue->clear();
    ui->cbBlue->addItem(NoneString);

    if( m_project){
        auto grids=m_project->gridList(GridType::Other);
        ui->cbRed->addItems(grids);
        ui->cbGreen->addItems(grids);
        ui->cbBlue->addItems(grids);
    }
}

double sliderToPower(int value){

    if( value<50) return double(value+1)/50;
    else return 1. + 0.1 * (value-50);
}

void ColorCompositeViewer::on_slRed_valueChanged(int value)
{
    ui->hwRed->setPower( sliderToPower(value));
}

void ColorCompositeViewer::on_slGreen_valueChanged(int value)
{
    ui->hwGreen->setPower( sliderToPower(value));
}

void ColorCompositeViewer::on_slBlue_valueChanged(int value)
{
    ui->hwBlue->setPower( sliderToPower(value));
}

void ColorCompositeViewer::on_cbRed_currentIndexChanged(const QString &arg1)
{
    std::shared_ptr<Grid2D<float>> g;
    if( arg1!=NoneString && m_project){  // g==nullptr otherwise
        g=m_project->loadGrid(GridType::Other, arg1 );
        if( !g ){
            QMessageBox::critical(this, tr("Load Grid"), QString("Loading grid \"%1\" failed!").arg(arg1) );
        }
    }
    setRed(g);
}

void ColorCompositeViewer::on_cbGreen_currentIndexChanged(const QString &arg1)
{
    std::shared_ptr<Grid2D<float>> g;
    if( arg1!=NoneString && m_project){  // g==nullptr otherwise
        g=m_project->loadGrid(GridType::Other, arg1 );
    }
    setGreen(g);
}

void ColorCompositeViewer::on_cbBlue_currentIndexChanged(const QString &arg1)
{
    std::shared_ptr<Grid2D<float>> g;
    if( arg1!=NoneString && m_project){  // g==nullptr otherwise
        g=m_project->loadGrid(GridType::Other, arg1 );
    }
    setBlue(g);
}
