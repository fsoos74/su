#include "colorbutton.h"
#include <QColorDialog>

ColorButton::ColorButton( QWidget* parent, const QColor& col ) : QPushButton(parent), m_color(col)
{
    setAutoFillBackground(true);
    updateBackground();
}

void ColorButton::setColor( const QColor& c ){

    if(c==m_color) return;

    m_color=c;
    updateBackground();

    emit colorChanged(c);
}

void ColorButton::chooseColor(){

    auto c=QColorDialog::getColor(m_color, this, "Select Color");
    if( c.isValid()) setColor(c);
}

void ColorButton::updateBackground(){
    QPalette pal;
    pal.setColor(QPalette::Button,m_color);
    setPalette(pal);
    update();
}
