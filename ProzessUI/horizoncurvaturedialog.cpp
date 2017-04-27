#include "horizoncurvaturedialog.h"
#include "ui_horizoncurvaturedialog.h"

HorizonCurvatureDialog::HorizonCurvatureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HorizonCurvatureDialog)
{
    ui->setupUi(this);
}

HorizonCurvatureDialog::~HorizonCurvatureDialog()
{
    delete ui;
}
