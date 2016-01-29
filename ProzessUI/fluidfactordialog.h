#ifndef FLUIDFACTORDIALOG_H
#define FLUIDFACTORDIALOG_H

#include <QDialog>
#include<QMap>

#include "processparametersdialog.h"

namespace Ui {
class FluidFactorDialog;
}

class FluidFactorDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit FluidFactorDialog(QWidget *parent = 0);
    ~FluidFactorDialog();

    QMap<QString,QString> params();

public slots:
    void setInterceptGrids( const QStringList&);
    void setGradientGrids( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::FluidFactorDialog *ui;
};

#endif // FLUIDFACTORDIALOG_H
