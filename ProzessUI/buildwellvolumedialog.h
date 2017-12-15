#ifndef BUILDWELLVOLUMEDIALOG_H
#define BUILDWELLVOLUMEDIALOG_H

#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class BuildWellVolumeDialog;
}

class BuildWellVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit BuildWellVolumeDialog(QWidget *parent = 0);
    ~BuildWellVolumeDialog();

    QMap<QString,QString> params();

public slots:
    void setProject( AVOProject* project);

private slots:
    void updateOkButton();
    void updateHorizons();          // all horizons from project
    void updateLogs();              // all logs from project
    void updateWells(QString log);  // find wells which have log

private:
    QStringList wellList();

    Ui::BuildWellVolumeDialog *ui;

    AVOProject* m_project=nullptr;      // parent must own project and not delete before dialog is finished!!!!

};

#endif // BUILDWELLVOLUMEDIALOG_H
