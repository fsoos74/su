#ifndef VSHALEDIALOG_H
#define VSHALEDIALOG_H

#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class VShaleDialog;
}

class VShaleDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit VShaleDialog(QWidget *parent = 0);
    ~VShaleDialog();

    QMap<QString,QString> params();

public slots:
    void setProject(AVOProject*);

private slots:

    void updateInputWells();
    void updateTops();
    void updateOkButton();
    void on_cbBlockingMode_currentIndexChanged(const QString &arg1);

private:
    QStringList selectedWells();
    QStringList selectedTops();

    Ui::VShaleDialog *ui;
    AVOProject* m_project=nullptr;
};


#endif // VSHALEDIALOG_H
