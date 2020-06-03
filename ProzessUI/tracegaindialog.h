#ifndef TRACEGAINDIALOG_H
#define TRACEGAINDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class TraceGainDialog;
}

class TraceGainDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit TraceGainDialog(QWidget *parent = 0);
    ~TraceGainDialog();

    QMap<QString,QString> params();

public slots:

    void setInputDatasets( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::TraceGainDialog *ui;
};

#endif // TraceGainDialog_H
