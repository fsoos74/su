#ifndef EXPORTLOGDIALOG_H
#define EXPORTDIALOG_H

#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class ExportLogDialog;
}

class ExportLogDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ExportLogDialog(QWidget *parent = 0);
    ~ExportLogDialog();

    QMap<QString,QString> params();

public slots:
    void setProject(AVOProject*);

private slots:

    void updateInputLogs(QString);
    void updateOkButton();

    void on_pbBrowse_clicked();

private:

    QStringList selectedLogs();

    Ui::ExportLogDialog *ui;
    AVOProject* m_project=nullptr;
};


#endif // EXPORTLOGDIALOG_H
