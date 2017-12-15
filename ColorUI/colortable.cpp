#include "colortable.h"

#include<iostream>
#include<cmath>
#include<QColor>

const int COLORTABLE_SIZE=256;

ColorTable::ColorTable(QObject *parent) : QObject(parent), m_colors(COLORTABLE_SIZE, qRgb(0,0,0)), m_range(0,1)
{
}

ColorTable::ColorTable( QObject* parent, const QVector<color_type>& cols, const std::pair<double, double>& r): QObject(parent), m_colors(cols),  m_range(r){
}


ColorTable::color_type ColorTable::map( const double val)const{

    Q_ASSERT( !m_colors.empty());


    // first make value between 0 and 1
    double x=m_range.second-m_range.first;
    if( !x ) return m_colors.front();           // special case for empty range, need to add approx ==0
    x=(val-m_range.first)/x;
    // apply power
    x=std::pow(x, m_power);
    //translate to index
    int idx=std::round(x*m_colors.size());//m_colors.size()*(val-m_range.first)/(m_range.second-m_range.first));



    if(idx<0) idx=0;
    if(idx>=m_colors.size()) idx=m_colors.size()-1;

    //if( m_colors.size()<1) return qRgb(0,0,255);

    return m_colors[idx];
}

void ColorTable::setColors( const QVector<color_type>& col){

    if( col==m_colors) return;

    m_colors=col;

    emit colorsChanged();
}

void ColorTable::setRange( std::pair<double,double> r ){
//std::cout<<"ColorTable::setRange locked="<<std::boolalpha<<m_lock<<std::endl;
    if( m_lock ) return;

    if( r==m_range ) return;

    m_range=r;

    emit( rangeChanged( m_range)  );

}

void ColorTable::setPower( double power){

    if( m_lock ) return;

    if( power == m_power ) return;

    m_power=power;

    emit powerChanged(power);
}

void ColorTable::setRange( double min,double max ){

    if( m_lock ) return;

    setRange( std::make_pair(min,max) );
}

void ColorTable::setLocked(bool on){

    if( on == m_lock ) return;

    m_lock=on;

    emit lockedChanged(on);
}

void ColorTable::blend( QVector<QRgb>& cols, int first, int last){

    int red1=qRed(cols[first]);
    int green1=qGreen(cols[first]);
    int blue1=qBlue(cols[first]);

    int red2=qRed(cols[last]);
    int green2=qGreen(cols[last]);
    int blue2=qBlue(cols[last]);

    for( int i=first+1; i<last; i++){

        int red=red1 + (i-first)*(red2-red1)/(last-first+1);
        int green=green1 + (i-first)*(green2-green1)/(last-first+1);
        int blue=blue1 + (i-first)*(blue2-blue1)/(last-first+1);
        cols[i]=qRgb( red, green, blue);
    }


}

// wave=380-780
uint rgbFromWaveLength(double wave)
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    if (wave >= 380.0 && wave <= 440.0) {
        r = -1.0 * (wave - 440.0) / (440.0 - 380.0);
        b = 1.0;
    } else if (wave >= 440.0 && wave <= 490.0) {
        g = (wave - 440.0) / (490.0 - 440.0);
        b = 1.0;
    } else if (wave >= 490.0 && wave <= 510.0) {
        g = 1.0;
        b = -1.0 * (wave - 510.0) / (510.0 - 490.0);
    } else if (wave >= 510.0 && wave <= 580.0) {
        r = (wave - 510.0) / (580.0 - 510.0);
        g = 1.0;
    } else if (wave >= 580.0 && wave <= 645.0) {
        r = 1.0;
        g = -1.0 * (wave - 645.0) / (645.0 - 580.0);
    } else if (wave >= 645.0 && wave <= 780.0) {
        r = 1.0;
    }

    double s = 1.0;
    if (wave > 700.0)
        s = 0.3 + 0.7 * (780.0 - wave) / (780.0 - 700.0);
    else if (wave <  420.0)
        s = 0.3 + 0.7 * (wave - 380.0) / (420.0 - 380.0);

    r = std::pow(r * s, 0.8);
    g = std::pow(g * s, 0.8);
    b = std::pow(b * s, 0.8);
    return qRgb(int(r * 255), int(g * 255), int(b * 255));
}

QVector<QRgb> ColorTable::defaultColors(){

    const int DEFAULT_SIZE=256;

    QVector<QRgb> cols(DEFAULT_SIZE, qRgb(0,0,0));

    for( int i=0; i<cols.size(); i++){
        double lambda=380. + (780.-380.)/(cols.size()-1)*i;
        cols[i]=rgbFromWaveLength(lambda);
    }
/*
    cols[0]=qRgb(255,0,64);
    cols[63]=qRgb(0,0,255);
    blend( cols, 0, 63);

    cols[127]=qRgb(0,255,255);
    blend(cols, 63,127);

    cols[191]=qRgb(255,255,0);
    blend(cols,127,191);
    cols[255]=qRgb(255,0,64);
    blend(cols,191,255);
*/


    return cols;
}

QVector<QRgb> ColorTable::grayscale(){

    const int SIZE=256;

    QVector<QRgb> cols(SIZE, qRgb(0,0,0));

    cols[SIZE-1]=qRgb(255,255,255);
    blend(cols, 0, SIZE-1);

    return cols;
}
