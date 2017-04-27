#ifndef HORIZONCURVATUREDIALOG_H
#define HORIZONCURVATUREDIALOG_H

#include <QDialog>
#include "processparametersdialog.h"


namespace Ui {
class HorizonCurvatureDialog;
}

class HorizonCurvatureDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit HorizonCurvatureDialog(QWidget *parent = 0);
    ~HorizonCurvatureDialog();

    QMap<QString,QString> params();

public slots:
    void setHorizons( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::HorizonCurvatureDialog *ui;
};

#endif // HORIZONCURVATUREDIALOG_H
