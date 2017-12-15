#ifndef HISTOGRAMRANGESELECTIONDIALOG_H
#define HISTOGRAMRANGESELECTIONDIALOG_H

#include <QDialog>

#include <histogram.h>
#include <colortable.h>

namespace Ui {
class HistogramRangeSelectionDialog;
}

class HistogramRangeSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistogramRangeSelectionDialog(QWidget *parent = 0);
    ~HistogramRangeSelectionDialog();

    std::pair<double, double> range();

    bool isLocked();
    bool isInstantUpdates();

public slots:

    void setHistogram(const Histogram&);
    void setColorTable(ColorTable*);

    void setRange(std::pair<double, double>);
    void setRange(double,double);
    void setLocked( bool);
    void setInstantUpdates(bool);
    void updateRange();

signals:

    void rangeChanged( std::pair<double, double>);
    void rangeChanged(double,double);
    void lockChanged(bool);

protected:

    void keyPressEvent(QKeyEvent *)override;

private slots:
    void on_leMinimum_returnPressed();

    void on_leMaximum_returnPressed();

    void on_pbResetMinimum_clicked();

    void on_pbResetMaximum_clicked();

private:

    Ui::HistogramRangeSelectionDialog *ui;
};

#endif // HISTOGRAMRANGESELECTIONDIALOG_H
