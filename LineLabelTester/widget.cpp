#include "widget.h"
#include "ui_widget.h"


#include<QGraphicsScene>
#include<linelabelgraphicsitem.h>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    fillScene();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::fillScene(){

    const qreal WIDTH=297;
    const qreal HEIGHT=210;
    const qreal PAD=10;

    QGraphicsScene* scene=new QGraphicsScene();
/*
    QLineF l1( 50, 50, 200, 50 );
    LineLabelGraphicsItem* item1=new LineLabelGraphicsItem(l1, "abcdABCD1234");
    item1->setVAlign(Qt::AlignTop);
    item1->setPadY(PAD);
    scene->addItem(item1);
    scene->addLine(l1, QPen(Qt::red,0));

    QLineF l2( 50, 100, 200, 100 );
    LineLabelGraphicsItem* item2=new LineLabelGraphicsItem(l2, "abcdABCD1234");
    item2->setVAlign(Qt::AlignVCenter);
     item2->setPadY(PAD);
    scene->addItem(item2);
    scene->addLine(l2, QPen(Qt::red,0));

    QLineF l3( 50, 150, 200, 150 );
    LineLabelGraphicsItem* item3=new LineLabelGraphicsItem(l3, "abcdABCD1234");
    item3->setVAlign(Qt::AlignBottom);
     item3->setPadY(PAD);
    scene->addItem(item3);
    scene->addLine(l3, QPen(Qt::red,0));


    LineLabelGraphicsItem* item11=new LineLabelGraphicsItem(l1, "abcdABCD1234");
    item11->setHAlign(Qt::AlignLeft);
    item11->setVAlign(Qt::AlignVCenter);
    item11->setPadX(PAD);
    scene->addItem(item11);

    LineLabelGraphicsItem* item12=new LineLabelGraphicsItem(l2, "abcdABCD1234");
    item12->setVAlign(Qt::AlignVCenter);
    item12->setHAlign(Qt::AlignHCenter);
    item12->setPadX(PAD);
    scene->addItem(item12);

    LineLabelGraphicsItem* item13=new LineLabelGraphicsItem(l3, "abcdABCD1234");
    item13->setVAlign(Qt::AlignVCenter);
    item13->setHAlign(Qt::AlignRight);
    item13->setPadY(PAD);
    item13->setPadX(PAD);
    scene->addItem(item13);
*/

    QRectF bbPlot(0,0,WIDTH,HEIGHT);
    scene->addRect(bbPlot,QPen(Qt::black,4));

    QRectF bbMap(50, 50, 100, 100);
    scene->addRect(bbMap,QPen(Qt::red,3));

    QRectF bbXY(200000,500000, 10000, 10000);

    QTransform map_to_plot;
    map_to_plot.translate(bbMap.left(), bbMap.top());
    scene->addLine( map_to_plot.map(QLineF(0,0,bbMap.width(),bbMap.height())), QPen(Qt::blue,2));

    QTransform xy_to_map;
    qreal sx=bbMap.width()/bbXY.width();
    qreal sy=bbMap.height()/bbXY.height();
    qreal s=(sx>sy)?sx:sy; // avoid distortion

    xy_to_map.scale( s, -s);
    xy_to_map.translate( -bbXY.left(), -bbXY.bottom());
    QTransform xy_to_plot=xy_to_map * map_to_plot;

    scene->addRect(xy_to_plot.mapRect(bbXY), QPen(Qt::blue, 2));
    scene->addLine(xy_to_plot.map(QLineF(bbXY.left(),bbXY.top(),bbXY.right(),bbXY.top())), QPen(Qt::yellow,4, Qt::DashLine));

    scene->setSceneRect(0,0,WIDTH,HEIGHT);
    ui->graphicsView->setScene(scene);
}
