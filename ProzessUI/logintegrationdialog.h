#ifndef LOGINTEGRATIONDIALOG_H
#define LOGINTEGRATIONDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class LogIntegrationDialog;
}

class LogIntegrationDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit LogIntegrationDialog(QWidget *parent = 0);
    ~LogIntegrationDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:

    QStringList selectedWells();
    void updateLogs();
    void updateWells();
    void updateOkButton();

private:
    Ui::LogIntegrationDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // LOGIntegrationDIALOG_H
