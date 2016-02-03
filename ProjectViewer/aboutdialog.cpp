#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QString text=QString( "<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">AVO-Detect</span></p>"
                          "<p><span style=\" font-size:12pt;\">Version %1</span></p>"
            "<p><span style=\" font-size:12pt;\">Built %2 %3</span></p>"
            "<p><span style=\" font-size:12pt;\">support@seismicutensils.com</span></p>"
            "<p><span style=\" font-style:italic;\">(C) Seismic Utensils, LLC</span></p>"
            "<p><br/></p></body></html>").arg(VERSION, __DATE__, __TIME__);


    ui->label->setText(text);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
