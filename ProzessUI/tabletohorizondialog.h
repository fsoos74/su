#ifndef TABLETOHORIZONDIALOG_H
#define TABLETOHORIZONDIALOG_H

#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class TableToHorizonDialog;
}

class TableToHorizonDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit TableToHorizonDialog(QWidget *parent = 0);
    ~TableToHorizonDialog();

    QMap<QString,QString> params();

public slots:
    void setProject(AVOProject*);

private slots:
    void updateOkButton();

private:
    QStringList selectedWells();

    Ui::TableToHorizonDialog *ui;
    AVOProject* m_project=nullptr;
};


#endif // TABLETOHORIZONDIALOG_H
