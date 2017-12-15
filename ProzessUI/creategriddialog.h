#ifndef CREATEGRIDDIALOG_H
#define CREATEGRIDDIALOG_H

#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class CreateGridDialog;
}

class CreateGridDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit CreateGridDialog(QWidget *parent = 0);
    ~CreateGridDialog();

    QMap<QString,QString> params();

public slots:
    void setProject( AVOProject* project);

private slots:
    void updateOkButton();

private:
    Ui::CreateGridDialog *ui;

    AVOProject* m_project=nullptr;      // parent must own project and not delete before dialog is finished!!!!

};

#endif // CREATEGRIDDIALOG_H
