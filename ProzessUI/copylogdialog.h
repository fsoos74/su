#ifndef COPYLOGDIALOG_H
#define COPYLOGDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class CopyLogDialog;
}

class CopyLogDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit CopyLogDialog(QWidget *parent = 0);
    ~CopyLogDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:

    void updateLogs();
    void updateWells();
    void updateOkButton();

private:
    Ui::CopyLogDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // COPYLOGDIALOG_H
