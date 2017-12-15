#ifndef SMOOTHLOGDIALOG_H
#define SMOOTHLOGDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class SmoothLogDialog;
}

class SmoothLogDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit SmoothLogDialog(QWidget *parent = 0);
    ~SmoothLogDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:

    QStringList selectedWells();
    void updateLogs();
    void updateWells();
    void updateOkButton();

private:
    Ui::SmoothLogDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // SMOTTHLOGDIALOG_H
