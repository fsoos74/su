#ifndef SIMPLESCALINGDIALOG_H
#define SIMPLESCALINGDIALOG_H

#include <QDialog>

namespace sliceviewer {

namespace Ui {
class SimpleScalingDialog;
}

class SimpleScalingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimpleScalingDialog(QWidget *parent = 0);
    ~SimpleScalingDialog();
    double minimum();
    double maximum();
    double power();

public slots:
    void setMinimum(double);
    void setMaximum(double);
    void setPower(double);

private:
    Ui::SimpleScalingDialog *ui;
};

}

#endif // SIMPLESCALINGDIALOG_H
