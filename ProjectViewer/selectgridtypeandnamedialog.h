#ifndef SELECTGRIDTYPEANDNAMEDIALOG_H
#define SELECTGRIDTYPEANDNAMEDIALOG_H

#include <QDialog>

#include<avoproject.h>


namespace Ui {
class SelectGridTypeAndNameDialog;
}

class SelectGridTypeAndNameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectGridTypeAndNameDialog(QWidget *parent = 0);
    ~SelectGridTypeAndNameDialog();

    QString gridName()const;
    GridType gridType()const;
    bool isDisplay()const;

private:
    Ui::SelectGridTypeAndNameDialog *ui;
};

#endif // SELECTGRIDTYPEANDNAMEDIALOG_H
