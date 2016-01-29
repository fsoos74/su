#include "twocombosdialog.h"
#include "ui_twocombosdialog.h"

TwoCombosDialog::TwoCombosDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TwoCombosDialog)
{
    ui->setupUi(this);
}

TwoCombosDialog::~TwoCombosDialog()
{
    delete ui;
}


QString TwoCombosDialog::selection1()const{

    return ui->comboBox1->currentText();
}

QString TwoCombosDialog::selection2()const{

    return ui->comboBox2->currentText();
}

void TwoCombosDialog::setLabelText1(const QString& str){

    ui->label1->setText(str);
}

void TwoCombosDialog::setLabelText2(const QString& str){

    ui->label2->setText(str);
}

void TwoCombosDialog::setItems1( const QStringList& l){

    ui->comboBox1->clear();
    ui->comboBox1->addItems(l);
}

void TwoCombosDialog::setItems2( const QStringList& l){

    ui->comboBox2->clear();
    ui->comboBox2->addItems(l);
}
