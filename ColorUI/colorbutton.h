#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include<QPushButton>
#include<QColor>
#include<QIcon>

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

protected:

    void resizeEvent(QResizeEvent*);

private:

    void updateIcon();

    QColor m_color;
    QIcon m_icon;
};

#endif // COLORBUTTON_H
