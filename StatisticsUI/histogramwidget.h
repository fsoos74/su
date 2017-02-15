#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include <histogram.h>
#include<colortable.h>

class HistogramWidget : public QWidget
{
    Q_OBJECT

public:

    HistogramWidget( QWidget* parent=nullptr);

    const Histogram& histogram()const{
        return m_histogram;
    }

    ColorTable* colorTable()const{
        return m_colorTable;
    }

    // update on mouse move, no only at release
    bool isInstantUpdates()const{
        return m_instantUpdates;
    }

public slots:

    void setHistogram(const Histogram&);
    void setColorTable( ColorTable*);

    void setRange( std::pair<double, double> );
    void setPower( double );

    void setInstantUpdates(bool);

signals:

    void rangeChanged(std::pair<double,double> );
    void powerChanged(double);
    void instantUpdatesChanged(bool);

protected:

    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void leaveEvent(QEvent *event) override;

private:

    void handleMouseEvent( QMouseEvent* event );

    double xToValue(int)const;
    int valueToX(double)const;

    enum class MouseMode{ EditMinimum, EditMaximum, None };

    MouseMode m_mode=MouseMode::None;
    Histogram m_histogram;
    ColorTable* m_colorTable=nullptr;
    std::pair<double,double> m_range;
    double m_power;
    bool m_instantUpdates=true;
};

#endif // HISTOGRAMWIDGET_H
