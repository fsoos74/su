#ifndef CASTAGNADIALOG_H
#define CASTAGNADIALOG_H

#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class CastagnaDialog;
}

class CastagnaDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit CastagnaDialog(QWidget *parent = 0);
    ~CastagnaDialog();

    QMap<QString,QString> params();

public slots:
    void setProject(AVOProject*);

private slots:

    void updateInputWells(QString);
    void updateOkButton();
    void updateCoefficients();

private:
    QStringList selectedWells();

    Ui::CastagnaDialog *ui;
    AVOProject* m_project=nullptr;
};


#endif // CASTAGNADIALOG_H
