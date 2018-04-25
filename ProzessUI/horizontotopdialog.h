#ifndef HORIZONTODIALOG_H
#define HORIZONTODIALOG_H

#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class HorizonToTopDialog;
}

class HorizonToTopDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit HorizonToTopDialog(QWidget *parent = 0);
    ~HorizonToTopDialog();

    QMap<QString,QString> params();

public slots:
    void setProject(AVOProject*);

private slots:
    void updateOkButton();

private:
    QStringList selectedWells();

    Ui::HorizonToTopDialog *ui;
    AVOProject* m_project=nullptr;
};


#endif // HORIZONTOTOPDIALOG_H
