#include "smartcolorbarwidget.h"
#include<QPainter>

SmartColorBarWidget::SmartColorBarWidget(QWidget *parent) : QWidget(parent)
{

}


void SmartColorBarWidget::setColorTable(ColorTable* colorTable){
    if(colorTable==mColorTable) return;
    if(mColorTable){
        disconnect(mColorTable,  SIGNAL(colorsChanged()), this, SLOT(update()));
        disconnect(mColorTable, SIGNAL(rangeChanged( std::pair<double,double> )), this, SLOT(update()));
        disconnect(mColorTable, SIGNAL(powerChanged( double )), this, SLOT(update()));
    }
    mColorTable=colorTable;
    if(mColorTable){
        connect(mColorTable, SIGNAL(colorsChanged()), this, SLOT(update()));
        connect(mColorTable, SIGNAL(rangeChanged( std::pair<double,double> )), this, SLOT(update()));
        connect(mColorTable, SIGNAL(powerChanged( double )), this, SLOT(update()));
    }
    update();
}

void SmartColorBarWidget::setHistogram(Histogram histogram){
    mHistogram=histogram;
    update();
}

void SmartColorBarWidget::paintEvent(QPaintEvent* event){
    const int PRECISION=4;
    QWidget::paintEvent(event);
    QPainter painter(this);
    // reserve spacw for top and bottom histogram range value label
    auto gap=3*painter.fontMetrics().xHeight();
    auto histHeight=height()-2*gap;

    // draw histogram
    // fill with non-uniform background such that all colors from colortable will show
    painter.fillRect( 0, gap, width(), histHeight, QBrush( Qt::darkGray, Qt::Dense4Pattern ) );
    if(!mColorTable) return;
    auto minMaxCount=std::minmax_element(mHistogram.begin(), mHistogram.end());
    auto minCount=*minMaxCount.first;
    auto maxCount=*minMaxCount.second;
    auto min=mHistogram.minimum();//  mColorTable->range().first;
    auto max=mHistogram.maximum();//mColorTable->range().second;
    for(int i=0; i<histHeight; i++){
        auto y=gap+histHeight-1-i;    // largest value on top, smallest at bottom
        auto x= min + i*(max-min)/histHeight;
        auto color=mColorTable->map(x);
        int j=i*static_cast<int>(histogram().binCount())/histHeight;
        int len=width()*(mHistogram.binValue(j)-minCount)/(maxCount-minCount);
        painter.setPen(color);
        painter.drawLine(0,y,len,y);
    }

    // draw histogram (data) range
    painter.setPen(Qt::darkGray);
    painter.drawText(rect(),Qt::AlignLeft|Qt::AlignTop, QString::number(max,'g',PRECISION));
    painter.drawText(rect(),Qt::AlignLeft|Qt::AlignBottom, QString::number(min,'g',PRECISION));
    // draw scaling range
    auto maxc=mColorTable->range().second;
    auto maxy=gap+histHeight-histHeight*(maxc-min)/(max-min);
    painter.drawLine(0,maxy,width(),maxy);
    painter.drawText(0,0,width(),maxy, Qt::AlignRight|Qt::AlignBottom,
                     QString::number(maxc,'g',PRECISION));
    auto minc=mColorTable->range().first;
    auto miny=gap+histHeight-histHeight*(minc-min)/(max-min);
    painter.drawLine(0,miny,width(),miny);
    painter.drawText(0,miny,width(),height()-miny,
                     Qt::AlignRight|Qt::AlignTop,QString::number(minc,'g',PRECISION));
}
