#ifndef EDITTRENDDIALOG_H
#define EDITTRENDDIALOG_H

#include <QDialog>

namespace Ui {
class EditTrendDialog;
}

class EditTrendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditTrendDialog(QWidget *parent = 0);
    ~EditTrendDialog();

    double angle();
    double intercept();

public slots:
    void setAngle(double);
    void setIntercept(double);

private:
    Ui::EditTrendDialog *ui;
};

#endif // EDITTRENDDIALOG_H
