#ifndef PUNCHOUTVOLUMEDIALOG_H
#define PUNCHOUTVOLUMEDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"
#include <avoproject.h>

namespace Ui {
class PunchOutVolumeDialog;
}

class PunchOutVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit PunchOutVolumeDialog(QWidget *parent = 0);
    ~PunchOutVolumeDialog();

    QMap<QString,QString> params();

    void setProject(AVOProject*);

private slots:
    void updateOkButton();

private:
    Ui::PunchOutVolumeDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // PUNCHOUTVOLUMEDIALOG_H
