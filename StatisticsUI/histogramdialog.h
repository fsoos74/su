#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include <QDialog>
#include <algorithm>
#include<histogram.h>

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

public slots:

    void setData( QVector<double> );
    void setMinimumFromData();
    void setMaximumFromData();

private slots:

    void updateStatisticsControls();
    void updatePlotControlsFromData();
    void updatePlotDataFromControls();
    void updateHistogram();

protected:

    void keyPressEvent(QKeyEvent* event);

private:

    void computeStatistics();
    void updateMaximumCount();
    void updateScene();


    Ui::HistogramDialog *ui;

    QVector<double> m_data;
    long m_dataCount=0;
    double m_dataMin=0;
    double m_dataMax=1;
    double m_dataMean;
    double m_dataSigma;
    double m_dataMedian;
    double m_dataRMS;

    double m_plotMin;
    double m_plotMax;
    double m_plotBinWidth;

    Histogram m_histogram;
    size_t m_maximumCount;
};

#endif // HISTOGRAMDIALOG_H
