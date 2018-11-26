#ifndef AXISTICKSCONFIGDIALOG_H
#define AXISTICKSCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class AxisTicksConfigDialog;
}

class AxisTicksConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AxisTicksConfigDialog(QWidget *parent = 0);
    ~AxisTicksConfigDialog();
    double interval();
    int subTicks();

public slots:
    void setInterval(double);
    void setSubTicks(int);

private:
    Ui::AxisTicksConfigDialog *ui;
};

#endif // AXISTICKSCONFIGDIALOG_H
