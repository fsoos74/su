#ifndef REPLACEBADTRACESDIALOG_H
#define REPLACEBADTRACESDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class ReplaceBadTracesDialog;
}

class ReplaceBadTracesDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ReplaceBadTracesDialog(QWidget *parent = 0);
    ~ReplaceBadTracesDialog();

    QMap<QString,QString> params();

public slots:

    void setInputDatasets( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::ReplaceBadTracesDialog *ui;
};

#endif // ReplaceBadTracesDialog_H
