#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include <QDialog>
#include <algorithm>
#include<histogram.h>
#include<colortable.h>
#include<QVector>



namespace Ui {
class HistogramDialog;
}

class HistogramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistogramDialog(QWidget *parent = 0);
    ~HistogramDialog();

    void setHistogram(const Histogram&);

    ColorTable* colorTable()const{
        return m_colorTable;
    }

public slots:

    void setData( QVector<double> );
    void setMinimumFromData();
    void setMaximumFromData();
    void setColorTable(ColorTable*);

private slots:

    void updateStatisticsControls();
    void updatePlotControlsFromData();
    void updatePlotDataFromControls();
    void updateHistogram();
    void updateScene();


protected:

    void keyPressEvent(QKeyEvent* event);

private:

    void computeStatistics();
    void updateMaximumCount();


    Ui::HistogramDialog *ui;

    ColorTable* m_colorTable=nullptr;

    QVector<double> m_data;
    long m_dataCount=0;
    double m_dataMin=0;
    double m_dataMax=1;
    double m_dataMean;
    double m_dataSigma;
    double m_dataMedian;
    double m_dataRMS;

    double m_plotMinX;
    double m_plotMaxX;
    double m_plotBinWidth;
    double m_plotMinY;
    double m_plotMaxY;
    Histogram m_histogram;
    size_t m_maximumCount;
};

#endif // HISTOGRAMDIALOG_H
