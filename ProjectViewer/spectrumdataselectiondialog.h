#ifndef SPECTRUMDATASELECTIONDIALOG_H
#define SPECTRUMDATASELECTIONDIALOG_H

#include <QDialog>
#include "spectrumdefinition.h"

namespace Ui {
class SpectrumDataSelectionDialog;
}

class SpectrumDataSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpectrumDataSelectionDialog(QWidget *parent = 0);
    ~SpectrumDataSelectionDialog();

    QString datasetName();
    int inlineNumber();
    int crosslineNumber();
    int windowStartMS();
    int windowLengthMS();

public slots:
    void setDatasetNames( const QStringList&);
    void setInlineNumber(int);
    void setCrosslineNumber(int);
    void setWindowStartMS(int);
    void setWindowLengthMS(int);

signals:
    void dataSelected(SpectrumDefinition);

private slots:
    void on_pbAdd_clicked();

private:
    Ui::SpectrumDataSelectionDialog *ui;
};

#endif // SPECTRUMDATASELECTIONDIALOG_H
