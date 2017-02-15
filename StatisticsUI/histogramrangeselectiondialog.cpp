#include "histogramrangeselectiondialog.h"
#include "ui_histogramrangeselectiondialog.h"

HistogramRangeSelectionDialog::HistogramRangeSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistogramRangeSelectionDialog)
{
    ui->setupUi(this);
}

HistogramRangeSelectionDialog::~HistogramRangeSelectionDialog()
{
    delete ui;
}
