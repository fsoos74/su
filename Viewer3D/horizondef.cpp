#include "horizondef.h"
#include<QMap>

namespace{
QMap<HorizonDef::ColorStyle,QString> lookup{
    {HorizonDef::ColorStyle::Fixed, "Fixed"},
    {HorizonDef::ColorStyle::Gradient, "Gradient"},
    {HorizonDef::ColorStyle::Volume, "Volume"}
};
}

HorizonDef::HorizonDef(std::shared_ptr<Grid2D<float>> horizon,
        ColorStyle cs, QColor color, qreal(alpha), qreal gradient, QString volume, int delay):
    m_horizon(horizon), m_colorStyle(cs), m_color(color), m_alpha(alpha), m_colorGradient(gradient),
    m_volume(volume), m_delay(delay){
}

QString HorizonDef::toQString(HorizonDef::ColorStyle cs){
    return lookup.value(cs);
}

HorizonDef::ColorStyle HorizonDef::toColorStyle(QString str){
    return lookup.key(str);
}

void HorizonDef::setHorizon(std::shared_ptr<Grid2D<float> > h){
    m_horizon=h;
}

void HorizonDef::setColorStyle(ColorStyle cs){
    m_colorStyle=cs;
}

void HorizonDef::setColor(QColor c){
    m_color=c;
}

void HorizonDef::setAlpha(qreal a){
    m_alpha=a;
}

void HorizonDef::setColorGradient(qreal g){
    m_colorGradient=g;
}

void HorizonDef::setVolume(QString s){
    m_volume=s;
}

void HorizonDef::setDelay(int d){
    m_delay=d;
}
