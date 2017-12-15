#ifndef VOLUMEMATHDIALOG_H
#define VOLUMEMATHDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class VolumeMathDialog;
}

class VolumeMathDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit VolumeMathDialog(QWidget *parent = 0);
    ~VolumeMathDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:
    void updateHorizons();
    void updateInputVolumes();
    void updateOkButton();
    void on_cbFunction_currentIndexChanged(const QString &arg1);

private:
    Ui::VolumeMathDialog *ui;

    AVOProject* m_project;
};

#endif // VOLUMEMATHDIALOG_H
