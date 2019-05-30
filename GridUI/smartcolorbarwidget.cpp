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
    QPainter painter(this);
    // fill with non-uniform background such that all colors from colortable will show
    painter.fillRect( rect(), QBrush( Qt::darkGray, Qt::Dense4Pattern ) );
    if(!mColorTable) return;
    auto minMaxCount=std::minmax_element(mHistogram.begin(), mHistogram.end());
    auto minCount=*minMaxCount.first;
    auto maxCount=*minMaxCount.second;
    auto min=mHistogram.minimum();//  mColorTable->range().first;
    auto max=mHistogram.maximum();//mColorTable->range().second;
    for(int i=0; i<height(); i++){
        auto y=height()-1-i;    // largest value on top, smallest at bottom
        auto x= min + i*(max-min)/height();
        auto color=mColorTable->map(x);
        int j=i*static_cast<int>(histogram().binCount())/height();
        int len=width()*(mHistogram.binValue(j)-minCount)/(maxCount-minCount);
        painter.setPen(color);
        painter.drawLine(0,y,len,y);
    }

}
