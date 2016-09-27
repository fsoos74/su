#ifndef COLORBARWIDGET_H
#define COLORBARWIDGET_H

#include <QWidget>
#include <QVector>
#include<QString>
#include "colortable.h"


class ColorBarWidget : public QWidget
{
    Q_OBJECT
public:

    enum ScaleAlignment{ SCALE_LEFT, SCALE_RIGHT };

    explicit ColorBarWidget(QWidget *parent = 0);

    QVector<double> ticks()const{
        return m_ticks;
    }

    ScaleAlignment scaleAlignmen()const{
        return m_scaleAlignment;
    }

    const QString& label()const{
        return m_label;

    }

    const int steps()const{
        return m_steps;
    }

    std::pair<double,double> range()const{
        return m_range;
    }

signals:

    void colorTableChanged( ColorTable* );
    void ticksChanged( const QVector<double>&);
    void labelChanged( QString );
    void stepsChanged(int);
    void rangeChanged(std::pair<double,double>);

public slots:

    void setColorTable( ColorTable* ct);
    void setTicks( const QVector<double>& ticks);
    void setScaleAlignment(ScaleAlignment);
    void setLabel( const QString&);
    void setSteps(int);
    void setRange(std::pair<double,double>);
protected:

     void paintEvent( QPaintEvent *);

private slots:

     void refreshImage();
     void refreshTicks();

private:

     //const int N_STEPS=10; now this is a property
     const int BOX_WIDTH=30;
     const int BOX_LINE_WIDTH=2;
     const int MARK_SIZE=10;
     const int PAD_Y=10;

    int m_steps=10;
    std::pair<double,double> m_range;
    QVector<double> m_ticks;
    QImage m_image;
    ColorTable* m_colorTable=nullptr;
    ScaleAlignment m_scaleAlignment=SCALE_LEFT;
    QString m_label;
};

#endif // COLORBARWIDGET_H
