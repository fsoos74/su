#ifndef EXPORTLASDIALOG_H
#define EXPORTLASDIALOG_H

#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class ExportLASDialog;
}

class ExportLASDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ExportLASDialog(QWidget *parent = 0);
    ~ExportLASDialog();

    QMap<QString,QString> params();

public slots:
    void setProject(AVOProject*);

private slots:
    void updateInputLogs(QString);
    void updateOkButton();

    void on_pbSelect_clicked();

private:
    QStringList selectedLogs();

    Ui::ExportLASDialog *ui;
    AVOProject* m_project=nullptr;
};


#endif // EXPORTLASDIALOG_H
