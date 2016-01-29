#ifndef COMPUTEINTERCEPTGRADIENTDIALOG_H
#define COMPUTEINTERCEPTGRADIENTDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class ComputeInterceptGradientDialog;
}

class ComputeInterceptGradientDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ComputeInterceptGradientDialog(QWidget *parent = 0);
    ~ComputeInterceptGradientDialog();

    QMap<QString,QString> params();

public slots:
    void setDatasets( const QStringList&);
    void setHorizons( const QStringList&);
    void setReductionMethods( const QStringList&);
    void setNumberOfSamples( int );

private slots:
    void updateOkButton();

private:
    Ui::ComputeInterceptGradientDialog *ui;

};

#endif // COMPUTEINTERCEPTGRADIENTDIALOG_H
