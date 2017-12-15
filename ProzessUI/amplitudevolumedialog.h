#ifndef AMPLITUDEVOLUMEDIALOG_H
#define AMPLITUDEVOLUMEDIALOG_H

#include <QDialog>
#include <QMap>
#include <avoproject.h>

#include "processparametersdialog.h"


namespace Ui {
class AmplitudeVolumeDialog;
}

class AmplitudeVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit AmplitudeVolumeDialog(QWidget *parent = 0);
    ~AmplitudeVolumeDialog();

    QMap<QString,QString> params();

    int minIline();
    int maxIline();
    int minXline();
    int maxXline();
    int minZ();
    int maxZ();

public slots:
    void setProject( AVOProject* p);
    void setMinIline(int);
    void setMaxIline(int);
    void setMinXline(int);
    void setMaxXline(int);
    void setMinZ(int);
    void setMaxZ(int);

private slots:
    void updateOkButton();
    void updateDatasets();

private:
    Ui::AmplitudeVolumeDialog *ui;
    AVOProject* m_project=nullptr;
};

#endif // AMPLITUDEVOLUMEDIALOG_H
