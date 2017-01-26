#ifndef EDITHORIZONSDIALOG_H
#define EDITHORIZONSDIALOG_H

#include <QDialog>

namespace Ui {
class EditHorizonsDialog;
}

class EditHorizonsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditHorizonsDialog(QWidget *parent = 0);
    ~EditHorizonsDialog();

private:
    Ui::EditHorizonsDialog *ui;
};

#endif // EDITHORIZONSDIALOG_H
