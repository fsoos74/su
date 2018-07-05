#ifndef VOLUMECIRCLEDETECTIONDIALOG_H
#define VOLUMECIRCLEDETECTIONDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class VolumeCircleDetectionDialog;
}

class VolumeCircleDetectionDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit VolumeCircleDetectionDialog(QWidget *parent = 0);
    ~VolumeCircleDetectionDialog();

    QMap<QString,QString> params();

public slots:
    void setInputs( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::VolumeCircleDetectionDialog *ui;
};

#endif // VOLUMECIRCLEDETECTIONDIALOG_H
