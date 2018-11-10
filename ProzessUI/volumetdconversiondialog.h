#ifndef VOLUMETDCONVERSIONDIALOG_H
#define VOLUMETDCONVERSIONDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class VolumeTDConversionDialog;
}

class VolumeTDConversionDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit VolumeTDConversionDialog(QWidget *parent = 0);
    ~VolumeTDConversionDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:
    void updateInputVolumes();
    void updateOkButton();

private:
    Ui::VolumeTDConversionDialog *ui;

    AVOProject* m_project;
};

#endif // VOLUMETDCONVERSIONDIALOG_H
