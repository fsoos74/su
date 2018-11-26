#ifndef HISTOGRAMCOLORBARWIDGET_H
#define HISTOGRAMCOLORBARWIDGET_H

#include <QObject>
#include <QWidget>
#include <colortable.h>
#include <histogram.h>


// histogram ill be created dynamically from colortable range and data
class HistogramColorBarWidget : public QWidget
{
    Q_OBJECT

public:

    explicit HistogramColorBarWidget(QWidget *parent = 0);

    const Histogram& histogram()const{
        return m_histogram;
    }

    ColorTable* colorTable()const{
        return m_colorTable;
    }

    Qt::Orientation orientation()const{
        return m_orientation;
    }

    int scalePrecision()const{
        return m_scalePrecision;
    }

public slots:

    void setColorTable( ColorTable*);
    void setData( float* begin, size_t count, float null_value );
    void setOrientation(Qt::Orientation);
    void setScalePrecision(int);

protected:

    void paintEvent(QPaintEvent *) override;

private slots:

    void updateHistogram();

private:

    float* m_data_beg=nullptr;
    size_t m_data_count=0;;
    float m_null_value=0;
    ColorTable* m_colorTable=nullptr;
    Histogram m_histogram;
    Qt::Orientation m_orientation=Qt::Vertical;
    int m_scalePrecision=4;
};

#endif // HISTOGRAMCOLORBARWIDGET_H
