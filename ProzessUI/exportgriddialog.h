#ifndef EXPORTGRIDDIALOG_H
#define EXPORTGRIDDIALOG_H

#include <processparametersdialog.h>
#include <gridtype.h>
#include <avoproject.h>


namespace Ui {
class ExportGridDialog;
}

class ExportGridDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ExportGridDialog(QWidget *parent = 0);
    ~ExportGridDialog();

    QMap<QString,QString> params() override;

public slots:
    void setProject(AVOProject*);
    void setType(GridType);
    void setName(QString);

private slots:
    void setType(QString);
    void updateOkButton();
    void on_pbSelect_clicked();

private:
    Ui::ExportGridDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // EXPORTGRIDDIALOG_H
