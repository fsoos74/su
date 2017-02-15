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

    std::pair<double, double> defaultRange()const{
        return m_defaultRange;
    }

    bool isLocked();
    bool isInstantUpdates();

public slots:

    void setHistogram(const Histogram&);
    void setColorTable(ColorTable*);

    void setRange(std::pair<double, double>);
    void setDefaultRange( std::pair<double, double>) ;
    void setLocked( bool);
    void setInstantUpdates(bool);

signals:

    void rangeChanged( std::pair<double, double>);
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

    std::pair<double, double> m_defaultRange=std::make_pair(0., 1.);

};

#endif // HISTOGRAMRANGESELECTIONDIALOG_H
