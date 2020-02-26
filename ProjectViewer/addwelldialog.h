#ifndef ADDWELLDIALOG_H
#define ADDWELLDIALOG_H

#include <QDialog>
#include<wellinfo.h>


namespace Ui {
class AddWellDialog;
}

class AddWellDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddWellDialog(QWidget *parent = nullptr);
    ~AddWellDialog();
    WellInfo wellInfo()const;
    void setWellInfo(const WellInfo&);

private:
    Ui::AddWellDialog *ui;
};

#endif // ADDWELLDIALOG_H
