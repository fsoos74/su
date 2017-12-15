#ifndef VOLUMESTATISTICSDIALOG_H
#define VOLUMESTATISTICSDIALOG_H

#include <QDialog>
#include<QMap>

#include "processparametersdialog.h"

namespace Ui {
class VolumeStatisticsDialog;
}

class VolumeStatisticsDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit VolumeStatisticsDialog(QWidget *parent = 0);
    ~VolumeStatisticsDialog();

    QMap<QString,QString> params();

public slots:
    void setInputVolumes( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::VolumeStatisticsDialog *ui;
};

#endif // VOLUMESTATISTICSDIALOG_H
