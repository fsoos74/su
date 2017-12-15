#ifndef UNFLATTENVOLUMEDIALOG_H
#define UNFLATTENVOLUMEDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class UnflattenVolumeDialog;
}

class UnflattenVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit UnflattenVolumeDialog(QWidget *parent = 0);
    ~UnflattenVolumeDialog();

    QMap<QString,QString> params();

public slots:
    void setInputs( const QStringList&);
    void setHorizons( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::UnflattenVolumeDialog *ui;
};

#endif
