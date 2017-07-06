#ifndef SEMBLANCEVOLUMEDIALOG_H
#define SEMBLANCEVOLUMEDIALOG_H

#include <QDialog>
#include<QMap>

#include "processparametersdialog.h"

namespace Ui {
class SemblanceVolumeDialog;
}

class SemblanceVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit SemblanceVolumeDialog(QWidget *parent = 0);
    ~SemblanceVolumeDialog();

    QMap<QString,QString> params();

public slots:
    void setInputVolumes( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::SemblanceVolumeDialog *ui;
};

#endif // SEMBLANCEVOLUMEDIALOG_H
