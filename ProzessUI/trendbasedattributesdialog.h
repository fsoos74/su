#ifndef TRENDBASEDATTRIBUTESDIALOG_H
#define TRENDBASEDATTRIBUTESDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"


namespace Ui {
class TrendBasedAttributesDialog;
}

class TrendBasedAttributesDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:

    enum TypeIndex{ FF=0, LF, PI, AC };

    explicit TrendBasedAttributesDialog(QWidget *parent = 0);
    ~TrendBasedAttributesDialog();

    QMap<QString,QString> params();

    bool isVolumeMode()const{
        return m_volumeMode;
    }

public slots:
    void setInterceptList( const QStringList&);
    void setGradientList( const QStringList&);
    void setComputeTrend(bool);
    void setTrendIntercept(double);
    void setTrendAngle(double);
    void setVolumeMode(bool);

private slots:
    void updateOkButton();

private:
    Ui::TrendBasedAttributesDialog *ui;

    bool m_volumeMode=false;
};

#endif // TRENDBASEDATTRIBUTESDIALOG_H
