#ifndef FLUIDFACTORVOLUMEDIALOG_H
#define FLUIDFACTORVOLUMEDIALOG_H

#include <QDialog>
#include<QMap>

#include "processparametersdialog.h"

namespace Ui {
class FluidFactorVolumeDialog;
}

class FluidFactorVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit FluidFactorVolumeDialog(QWidget *parent = 0);
    ~FluidFactorVolumeDialog();

    QMap<QString,QString> params();

public slots:
    void setInterceptVolumes( const QStringList&);
    void setGradientVolumes( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::FluidFactorVolumeDialog *ui;
};

#endif // FLUIDFACTORVOLUMEDIALOG_H
