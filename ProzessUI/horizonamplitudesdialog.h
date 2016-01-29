#ifndef HORIZONAMPLITUDESDIALOG_H
#define HORIZONAMPLITUDESDIALOG_H

#include <QDialog>
#include "processparametersdialog.h"


namespace Ui {
class HorizonAmplitudesDialog;
}

class HorizonAmplitudesDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit HorizonAmplitudesDialog(QWidget *parent = 0);
    ~HorizonAmplitudesDialog();

    QMap<QString,QString> params();

public slots:
    void setDatasets( const QStringList&);
    void setHorizons( const QStringList&);
    void setMethods( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::HorizonAmplitudesDialog *ui;
};

#endif // HORIZONAMPLITUDESDIALOG_H
