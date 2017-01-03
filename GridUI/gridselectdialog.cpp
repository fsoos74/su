#include "gridselectdialog.h"
#include "ui_gridselectdialog.h"

#include<QPushButton>

GridSelectDialog::GridSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridSelectDialog)
{
    ui->setupUi(this);


    connect( ui->cbType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNames()));

    connect( ui->cbName, SIGNAL(currentIndexChanged(int)), this, SLOT(updateOkButton()) );
}

GridSelectDialog::~GridSelectDialog()
{
    delete ui;
}


void GridSelectDialog::setProject(const AVOProject &project){

    m_horizonGrids=project.gridList(GridType::Horizon);
    m_attributeGrids=project.gridList(GridType::Attribute);
    m_otherGrids=project.gridList(GridType::Other);

    updateNames();
}

GridType GridSelectDialog::type(){

    switch( ui->cbType->currentIndex() ){

    case 0: return GridType::Horizon; break;
    case 1: return GridType::Attribute; break;
    case 2: return GridType::Other; break;

    default: qFatal("Unexpected grid type!!!");
    }

}

QString GridSelectDialog::name(){

    return ui->cbName->currentText();
}

void GridSelectDialog::updateNames(){

    ui->cbName->clear();

    switch( type() ){

        case GridType::Horizon: ui->cbName->addItems(m_horizonGrids); break;
        case GridType::Attribute: ui->cbName->addItems(m_attributeGrids); break;
        case GridType::Other: ui->cbName->addItems(m_otherGrids); break;
    }


    updateOkButton();

}


void GridSelectDialog::updateOkButton(){

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!name().isEmpty());
}
