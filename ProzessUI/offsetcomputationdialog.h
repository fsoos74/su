#ifndef OFFSETCOMPUTATIONDIALOG_H
#define OFFSETCOMPUTATIONDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class OffsetComputationDialog;
}

class OffsetComputationDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit OffsetComputationDialog(QWidget *parent = 0);
    ~OffsetComputationDialog();

    QMap<QString,QString> params();

public slots:

    void setInputDatasets( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::OffsetComputationDialog *ui;
};

#endif // OffsetComputationDialog_H
