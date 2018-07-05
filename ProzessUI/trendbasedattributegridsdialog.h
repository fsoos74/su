#ifndef TRENDBASEDATTRIBUTEGRIDSDIALOG_H
#define TRENDBASEDATTRIBUTEGRIDSDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"


namespace Ui {
class TrendBasedAttributeGridsDialog;
}

class TrendBasedAttributeGridsDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:

    enum TypeIndex{ FF=0, LF, PI, AC };

    explicit TrendBasedAttributeGridsDialog(QWidget *parent = 0);
    ~TrendBasedAttributeGridsDialog();

    QMap<QString,QString> params();

public slots:
    void setInterceptList( const QStringList&);
    void setGradientList( const QStringList&);
    void setComputeTrend(bool);
    void setTrendIntercept(double);
    void setTrendAngle(double);

private slots:
    void updateOkButton();

private:
    Ui::TrendBasedAttributeGridsDialog *ui;
};

#endif // TRENDBASEDATTRIBUTESDIALOG_H
