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

    void updateHistogram();
    void updateDataMinMax();

protected:

    void keyPressEvent(QKeyEvent* event);

private:

    void updateMaximumCount();
    void updateScene();


    Ui::HistogramDialog *ui;

    QVector<double> m_data;
    double m_dataMin=0;
    double m_dataMax=1;

    Histogram m_histogram;
    size_t m_maximumCount;
};

#endif // HISTOGRAMDIALOG_H
