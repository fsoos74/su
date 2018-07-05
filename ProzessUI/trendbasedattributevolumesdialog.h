#ifndef TRENDBASEDATTRIBUTEVOLUMESDIALOG_H
#define TRENDBASEDATTRIBUTEVOLUMESDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"


namespace Ui {
class TrendBasedAttributeVolumesDialog;
}

class TrendBasedAttributeVolumesDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:

    enum TypeIndex{ FF=0, LF, PI, AC };

    explicit TrendBasedAttributeVolumesDialog(QWidget *parent = 0);
    ~TrendBasedAttributeVolumesDialog();

    QMap<QString,QString> params();

public slots:
    void setInterceptList( const QStringList&);
    void setGradientList( const QStringList&);
    void setHorizons( const QStringList&);
    void setComputeTrend(bool);
    void setTrendIntercept(double);
    void setTrendAngle(double);

private slots:
    void updateOkButton();

private:
    Ui::TrendBasedAttributeVolumesDialog *ui;
};

#endif // TRENDBASEDATTRIBUTESDIALOG_H
