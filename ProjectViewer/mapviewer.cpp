#define _USE_MATH_DEFINES

#include "mapviewer.h"
#include "ui_mapviewer.h"


#include<QGraphicsTextItem>
#include<QPrinter>
#include<QPainter>
#include<QGraphicsScene>
#include<QInputDialog>
#include<QFileDialog>
#include<QMessageBox>
#include<QFontDialog>
#include<cmath>
#include<limits>
#include<iostream>

#include<linelabelgraphicsitem.h>

#include<colortable.h>
#include<griduiutil.h>
#include<gridselectdialog.h>


// scale those to a4
const qreal PLOT_WIDTH=800;
const qreal PLOT_HEIGHT=800;
const qreal MAP_WIDTH=500;
const qreal MAP_HEIGHT=500;
const qreal COLORBAR_WIDTH=30;
const qreal COLORBAR_HEIGHT=MAP_HEIGHT;

const qreal MAP_PADDING=0.1;

MapViewer::MapViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MapViewer),
    m_tickFont("Helvetica [Cronyx]", 10 ),
    m_lineFont( "Helvetica [Cronyx]", 9 ),
    m_colorTable(new ColorTable(this))
{
    ui->setupUi(this);

    layoutPlot();

    m_colorTable->setColors(ColorTable::defaultColors());

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(refreshMap()) );
    connect(m_colorTable, SIGNAL(powerChanged(double)), this, SLOT(refreshMap()) );
    connect(m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(refreshMap()) );
    connect( ui->actionDisplay_Colorbar, SIGNAL(toggled(bool)), this, SLOT(refreshMap()) );
}

MapViewer::~MapViewer()
{
    delete ui;
}


void MapViewer::setProject(AVOProject* p){

    if( p==m_project) return;

    m_project=p;

    computeTransforms();

    refreshMap();

    ui->action_Fit_Window->trigger();
}



void MapViewer::setGrid(std::shared_ptr<Grid2D<float> > g ){

    if( g==m_grid ) return;

    m_grid=g;

    if( m_grid ){

        if( !isColorMappingLocked()){
            std::pair<float,float>  minMax=valueRange(*m_grid);
            m_colorTable->setRange(minMax.first, minMax.second);
        }

    }
    else{

        if( displayRangeDialog ){
            delete displayRangeDialog;
            displayRangeDialog=nullptr;
        }
    }

    bool useGrid=(m_grid)?true:false;

    ui->actionClose_Grid->setEnabled(useGrid);
    ui->actionEdit_Colortable->setEnabled(useGrid);
    ui->actionGrid_Display_Range->setEnabled(useGrid);
    ui->actionDisplay_Colorbar->setEnabled(useGrid);

    refreshMap();
}

void MapViewer::setColorMappingLocked(bool locked){

    if( locked==m_colorMappingLocked) return;

    m_colorMappingLocked=locked;
}

void MapViewer::setTickFont(const QFont& f){

    m_tickFont=f;

    refreshMap();
}

void MapViewer::setTickPen(const QPen& p){


    m_tickPen=p;

    refreshMap();
}

void MapViewer::setLineFont(const QFont& f){

    m_lineFont=f;

    refreshMap();
}


std::ostream& operator<<(std::ostream& os, const QPoint& p){
    os<<" ( "<<p.x()<<" , "<<p.y()<<" ) ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const QPointF& p){
    os<<" ( "<<p.x()<<" , "<<p.y()<<" ) ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const QRectF rect){

    os<<"QRectF( x="<<rect.x()<<", y="<<rect.y()<<", w="<<rect.width()<<", h="<<rect.height()<<" )";

    return os;
}

void MapViewer::layoutPlot(){

    qreal margin=(PLOT_WIDTH - MAP_WIDTH - COLORBAR_WIDTH ) / 3;

    m_bbPlot=QRectF(0,0,PLOT_WIDTH, PLOT_HEIGHT );
    m_bbMap=QRectF( margin, margin, MAP_WIDTH, MAP_HEIGHT );
    m_bbColorbar=QRectF(m_bbPlot.width() - margin, margin, COLORBAR_WIDTH, COLORBAR_HEIGHT );

    std::cout<<"bbplot "<<m_bbPlot<<std::endl;
    std::cout<<"bbmap  "<<m_bbMap<<std::endl;
    std::cout<<"bbcb   "<<m_bbColorbar<<std::endl;

}

void MapViewer::computeTransforms(){

    if( !m_project) return;

    ProjectGeometry geom=m_project->geometry();

    m_bbProject=geom.bboxLines();

    geom.computeTransforms(m_xy_to_ilxl, m_ilxl_to_xy);

    QTransform map_to_plot;
    map_to_plot.translate(m_bbMap.left(), m_bbMap.top());

    QRectF bb=m_ilxl_to_xy.mapRect(m_bbProject);

    qreal padx=MAP_PADDING*bb.width();
    qreal pady=MAP_PADDING*bb.height();
    m_bbCoords=bb.adjusted(-padx, -pady, padx, pady);

    qreal sx= m_bbMap.width() / m_bbCoords.width();
    qreal sy= m_bbMap.height() / m_bbCoords.height();

    // scale both axes identical, avoid distortion
    qreal s=( sx > sy ) ? sx : sy;

    // y axxis origin at bottom!!!!
    QTransform xy_to_map;
    xy_to_map.scale( s, -s );
    xy_to_map.translate( -m_bbCoords.x(), -m_bbCoords.bottom() );//m_bbCoords.y() );

    m_xy_to_plot=xy_to_map * map_to_plot;
    m_ilxl_to_plot= m_ilxl_to_xy * m_xy_to_plot ;
}


void MapViewer::drawGeometry( QGraphicsScene* scene ){

    // cannot simply transform rectf because depending on geom result may not be a redct anymore

    // inlines <=>x, xlines<=>y
    QPointF P1=m_ilxl_to_plot.map(m_bbProject.topLeft());      // min inline, min xline
    QPointF P2=m_ilxl_to_plot.map(m_bbProject.bottomLeft());   // min inline, max xline
    QPointF P3=m_ilxl_to_plot.map(m_bbProject.topRight());     // max inline, min xline
    QPointF P4=m_ilxl_to_plot.map(m_bbProject.bottomRight());  // max inline, max xline

    // point order is important to get labels aligned correctly
    QLineF liIlMin=QLineF( P2, P1);
    QLineF liXlMax=QLineF( P4, P2);
    QLineF liIlMax=QLineF( P3, P4);
    QLineF liXlMin=QLineF( P1, P3);

    QPolygonF poly;
    poly<<P1<<P2<<P4<<P3;
    scene->addPolygon( poly , QPen(Qt::blue, 0), QBrush(Qt::lightGray) );



    LineLabelGraphicsItem* lbIlMin=new LineLabelGraphicsItem( liIlMin, QString("iline %1").arg(m_bbProject.left() ), m_lineFont );
    lbIlMin->setPadY(2);
    lbIlMin->setVAlign(Qt::AlignTop);
    scene->addItem(lbIlMin);

    LineLabelGraphicsItem* lbIlMax=new LineLabelGraphicsItem( liIlMax, QString("iline %1").arg(m_bbProject.right()), m_lineFont);
    lbIlMax->setPadY(2);
    lbIlMax->setVAlign(Qt::AlignTop);
    scene->addItem(lbIlMax);

    LineLabelGraphicsItem* lbXlMin=new LineLabelGraphicsItem( liXlMin, QString("xline %1").arg(m_bbProject.top()), m_lineFont);
    lbXlMin->setPadY(2);
    lbXlMin->setVAlign(Qt::AlignTop);
    scene->addItem(lbXlMin);

    LineLabelGraphicsItem* lbXlMax=new LineLabelGraphicsItem( liXlMax, QString("xline %1").arg(m_bbProject.bottom()), m_lineFont);
    lbXlMax->setPadY(2);
    lbXlMax->setVAlign(Qt::AlignTop);
    scene->addItem(lbXlMax);

}


void MapViewer::drawCoords( QGraphicsScene* scene ){


    // compute tick increments
    const int MIN_TICKS_XY=5;
    qreal tickDX=std::pow(10, std::trunc( std::log10( m_bbCoords.width()/MIN_TICKS_XY) ) );
    qreal tickDY=std::pow(10, std::trunc( std::log10( m_bbCoords.height()/MIN_TICKS_XY) ) );

    // adjust rect to tick increment, make boundaries fall on ticks
    QRectF bb=m_bbCoords;
    bb.setLeft( tickDX * std::floor( m_bbCoords.left()/tickDX));
    bb.setRight( tickDX * std::ceil( m_bbCoords.right()/tickDX));
    bb.setTop( tickDY * std::floor( m_bbCoords.top()/tickDY));
    bb.setBottom( tickDY * std::ceil( m_bbCoords.bottom()/tickDY));

    QPen tickLinePen(Qt::darkGray, 0);

    // draw horizontal ticks
    int nx = std::trunc( bb.width()/tickDX + 1);
    for( int i=0; i<nx; i++){

        qreal x= bb.left() + i*tickDX;

        QLineF line=m_xy_to_plot.map( QLineF(x, bb.top(), x, bb.bottom() ) );

        scene->addLine( line, tickLinePen );

        LineLabelGraphicsItem* lbl1=new LineLabelGraphicsItem( line, QString::number(x, 'g', 7), m_tickFont );
        lbl1->setHAlign(Qt::AlignLeft);
        lbl1->setVAlign(Qt::AlignVCenter);
        lbl1->setPadX(10);
        scene->addItem(lbl1);

        LineLabelGraphicsItem* lbl2=new LineLabelGraphicsItem( line, QString::number(x, 'g', 7), m_tickFont );
        lbl2->setHAlign(Qt::AlignRight);
        lbl2->setVAlign(Qt::AlignVCenter);
        lbl2->setPadX(10);
        scene->addItem(lbl2);
    }

    // draw vertical ticks
    int ny= std::trunc( bb.height()/tickDY + 1 );
    for(int i=0; i<ny; i++){

        qreal y=bb.top() + i*tickDY;

        QLineF line=m_xy_to_plot.map( QLineF( bb.left(), y, bb.right(), y ) );

        scene->addLine( line, tickLinePen );

        LineLabelGraphicsItem* lbl1=new LineLabelGraphicsItem( line, QString::number(y, 'g', 7), m_tickFont );
        lbl1->setHAlign(Qt::AlignLeft);
        lbl1->setVAlign(Qt::AlignVCenter);
        lbl1->setPadX(10);
        scene->addItem(lbl1);

        LineLabelGraphicsItem* lbl2=new LineLabelGraphicsItem( line, QString::number(y, 'g', 7), m_tickFont );
        lbl2->setHAlign(Qt::AlignRight);
        lbl2->setVAlign(Qt::AlignVCenter);
        lbl2->setPadX(10);
        scene->addItem(lbl2);
    }


    scene->addRect( m_xy_to_plot.mapRect(bb), QPen(Qt::black, 0.5) );
}


void MapViewer::drawGrid( QGraphicsScene* scene ){

    QImage img=grid2image(m_grid.get(), m_colorTable);

    QGraphicsPixmapItem* item=new QGraphicsPixmapItem(QPixmap::fromImage(img));

    item->setTransformationMode(Qt::SmoothTransformation);

    QTransform tf;
    tf.translate(m_grid->bounds().i1(), m_grid->bounds().j1());

    // testing
    tf.scale(qreal(m_grid->bounds().height()-1)/m_grid->bounds().height(),
             qreal(m_grid->bounds().width()-1)/m_grid->bounds().width() );

    tf=tf*m_ilxl_to_plot;
    item->setTransform(tf);

    scene->addItem(item);
}


void MapViewer::drawColorbar(QGraphicsScene* scene){

    qreal sx= m_bbColorbar.width();
    qreal sy= m_bbColorbar.height()/ m_colorTable->colors().size();


    QTransform color_index_to_cb;
    color_index_to_cb.scale(sx, -sy);

    QTransform cb_to_plot;
    //cb_to_plot.scale( sx, sy );
    cb_to_plot.translate( m_bbColorbar.x(), m_bbColorbar.bottom() );
    cb_to_plot=color_index_to_cb * cb_to_plot;

    QImage img( 1, m_colorTable->colors().size(), QImage::Format_RGB32);

    for( int i=0; i<img.height(); i++){
        double v=m_colorTable->range().first + i*(m_colorTable->range().second - m_colorTable->range().first)/img.height();

        //img.setPixel(0, img.height() - i -1, m_colorTable->map(v) );   // lowest at bottom, wrong vertically mirrored
        img.setPixel(0, i, m_colorTable->map(v) );   // lowest at bottom, image origin at bottom of image
    }

    QGraphicsPixmapItem* item=new QGraphicsPixmapItem(QPixmap::fromImage(img) );
    item->setTransform(cb_to_plot);
    scene->addItem(item);

    QRectF r(0, 0, 1, m_colorTable->colors().size());
    scene->addRect(cb_to_plot.mapRect(r), QPen(Qt::black,1) );

    QPen tickPen(Qt::black, 1);
    QFont tickFont( "Helvetica [Cronyx]", 10 );//, QFont::Bold);


    int steps=10;
    for( int i=0; i<=steps; i++ ){

        qreal value=m_colorTable->range().first + i*(m_colorTable->range().second-m_colorTable->range().first)/steps;
        qreal color=i*qreal(m_colorTable->colors().size())/steps;
        QPointF p=cb_to_plot.map( QPointF(1,color) );

        QLineF  tick( p.x()-5, p.y(), p.x() + 5, p.y() );
        scene->addLine( tick, tickPen);

        LineLabelGraphicsItem* label=new LineLabelGraphicsItem(tick, QString::number(value, 'g', 3), tickFont);
        label->setAlignments(Qt::AlignRight, Qt::AlignVCenter);
        label->setPadX(3);
        scene->addItem(label);

        std::cout<<"p "<<p<<std::endl;

    }

    //scene->addRect(m_bbPlot, QPen(Qt::black,3));

}

void MapViewer::refreshMap(){

    // project geometry describes transforms between iline,xline and map coordinates
    // scene is in map coordinates
    // plot is in pixel

    QGraphicsScene* scene=new QGraphicsScene();

    drawCoords( scene );

    drawGeometry( scene );

    if( m_grid ){

        drawGrid( scene );

        if( ui->actionDisplay_Colorbar->isChecked()){
            drawColorbar(scene);
        }
    }


    ui->graphicsView->setScene(scene);

}



void MapViewer::on_actionOpen_Grid_triggered()
{
    if( !m_project ) return;

    GridSelectDialog dlg;
    dlg.setWindowTitle(tr("Select Grid"));
    dlg.setProject(*m_project);

    if( dlg.exec()!=QDialog::Accepted) return;

    GridType type=dlg.type();
    QString name=dlg.name();//QInputDialog::getItem(this, tr("Select Attribute"), tr("Grid:"), m_project->gridList(GridType::Attribute));
    //if( name.isNull() ) return;

    std::shared_ptr<Grid2D<float>> g=m_project->loadGrid(type, name);

    if( !g ){

        QMessageBox::critical(this, tr("Load Grid"), QString("Loading grid \"%1\" failed!"). arg(name));
        return;
    }

    setGrid(g);
}

void MapViewer::on_action_Print_triggered()
{

    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                                                QString(), "*.pdf");
    if( fileName.isNull()) return;

    QPrinter printer( QPrinter::HighResolution );
    printer.setPageSize( QPrinter::A4 );
    printer.setOrientation( QPrinter::Portrait );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setOutputFileName( fileName );
    QPainter p;

    if( !p.begin( &printer ) )
    {
        QMessageBox::critical(this, tr("Export Map"), tr("Error printing"));
        return;
    }

    ui->graphicsView->scene()->render( &p );

    p.end();
}


void MapViewer::on_actionZoom_In_triggered()
{
    ui->graphicsView->scale(1.2, 1.2);
}

void MapViewer::on_actionZoom_Out_triggered()
{
    ui->graphicsView->scale(0.8, 0.8);
}

void MapViewer::on_action_Fit_Window_triggered()
{
    ui->graphicsView->fitInView(ui->graphicsView->sceneRect(), Qt::KeepAspectRatio);
}


void MapViewer::on_actionEdit_Colortable_triggered()
{

    QVector<QRgb> oldColors=m_colorTable->colors();

    ColorTableDialog colorTableDialog( m_colorTable->colors() );

    connect( &colorTableDialog, SIGNAL(colorsChanged(QVector<QRgb>)), m_colorTable, SLOT(setColors(QVector<QRgb>)));

    if( colorTableDialog.exec()==QDialog::Accepted ){
        m_colorTable->setColors( colorTableDialog.colors());
    }else{
        m_colorTable->setColors( oldColors );
    }

}

void MapViewer::on_actionGrid_Display_Range_triggered()
{
    if( !displayRangeDialog){
        displayRangeDialog=new DisplayRangeDialog(this);

        displayRangeDialog->setPower(m_colorTable->power());
        displayRangeDialog->setRange(m_colorTable->range());
        displayRangeDialog->setLocked(isColorMappingLocked() );
        connect( displayRangeDialog, SIGNAL(rangeChanged(std::pair<double,double>)),
                 m_colorTable, SLOT(setRange(std::pair<double,double>)) );
        connect( displayRangeDialog, SIGNAL(powerChanged(double)),
                 m_colorTable, SLOT( setPower(double)) );
        connect( displayRangeDialog, SIGNAL(lockedChanged(bool)),
                 this, SLOT(setColorMappingLocked(bool)) );
    }
    displayRangeDialog->show();
}

void MapViewer::on_actionClose_Grid_triggered()
{
    std::shared_ptr<Grid2D<float> > null;
    setGrid(null);
}


void MapViewer::on_actionCoordinate_Font_triggered()
{

    bool ok=false;

    QFont font=QFontDialog::getFont(&ok, tickFont(),this,tr("Select Coordinate Label Font"));

    if( ok ){
        setTickFont(font);
    }

}

void MapViewer::on_actionLines_Font_triggered()
{

    bool ok=false;

    QFont font=QFontDialog::getFont(&ok, lineFont(),this,tr("Select Inline/Crossline Label Font"));

    if( ok ){
        setLineFont(font);
    }
}
