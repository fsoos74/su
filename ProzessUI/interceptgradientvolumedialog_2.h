#ifndef INTERCEPTGRADIENTVOLUMEDIALOG_2_H
#define INTERCEPTGRADIENTVOLUMEDIALOG_2_H

#include <QDialog>
#include <QMap>
#include<QIntValidator>

#include "processparametersdialog.h"

namespace Ui {
class InterceptGradientVolumeDialog_2;
}

class InterceptGradientVolumeDialog_2 : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit InterceptGradientVolumeDialog_2(QWidget *parent = 0);
    ~InterceptGradientVolumeDialog_2();

    QMap<QString,QString> params();

public slots:
    void setDatasets( const QStringList&);
    void setWindowStartMS( int );
    void setWindowEndMS( int );

private slots:
    void updateOkButton();

private:
    Ui::InterceptGradientVolumeDialog_2 *ui;
    QIntValidator* windowValidator;
};

#endif // INTERCEPTGRADIENTVOLUMEDIALOG_H
