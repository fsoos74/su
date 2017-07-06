#ifndef CURVATUREVOLUMEDIALOG_H
#define CURVATUREVOLUMEDIALOG_H

#include <QDialog>
#include "processparametersdialog.h"


namespace Ui {
class CurvatureVolumeDialog;
}

class CurvatureVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:

    enum Mode{ Horizon, Volume };

    explicit CurvatureVolumeDialog(QWidget *parent = 0);
    ~CurvatureVolumeDialog();


    QMap<QString,QString> params();

public slots:
    void setInputs( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::CurvatureVolumeDialog *ui;
};

#endif // CURVATUREVOLUMEDIALOG_H
