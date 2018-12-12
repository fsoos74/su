#ifndef ROCKPROPERTIESDIALOG_H
#define ROCKPROPERTIESDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class RockPropertiesDialog;
}

class RockPropertiesDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit RockPropertiesDialog(QWidget *parent = 0);
    ~RockPropertiesDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:

    void updateLogs();
    void updateWells();
    void updateOkButton();

private:
    Ui::RockPropertiesDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // ROCKPROPERTIESDIALOG_H
