#ifndef AXISCONFIGDIALOG_H
#define AXISCONFIGDIALOG_H

#include <QDialog>
#include "axis.h"


namespace Ui {
class AxisConfigDialog;
}

class AxisConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AxisConfigDialog(QWidget *parent = 0);
    ~AxisConfigDialog();

    QString name();
    QString unit();
    qreal minimum();
    qreal maximum();
    int type();
    bool isReversed();
    bool isAutomatic();
    qreal interval();
    int subTicks();
    bool labelSubTicks();
    int labelPrecision();

    static bool configAxis(Axis* axis, const QString& title="Configure Axis");

public slots:
    void setName(QString);
    void setUnit(QString);
    void setMinimum(qreal);
    void setMaximum(qreal);
    void setType(int);
    void setReversed(bool);
    void setAutomatic(bool);
    void setInterval(qreal);
    void setSubTicks(int);
    void setLabelSubTicks(bool);
    void setLabelPrecision(int);

private slots:
    void updateOkButton();

    void on_cbType_currentIndexChanged(int index);

private:
    Ui::AxisConfigDialog *ui;
};

#endif // AXISCONFIGDIALOG_H
