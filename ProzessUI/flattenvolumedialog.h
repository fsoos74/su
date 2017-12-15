#ifndef FLATTENVOLUMEDIALOG_H
#define FLATTENVOLUMEDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class FlattenVolumeDialog;
}

class FlattenVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit FlattenVolumeDialog(QWidget *parent = 0);
    ~FlattenVolumeDialog();

    QMap<QString,QString> params();

public slots:
    void setInputs( const QStringList&);
    void setHorizons( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::FlattenVolumeDialog *ui;
};

#endif // CREATESLICEDIALOG_H
