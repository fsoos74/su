#ifndef TRACESCALINGDIALOG_H
#define TRACESCALINGDIALOG_H

#include <QDialog>
#include <gatherscaler.h>
namespace Ui {
class TraceScalingDialog;
}

class TraceScalingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TraceScalingDialog(QWidget *parent = 0);
    ~TraceScalingDialog();

public slots:
    void setScalingMode( GatherScaler::Mode mode);
    void setScalingFactor(qreal);

signals:
    void scalingModeChanged( GatherScaler::Mode);
    void scalingFactorChanged( qreal );

private slots:
    void on_cbScalingMode_currentIndexChanged(int index);
    void on_leScalingFactor_returnPressed();

private:
    Ui::TraceScalingDialog *ui;
};

#endif // TRACESCALINGDIALOG_H
