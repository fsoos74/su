#ifndef CURVATUREDIALOG_H
#define CURVATUREDIALOG_H

#include <QDialog>
#include "processparametersdialog.h"


namespace Ui {
class CurvatureDialog;
}

class CurvatureDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:

    enum Mode{ Horizon, Volume };

    explicit CurvatureDialog(QWidget *parent = 0);
    ~CurvatureDialog();


    QMap<QString,QString> params();

public slots:
    void setMode( Mode );
    void setInputs( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::CurvatureDialog *ui;
    Mode m_mode=Mode::Horizon;
};

#endif // CURVATUREDIALOG_H
