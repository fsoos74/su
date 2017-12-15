#include "selecttypeanditemdialog.h"
#include "ui_selecttypeanditemdialog.h"

SelectTypeAndItemDialog::SelectTypeAndItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectTypeAndItemDialog)
{
    ui->setupUi(this);
}

SelectTypeAndItemDialog::~SelectTypeAndItemDialog()
{
    delete ui;
}


QString SelectTypeAndItemDialog::selectedType(){
    return ui->cbType->currentText();
}

QString SelectTypeAndItemDialog::selectedItem(){
    return ui->cbItem->currentText();
}

void SelectTypeAndItemDialog::addType(QString name, QStringList items){

    if( m_typeItems.contains(name)) return;

    m_typeItems.insert(name, items);
}

void SelectTypeAndItemDialog::setTypeLabelText(QString text){
    ui->lbType->setText(text);
}

void SelectTypeAndItemDialog::setItemLabelText(QString text){
    ui->lbItem->setText(text);
}

void SelectTypeAndItemDialog::on_cbType_currentIndexChanged(const QString & type)
{
    ui->cbItem->clear();
    ui->cbItem->addItems(m_typeItems.value(type));
}
