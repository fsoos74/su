#ifndef VELOCITYCONVERSIONDIALOG_H
#define VELOCITYCONVERSIONDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class VelocityConversionDialog;
}

class VelocityConversionDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit VelocityConversionDialog(QWidget *parent = 0);
    ~VelocityConversionDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:
    void updateInputVolumes();
    void updateOkButton();

private:
    Ui::VelocityConversionDialog *ui;

    AVOProject* m_project;
};

#endif // VELOCITYCONVERSIONDIALOG_H
