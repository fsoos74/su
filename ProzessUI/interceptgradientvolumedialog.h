#ifndef INTERCEPTGRADIENTVOLUMEDIALOG_H
#define INTERCEPTGRADIENTVOLUMEDIALOG_H

#include <QDialog>
#include <QMap>
#include<QIntValidator>

#include "processparametersdialog.h"

namespace Ui {
class InterceptGradientVolumeDialog;
}

class InterceptGradientVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit InterceptGradientVolumeDialog(QWidget *parent = 0);
    ~InterceptGradientVolumeDialog();

    QMap<QString,QString> params();

public slots:
    void setDatasets( const QStringList&);
    void setWindowStartMS( int );
    void setWindowEndMS( int );

private slots:
    void updateOkButton();

private:
    Ui::InterceptGradientVolumeDialog *ui;
    QIntValidator* windowValidator;
};

#endif // INTERCEPTGRADIENTVOLUMEDIALOG_H
