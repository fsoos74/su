#ifndef MODALDISPLAYRANGEDIALOG_H
#define MODALDISPLAYRANGEDIALOG_H

#include <QDialog>

namespace Ui {
class ModalDisplayRangeDialog;
}

class ModalDisplayRangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModalDisplayRangeDialog(QWidget *parent = 0);
    ~ModalDisplayRangeDialog();

    qreal minimum()const;
    qreal maximum()const;
    qreal power()const;

public slots:

    void setMinimum(qreal);
    void setMaximum(qreal);
    void setPower(qreal);

private:
    Ui::ModalDisplayRangeDialog *ui;
};

#endif // MODALDISPLAYRANGEDIALOG_H
