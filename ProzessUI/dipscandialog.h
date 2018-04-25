#ifndef DIPSCANDIALOG_H
#define DIPSCANDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class DipScanDialog;
}

class DipScanDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit DipScanDialog(QWidget *parent = 0);
    ~DipScanDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);
    void setInputs(QStringList);            // this way we can use dialog for volumes or datasets

private slots:
    void updateOkButton();

private:
    Ui::DipScanDialog *ui;

    AVOProject* m_project;
};

#endif // DIPSCANDIALOG_H
