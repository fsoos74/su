#include "colorbutton.h"

ColorButton::ColorButton( QWidget* parent, const QColor& col ) : QPushButton(parent), m_color(col)
{

}


void ColorButton::setColor( const QColor& c ){

    if(c==m_color) return;

    m_color=c;

    updateIcon();

    emit colorChanged(c);
}


void ColorButton::resizeEvent(QResizeEvent *){

    updateIcon();
}


void ColorButton::updateIcon(){

    QPixmap pixmap( size());
    pixmap.fill(m_color);
    m_icon=QIcon( pixmap );
    setIcon(m_icon);
}
