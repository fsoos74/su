#include "selectgridtypeandnamedialog.h"
#include "ui_selectgridtypeandnamedialog.h"

SelectGridTypeAndNameDialog::SelectGridTypeAndNameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectGridTypeAndNameDialog)
{
    ui->setupUi(this);

    ui->cbGridType->addItem("Horizon");
    ui->cbGridType->addItem("Other Grid");
}

SelectGridTypeAndNameDialog::~SelectGridTypeAndNameDialog()
{
    delete ui;
}

QString SelectGridTypeAndNameDialog::gridName()const{

    return ui->leGridName->text();
}


GridType SelectGridTypeAndNameDialog::gridType()const{

    switch( ui->cbGridType->currentIndex()){

    case 0: return GridType::Horizon; break;
    case 1: return GridType::Other; break;
    default:
        qFatal("Invalied grid type index!");
        break;
    }
}

bool SelectGridTypeAndNameDialog::isDisplay()const{

    return ui->cbDisplay->isChecked();
}

