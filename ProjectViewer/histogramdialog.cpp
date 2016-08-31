#include "histogramdialog.h"
#include "ui_histogramdialog.h"

#include<QGraphicsScene>
#include<QGraphicsRectItem>
#include<QGraphicsLineItem>
#include<alignedtextgraphicsitem.h>
#include<QDoubleValidator>
#include<QKeyEvent>
#include<limits>
#include<iostream>

#include<cmath>


HistogramDialog::HistogramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistogramDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leMin->setValidator(validator);
    ui->leMax->setValidator(validator);

    connect( ui->leMin, SIGNAL( returnPressed()), this, SLOT(updateHistogram()));
    connect( ui->leMax, SIGNAL( returnPressed()), this, SLOT(updateHistogram()));
    connect( ui->sbBinCount, SIGNAL(valueChanged(int)), this, SLOT(updateHistogram()));

    connect( ui->pbMinFromData, SIGNAL(clicked()), this, SLOT(setMinimumFromData()));
    connect( ui->pbMaxFromData, SIGNAL(clicked()), this, SLOT(setMaximumFromData()));
}

HistogramDialog::~HistogramDialog()
{
    delete ui;
}


void HistogramDialog::setHistogram(const Histogram& h){
    m_histogram=h;
    updateMaximumCount();
    updateScene();
}

void HistogramDialog::setData(QVector<double> data){
    m_data=data;
    updateDataMinMax();
    //updateHistogram();  // called byupdate min max
}

void HistogramDialog::setMinimumFromData(){
    ui->leMin->setText(QString::number(m_dataMin));
    updateHistogram();
}

void HistogramDialog::setMaximumFromData(){
    ui->leMax->setText(QString::number(m_dataMax));
    updateHistogram();
}


void HistogramDialog::updateHistogram(){
/*
    double min=std::numeric_limits<double>::max();
    double max=std::numeric_limits<double>::lowest();
    for( auto x : m_data){
        if(x<min)min=x;
        if(x>max)max=x;
    }
*/
    double min=ui->leMin->text().toDouble();
    double max=ui->leMax->text().toDouble();
    int binCount=ui->sbBinCount->value();
/*
    // estimate optimum bin width
    double w=(max-min)/binCount;  // cannot be zero be cause of ui def
    double q=std::pow(10, std::ceil(std::log10(w)));
    if(q/5>w) w=q/5;
    else if(q/2>w) w=q/2;
    else w=q;

    double delta=binCount*w - (max-min);    // center actual data range on plot
    min=w*std::floor(min/w) - w*std::floor(delta/w/2);
*/
    double w=(max-min)/binCount;
    // generate histogram
    Histogram hist(min, w, binCount);
    for( auto x : m_data){
        hist.addValue(x);
    }

    setHistogram(hist);
}

void HistogramDialog::updateDataMinMax(){
    double min=std::numeric_limits<double>::max();
    double max=std::numeric_limits<double>::lowest();
    for( auto x : m_data){
        if(x<min)min=x;
        if(x>max)max=x;
    }

    m_dataMin=min;
    m_dataMax=max;

    int binCount=ui->sbBinCount->value();
    // estimate optimum bin width
    double w=(max-min)/binCount;  // cannot be zero be cause of ui def
    double q=std::pow(10, std::ceil(std::log10(w)));
    if(q/5>w) w=q/5;
    else if(q/2>w) w=q/2;
    else w=q;

    double delta=binCount*w - (max-min);    // center actual data range on plot
    min=w*std::floor(min/w) - w*std::floor(delta/w/2);
    max=min + binCount*w;

    ui->leMin->setText(QString::number(min));
    ui->leMax->setText(QString::number(max));

    updateHistogram();
}

void HistogramDialog::updateMaximumCount(){

    size_t max=0;

    for( size_t i=0; i<m_histogram.binCount(); i++){
        size_t count=m_histogram.binValue(i);
        if( count>max) max=count;
    }

    m_maximumCount=max;

}

const qreal MARGIN_X=50;
const qreal MARGIN_Y=50;
const qreal BAR_WIDTH=25;
const qreal BAR_MAX_HEIGHT=250;


void HistogramDialog::updateScene(){

    if( m_histogram.binCount()<1) return;

    QGraphicsScene* scene=new QGraphicsScene(this);

    qreal totalWidth=MARGIN_X + m_histogram.binCount()*BAR_WIDTH + MARGIN_X;
    qreal totalHeight=MARGIN_Y + BAR_MAX_HEIGHT + MARGIN_Y;

    QGraphicsRectItem* frame=new QGraphicsRectItem(
            0, 0, m_histogram.binCount()*BAR_WIDTH, BAR_MAX_HEIGHT);
    frame->moveBy(MARGIN_X, MARGIN_Y);
    scene->addItem(frame);

    qreal yfactor=BAR_MAX_HEIGHT/ (1.2*m_maximumCount );

    // horizontal lines and vertical scale

    QPen lpen(Qt::black,0);
    lpen.setStyle(Qt::DotLine);

    size_t N_TICKS=10;
    double h=1.2*m_maximumCount/N_TICKS;
    double q=std::pow(10, std::ceil(std::log10(h)));
    if(q/5>h) h=q/5;
    else if(q/2>h) h=q/2;
    else h=q;

    for( size_t i=0; i<N_TICKS; i++){

        qreal tick=i*h;
        if( tick>1.2*m_maximumCount) break;
        double y=BAR_MAX_HEIGHT-yfactor*qreal(tick);
        QGraphicsLineItem* line=new QGraphicsLineItem(
                QLineF(0, 0, m_histogram.binCount()*BAR_WIDTH, 0 ), frame );
        line->setPen(lpen);
        line->moveBy(0, y );

        AlignedTextGraphicsItem* label=new AlignedTextGraphicsItem(QString::number(tick), frame);
        label->setHAlign(Qt::AlignRight);
        label->setVAlign(Qt::AlignVCenter);
        label->moveBy( -5, y);
    }


    for( size_t i=0; i<m_histogram.binCount(); i++){

        qreal barHeight= yfactor * qreal(m_histogram.binValue(i));
        qreal x= i * BAR_WIDTH;
        QGraphicsRectItem* bar=new QGraphicsRectItem( 0, 0, BAR_WIDTH, -barHeight, frame);
        bar->setBrush(Qt::red);
        bar->moveBy( x, BAR_MAX_HEIGHT);

        AlignedTextGraphicsItem* label=new AlignedTextGraphicsItem(
                QString::number(m_histogram.binInterval(i).first), frame );
        label->setRotation(90);
        label->setHAlign(Qt::AlignLeft);
        label->setVAlign(Qt::AlignBottom);
        label->moveBy( x, BAR_MAX_HEIGHT + 5 );
    }

    QRectF br=scene->itemsBoundingRect();
    br=br.marginsAdded(QMarginsF(20, 20, 20, 20));

    scene->setSceneRect( br);

    ui->graphicsView->setScene(scene);

    ui->graphicsView->setMinimumSize(scene->sceneRect().width(), scene->sceneRect().height());
}


void HistogramDialog::keyPressEvent(QKeyEvent *ev){

    if( ev->key() == Qt::Key_Return){
        ev->accept(); // ate it
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}

