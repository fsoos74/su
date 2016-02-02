#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QString text=QString("<html><big><b>%1<\b><\big><br>Version %2<br>built %3 %4<br><i>(C)%5<\i><\html>").arg(
                PROGRAM, VERSION, __DATE__, __TIME__, COMPANY );
    ui->label->setText(text);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
