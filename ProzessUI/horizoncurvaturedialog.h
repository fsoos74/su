#ifndef HORIZONCURVATUREDIALOG_H
#define HORIZONCURVATUREDIALOG_H

#include <QDialog>

namespace Ui {
class HorizonCurvatureDialog;
}

class HorizonCurvatureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HorizonCurvatureDialog(QWidget *parent = 0);
    ~HorizonCurvatureDialog();

private:
    Ui::HorizonCurvatureDialog *ui;
};

#endif // HORIZONCURVATUREDIALOG_H
