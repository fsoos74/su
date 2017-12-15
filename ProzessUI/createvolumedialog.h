#ifndef CREATEVOLUMEDIALOG_H
#define CREATEVOLUMEDIALOG_H

#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class CreateVolumeDialog;
}

class CreateVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit CreateVolumeDialog(QWidget *parent = 0);
    ~CreateVolumeDialog();

    QMap<QString,QString> params();

public slots:
    void setProject( AVOProject* project);

private slots:
    void updateOkButton();

private:
    Ui::CreateVolumeDialog *ui;

    AVOProject* m_project=nullptr;      // parent must own project and not delete before dialog is finished!!!!

};

#endif // CREATEVOLUMEDIALOG_H
