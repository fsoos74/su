#ifndef CURVATUREVOLUMEDIALOG_H
#define CURVATUREVOLUMEDIALOG_H


#include <processparametersdialog.h>

namespace Ui {
class CurvatureVolumeDialog;
}

class CurvatureVolumeDialog  : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit CurvatureVolumeDialog(QWidget *parent = 0);
    ~CurvatureVolumeDialog();

    QMap<QString,QString> params();

public slots:

    void setInputVolumes( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::CurvatureVolumeDialog *ui;

    QStringList m_inputVolumes;
};

#endif // CurvatureVolumeDIALOG_H
