#ifndef VOLUMEDIPDIALOG_H
#define VOLUMEDIPDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class VolumeDipDialog;
}

class VolumeDipDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit VolumeDipDialog(QWidget *parent = 0);
    ~VolumeDipDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:
    void updateInputVolumes();
    void updateOkButton();

private:
    Ui::VolumeDipDialog *ui;

    AVOProject* m_project;
};

#endif // VOLUMEDIPDIALOG_H
