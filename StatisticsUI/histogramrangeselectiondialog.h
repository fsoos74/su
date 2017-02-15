#ifndef HISTOGRAMRANGESELECTIONDIALOG_H
#define HISTOGRAMRANGESELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class HistogramRangeSelectionDialog;
}

class HistogramRangeSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistogramRangeSelectionDialog(QWidget *parent = 0);
    ~HistogramRangeSelectionDialog();

private:
    Ui::HistogramRangeSelectionDialog *ui;
};

#endif // HISTOGRAMRANGESELECTIONDIALOG_H
