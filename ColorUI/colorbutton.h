#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include<QPushButton>
#include<QColor>

class ColorButton : public QPushButton
{
Q_OBJECT

public:

    ColorButton( QWidget* parent=nullptr, const QColor& col=Qt::black );

    const QColor& color()const{
        return m_color;
    }

signals:
    void colorChanged( QColor );

public slots:
    void setColor( const QColor&);

private slots:
    void chooseColor();

private:
    void updateBackground();

    QColor m_color;
};

#endif // COLORBUTTON_H
