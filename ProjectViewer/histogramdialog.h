#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include <QDialog>

#include<histogram.h>

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

private:

    void updateMaximumCount();
    void updateScene();

    Ui::HistogramDialog *ui;
    Histogram m_histogram;
    size_t m_maximumCount;
};

#endif // HISTOGRAMDIALOG_H
