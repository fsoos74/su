#ifndef VOLUMEIOTESTDIALOG_H
#define VOLUMEIOTESTDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class VolumeIOTestDialog;
}

class VolumeIOTestDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit VolumeIOTestDialog(QWidget *parent = 0);
    ~VolumeIOTestDialog();

    QMap<QString,QString> params();

public slots:
    void setInputs( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::VolumeIOTestDialog *ui;
};

#endif // VOLUMEIOTESTDIALOG_H
