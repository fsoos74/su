#ifndef COMPASSWIDGET_H
#define COMPASSWIDGET_H

#include <QWidget>

class CompassWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CompassWidget(QWidget *parent = 0);

    qreal azimuth()const{
        return m_azimuth;
    }

    int s1()const{
        return m_s1;
    }

signals:
    void s1Changed(int);
    void azimuthChanged(qreal);

public slots:
    void setAzimuth(qreal);
    void setS1(int);

protected:
    virtual void paintEvent(QPaintEvent*);

private:

    int m_s1=20;
    qreal m_azimuth=0;

};

#endif // COMPASSWIDGET_H
