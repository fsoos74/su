#ifndef CROPVOLUMEDIALOG_H
#define CROPVOLUMEDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class CropVolumeDialog;
}

class CropVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit CropVolumeDialog(QWidget *parent = 0);
    ~CropVolumeDialog();

    QMap<QString,QString> params();

public slots:

    void setInputVolumes( const QStringList&);
    void setInlineRange(int min, int max);
    void setCrosslineRange(int min, int max);
    void setTimeRange(int min, int max);


private slots:
    void updateOkButton();

private:
    Ui::CropVolumeDialog *ui;
};

#endif // CROPVOLUMEDIALOG_H
