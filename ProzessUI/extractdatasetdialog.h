#ifndef EXTRACTDATASETDIALOG_H
#define EXTRACTDATASETDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class ExtractDatasetDialog;
}

class ExtractDatasetDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ExtractDatasetDialog(QWidget *parent = 0);
    ~ExtractDatasetDialog();

    QMap<QString,QString> params();

public slots:

    void setInputVolumes( const QStringList&);
    void setInlineRange(int min, int max);
    void setCrosslineRange(int min, int max);
    void setTimeRange(int min, int max);


private slots:
    void updateOkButton();

private:
    Ui::ExtractDatasetDialog *ui;
};

#endif // ExtractDatasetDialog_H
