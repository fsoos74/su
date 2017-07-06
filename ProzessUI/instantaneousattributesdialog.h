#ifndef INSTANTANEOUSATTRIBUTESDIALOG_H
#define INSTANTANEOUSATTRIBUTESDIALOG_H

#include <QDialog>
#include<QMap>

#include "processparametersdialog.h"

namespace Ui {
class InstantaneousAttributesDialog;
}

class InstantaneousAttributesDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit InstantaneousAttributesDialog(QWidget *parent = 0);
    ~InstantaneousAttributesDialog();

    QMap<QString,QString> params();

public slots:
    void setInputVolumes( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::InstantaneousAttributesDialog *ui;
};

#endif // INSTANTANEOUSATTRIBUTESDIALOG_H
