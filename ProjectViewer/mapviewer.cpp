#include "mapviewer.h"
#include "ui_mapviewer.h"


#include<QGraphicsTextItem>
#include<QPrinter>
#include<QPainter>
#include<QGraphicsScene>
#include<QInputDialog>
#include<QFileDialog>
#include<QMessageBox>
#include<cmath>
#include<limits>
#include<iostream>

#include<alignedtextgraphicsitem.h>

#include<colortable.h>


MapViewer::MapViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MapViewer)
{
    ui->setupUi(this);
}

MapViewer::~MapViewer()
{
    delete ui;
}


void MapViewer::setProject(std::shared_ptr<AVOProject> p){

    if( p==m_project) return;

    m_project=p;

    refreshMap();
}



void MapViewer::setGrid(std::shared_ptr<Grid2D<float> > g ){

    if( g==m_grid ) return;

    m_grid=g;

    refreshMap();
}


void MapViewer::refreshMap(){

    QGraphicsScene* scene=new QGraphicsScene;

    if( !m_project){
        ui->graphicsView->setScene(scene);
        return;
    }

    ProjectGeometry geom=m_project->geometry();

    // From here ALL drawing to scene in iline/xline coords

    // padding
    const int padIl=10;
    const int padXl=10;

    QTransform XYToILXL;
    QTransform ILXLToXY;
    geom.computeTransforms(XYToILXL, ILXLToXY);

    //compute inline/xline rotation vs x-axis for labels
    QPointF pt0=ILXLToXY.map(QPoint(0,0));
    QPointF pt1=ILXLToXY.map(QPoint(1,0));
    QPointF pt=pt1-pt0;
    qreal rotxl=180.0 * std::atan2(pt.y(), pt.x())/M_PI;
    qreal rotil=rotxl-90;


    // find project bounds
    int minIl=std::numeric_limits<int>::max();
    int maxIl=std::numeric_limits<int>::min();
    int minXl=minIl;
    int maxXl=maxIl;
    for( int i=0; i<3; i++){

        int il=geom.inlineAndCrossline(i).x();
        if( il<minIl ) minIl=il;
        if( il>maxIl ) maxIl=il;

        int xl=geom.inlineAndCrossline(i).y();
        if( xl<minXl ) minXl=xl;
        if( xl>maxXl ) maxXl=xl;
    }

    // draw project bounding rect
    QRectF brect( minIl, minXl, maxIl-minIl, maxXl-minXl);
    scene->addRect(brect, QPen(Qt::black,0), QBrush(Qt::lightGray));

    // draw grid
    if( m_grid ){

        Grid2DBounds bounds=m_grid->bounds();

        ColorTable ct;
        ct.setColors(ColorTable::defaultColors());
        std::pair<float, float> vr=valueRange(*m_grid);
        ct.setRange(vr);

        QImage img(bounds.height(), bounds.width(), QImage::Format_ARGB32 );
        for( int i=0; i<bounds.height(); i++){

            for( int j=0; j<bounds.width(); j++){

                float val=(*m_grid)(bounds.i1() + i, bounds.j1() + j );

                QRgb col;

                if( val!=m_grid->NULL_VALUE){
                    col=ct.map(val);
                }
                else{
                    col=Qt::lightGray;      // better make background color attribute
                }

                img.setPixel( i, j, col );
            }
        }

        QGraphicsPixmapItem* pitem=new QGraphicsPixmapItem( QPixmap::fromImage( img ) );

      //  pitem->setRotation(rotil);

        pitem->setPos( bounds.i1(), bounds.j1() );

        scene->addItem(pitem);
    }

    QFont lineFont("Helvetica [Cronyx]", 12);
    lineFont.setPixelSize(12);

    // draw inline/xline labels
    AlignedTextGraphicsItem* tiMinIline=new AlignedTextGraphicsItem(QString("iline %1").arg(minIl));
    tiMinIline->setFont(lineFont);
    tiMinIline->setVAlign(Qt::AlignBottom);
    tiMinIline->setRotation(rotil);
    tiMinIline->setPos( minIl, (minXl+maxXl)/2);
    tiMinIline->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    scene->addItem(tiMinIline);

    AlignedTextGraphicsItem* tiMaxIline=new AlignedTextGraphicsItem(QString("iline %1").arg(maxIl));
    tiMaxIline->setVAlign(Qt::AlignTop);
    tiMaxIline->setRotation(rotil);
    tiMaxIline->setPos( maxIl, (minXl+maxXl)/2);
    tiMaxIline->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    scene->addItem(tiMaxIline);

    AlignedTextGraphicsItem* tiMinXline=new AlignedTextGraphicsItem(QString("xline %1").arg(minXl));
    tiMinXline->setVAlign(Qt::AlignTop);
    tiMinXline->setRotation(rotxl);
    tiMinXline->setPos( (minIl+maxIl)/2, minXl);
    tiMinXline->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    scene->addItem(tiMinXline);

    AlignedTextGraphicsItem* tiMaxXline=new AlignedTextGraphicsItem(QString("xline %1").arg(maxXl));
    tiMaxXline->setVAlign(Qt::AlignBottom);
    tiMaxXline->setRotation(rotxl);
    tiMaxXline->setPos( (minIl+maxIl)/2, maxXl);
    tiMaxXline->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    scene->addItem(tiMaxXline);


    // graphicsview is in xy coords, scene il/xl
    ui->graphicsView->setTransform(ILXLToXY);
    scene->setSceneRect(minIl-padIl, minXl-padXl, maxIl-minIl+2*padIl, maxXl-minXl+2*padXl);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MapViewer::on_actionOpen_Grid_triggered()
{
    if( !m_project ) return;

    QString name=QInputDialog::getItem(this, tr("Select Attribute"), tr("Grid:"), m_project->gridList(GridType::Attribute));
    if( name.isNull() ) return;

    std::shared_ptr<Grid2D<float>> g=m_project->loadGrid(GridType::Attribute, name);

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
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QPainter painter(&printer);
    ui->graphicsView->scene()->render(&painter);
    painter.end();
}

