#include "histogramdialog.h"
#include "ui_histogramdialog.h"

#include<QGraphicsScene>
#include<QGraphicsRectItem>
#include<QGraphicsLineItem>
#include<QMessageBox>
#include<alignedtextgraphicsitem.h>
#include<QDoubleValidator>
#include<QKeyEvent>
#include<limits>
#include<iostream>
#include<algorithm>
#include<cmath>

const int MAX_BIN_COUNT=1000;

HistogramDialog::HistogramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistogramDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->lePlotMinimum->setValidator(validator);
    ui->lePlotMaximum->setValidator(validator);
    ui->lePlotBinWidth->setValidator(validator);

    connect( ui->lePlotMinimum, SIGNAL( returnPressed()), this, SLOT(updatePlotDataFromControls() ));
    connect( ui->lePlotMaximum, SIGNAL( returnPressed()), this, SLOT(updatePlotDataFromControls() ));
    connect( ui->lePlotBinWidth, SIGNAL(returnPressed()), this, SLOT(updatePlotDataFromControls() ));

    connect( ui->pbMinFromData, SIGNAL(clicked()), this, SLOT(setMinimumFromData()));
    connect( ui->pbMaxFromData, SIGNAL(clicked()), this, SLOT(setMaximumFromData()));

    connect( ui->pbRefresh, SIGNAL(clicked()), this, SLOT(updatePlotDataFromControls()) );
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

void HistogramDialog::setColorTable(ColorTable * ct){

    if( ct==m_colorTable ) return;

    if( m_colorTable ) disconnect(m_colorTable, 0, 0, 0);

    m_colorTable=ct;

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(updateScene()) );
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(updateScene()) );
    connect( m_colorTable, SIGNAL(powerChanged(double)), this, SLOT(updateScene()) );


    updateScene();
}

void HistogramDialog::setData(QVector<double> data){
    m_data=data;

    computeStatistics();

    if( std::isinf(m_dataMin) || std::isnan(m_dataMin) ||
            std::isinf(m_dataMax) || std::isnan(m_dataMax)){
        QMessageBox::critical(this, tr("Histogram"), tr("Invalid data"));
        return;
    }

    updateStatisticsControls();

    m_plotMinX=m_dataMin;
    m_plotMaxX=m_dataMax;
    m_plotBinWidth=(m_dataMax>m_dataMin) ? (m_dataMax-m_dataMin)/20 : 1.;   // important: handle case min==max

    updatePlotControlsFromData();

    updateHistogram();
}

void HistogramDialog::setMinimumFromData(){
    ui->lePlotMinimum->setText(QString::number(m_dataMin));
    updateHistogram();
}

void HistogramDialog::setMaximumFromData(){
    ui->lePlotMaximum->setText(QString::number(m_dataMax));
    updateHistogram();
}


void HistogramDialog::updateHistogram(){

    long binCount=std::ceil((m_plotMaxX - m_plotMinX)/m_plotBinWidth)+1;
    if( binCount>MAX_BIN_COUNT ){

        QMessageBox::warning(this, tr("Plot Histogram"), QString("Maximum number of bins(%1) exceeded").arg(QString::number(MAX_BIN_COUNT)));
        return;
    }

    double min=m_plotBinWidth*std::floor(m_plotMinX/m_plotBinWidth); // adjust to multiple of binwidth

    // generate histogram
    Histogram hist(min, m_plotBinWidth, binCount);
    for( auto x : m_data){
        hist.addValue(x);
    }

    setHistogram(hist);
}

void HistogramDialog::computeStatistics(){

    double min=std::numeric_limits<double>::max();
    double max=std::numeric_limits<double>::lowest();
    double sum=0;
    double sum2=0;

    for( auto x : m_data){
        if(x<min)min=x;
        if(x>max)max=x;
        sum+=x;
        sum2+=x*x;
    }

    double mean=0;
    double rms=0;

    if( m_data.size()>0 ){

        mean=sum/m_data.size();
        rms=std::sqrt(sum2/m_data.size());
    }

    std::nth_element(m_data.begin(), m_data.begin()+m_data.size()/2, m_data.end()); // this modifies the order of vector
    double median=m_data[m_data.size()/2];

    sum2=0;
    for( auto x : m_data){
        sum2+=(x-mean)*(x-mean);
    }
    double sigma=0;
    if( m_data.size()>1){
        sigma=std::sqrt(sum2/(m_data.size()-1));
    }

    m_dataCount=m_data.size();
    m_dataMin=min;
    m_dataMax=max;
    m_dataMean=mean;
    m_dataRMS=rms;
    m_dataMedian=median;
    m_dataSigma=sigma;
}


void HistogramDialog::updateStatisticsControls(){

    ui->leStatCount->setText(QString::number(m_dataCount));
    ui->leStatMinimum->setText(QString::number(m_dataMin));
    ui->leStatMaximum->setText(QString::number(m_dataMax));
    ui->leStatMean->setText(QString::number(m_dataMean));
    ui->leStatRMS->setText(QString::number(m_dataRMS));
    ui->leStatMedian->setText(QString::number(m_dataMedian));
    ui->leStatSigma->setText(QString::number(m_dataSigma));
}

void HistogramDialog::updatePlotControlsFromData(){
    ui->lePlotMinimum->setText(QString::number(m_plotMinX));
    ui->lePlotMaximum->setText(QString::number(m_plotMaxX));
    ui->lePlotBinWidth->setText(QString::number(m_plotBinWidth));
}

void HistogramDialog::updatePlotDataFromControls(){

    bool ok=true;

    double min=ui->lePlotMinimum->text().toDouble();
    double max=ui->lePlotMaximum->text().toDouble();

    QPalette minMaxPalette;
    if( min>max){
        ok=false;
        minMaxPalette.setColor(QPalette::Text, Qt::red);
    }
    ui->lePlotMinimum->setPalette(minMaxPalette);
    ui->lePlotMaximum->setPalette(minMaxPalette);

    double binWidth=ui->lePlotBinWidth->text().toDouble();
    QPalette widthPalette;
    if(binWidth<=0){
        ok=false;
        widthPalette.setColor(QPalette::Text, Qt::red);
    }
    ui->lePlotBinWidth->setPalette(widthPalette);

    if( !ok ) return;

    m_plotMinX=min;
    m_plotMaxX=max;
    m_plotBinWidth=binWidth;

    updateHistogram();
}

/*
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
*/

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
const int MAX_GRAPHICSVIEW_WIDTH=1000;
const int MAX_GRAPHICSVIEW_HEIGHT=1000;

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
        //label->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    }


    // drawbars
    for( size_t i=0; i<m_histogram.binCount(); i++){

        qreal barHeight= yfactor * qreal(m_histogram.binValue(i));
        qreal x= i * BAR_WIDTH;
        QGraphicsRectItem* bar=new QGraphicsRectItem( 0, 0, BAR_WIDTH, -barHeight, frame);
        bar->setPen(QPen(Qt::black,0));

        if( m_colorTable ){
            // get color from colortable based on center value of bar
            std::pair<double, double> interval=m_histogram.binInterval(i);
            double cv=(interval.first + interval.second)/2;
            QColor color=m_colorTable->map( cv );
            bar->setBrush(color);
        }
        else{
            bar->setBrush(Qt::red);
        }
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

    // this distorts the labels, need to use scale factor 1
    //ui->graphicsView->resetTransform();
    //ui->graphicsView->scale(ui->graphicsView->width()/scene->sceneRect().width(), ui->graphicsView->height()/scene->sceneRect().height());


    qreal ww=scene->sceneRect().width();
    if( ww>MAX_GRAPHICSVIEW_WIDTH ) ww=MAX_GRAPHICSVIEW_WIDTH;
    qreal hh=scene->sceneRect().height();
    if( hh>MAX_GRAPHICSVIEW_HEIGHT ) hh=MAX_GRAPHICSVIEW_HEIGHT;

    ui->graphicsView->setMinimumSize( ww, hh );
    update();
    //ui->graphicsView->setMinimumSize(std::min(scene->sceneRect().width(),1000.), std::min(scene->sceneRect().height(),1000.));
}


void HistogramDialog::keyPressEvent(QKeyEvent *ev){

    if( ev->key() == Qt::Key_Return){
        ev->accept(); // ate it
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}

