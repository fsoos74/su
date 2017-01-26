#include "edithorizonsdialog.h"
#include "ui_edithorizonsdialog.h"

EditHorizonsDialog::EditHorizonsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditHorizonsDialog)
{
    ui->setupUi(this);
}

EditHorizonsDialog::~EditHorizonsDialog()
{
    delete ui;
}
