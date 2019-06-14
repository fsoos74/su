#ifndef VOLUMESTATISTICSDIALOG_H
#define VOLUMESTATISTICSDIALOG_H

#include <QDialog>
#include<QMap>


#include "processparametersdialog.h"

namespace Ui {
class VolumeStatisticsDialog;
}

class AVOProject;

class VolumeStatisticsDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit VolumeStatisticsDialog(QWidget *parent = 0);
    ~VolumeStatisticsDialog();

    QMap<QString,QString> params();

    void setProject(AVOProject*);

private slots:
    void updateHorizons();
    void updateInputVolumes();
    void updateOkButton();


private:
    Ui::VolumeStatisticsDialog *ui;
    AVOProject* m_project=nullptr;
};

#endif // VOLUMESTATISTICSDIALOG_H
