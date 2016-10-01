#include "scaledialog.h"
#include "ui_scaledialog.h"

#include<QIntValidator>
#include<QKeyEvent>


ScaleDialog::ScaleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScaleDialog)
{
    ui->setupUi(this);

    QIntValidator* validator=new QIntValidator(this);
    validator->setBottom(1);
    ui->leHIncrement->setValidator(validator);
    ui->leVIncrement->setValidator(validator);
}

ScaleDialog::~ScaleDialog()
{
    delete ui;
}


bool ScaleDialog::isHorizontalFixed(){
    return ui->cbHFixed->isChecked();
}

bool ScaleDialog::isVerticalFixed(){
    return ui->cbVFixed->isChecked();
}

int ScaleDialog::horizontalIncrement(){
    return ui->leHIncrement->text().toInt();
}

int ScaleDialog::verticalIncrement(){
    return ui->leVIncrement->text().toInt();
}

void ScaleDialog::setHorizontalFixed(bool on){
    ui->cbHFixed->setChecked(on);   // cb toggled will emit signal
}

void ScaleDialog::setVerticalFixed(bool on){
    ui->cbVFixed->setChecked(on);   // cb toggled will emit signal
}

void ScaleDialog::setHorizontalIncrement(int i){
    if( i==horizontalIncrement()) return;

    ui->leHIncrement->setText( QString::number(i) );

    emit horizontalIncrementChanged( i );
}

void ScaleDialog::setVerticalIncrement(int i){
    if( i==verticalIncrement()) return;

    ui->leVIncrement->setText( QString::number(i) );

    emit verticalIncrementChanged( i );
}


void ScaleDialog::on_leHIncrement_returnPressed()
{
    emit horizontalIncrementChanged( horizontalIncrement() );
}

void ScaleDialog::on_leVIncrement_returnPressed()
{
    emit verticalIncrementChanged( verticalIncrement() );
}

void ScaleDialog::on_cbHFixed_toggled(bool checked)
{
    emit horizontalFixedChanged(checked);
}

void ScaleDialog::on_cbVFixed_toggled(bool checked)
{
    emit verticalFixedChanged(checked);
}


void ScaleDialog::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}
