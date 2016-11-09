#include "volumedisplayoptionsdialog.h"
#include "ui_volumedisplayoptionsdialog.h"

#include<QDoubleValidator>
#include<QKeyEvent>

#include<colortabledialog.h>


VolumeDisplayOptionsDialog::VolumeDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeDisplayOptionsDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leMin->setValidator(validator);
    ui->leMax->setValidator(validator);

    connect( ui->leMin, SIGNAL(returnPressed()), this, SLOT(applyRange()));
    connect( ui->leMax, SIGNAL(returnPressed()), this, SLOT(applyRange()));

    connect( ui->sbOpacity, SIGNAL(valueChanged(int)), this, SLOT(setOpacity(int)) );
}

VolumeDisplayOptionsDialog::~VolumeDisplayOptionsDialog()
{
    delete ui;
}

std::pair<double, double> VolumeDisplayOptionsDialog::range(){

 double min=ui->leMin->text().toDouble();
 double max=ui->leMax->text().toDouble();
 return std::pair<double, double>(min, max);
}

int VolumeDisplayOptionsDialog::opacity(){
    return ui->sbOpacity->value();
}

void VolumeDisplayOptionsDialog::setEditColorTableAction(QAction* action){

    m_editColorTableAction=action;
}

void VolumeDisplayOptionsDialog::setRange(std::pair<double, double> r){

 if( r==range() ) return;

 ui->leMin->setText(QString::number(r.first));
 ui->leMax->setText(QString::number(r.second));

 emit rangeChanged(r);
}

void VolumeDisplayOptionsDialog::setOpacity(int o){

    // no check if value is equal, this is done be spinbox
    ui->sbOpacity->setValue(o);

    emit opacityChanged(o);
}

void VolumeDisplayOptionsDialog::applyRange(){

 emit rangeChanged( range());
}

void VolumeDisplayOptionsDialog::keyPressEvent(QKeyEvent *ev){
 if( ev->key() == Qt::Key_Return){
     ev->accept();
 }
 else{
     QWidget::keyPressEvent(ev);
 }

}

void VolumeDisplayOptionsDialog::on_pbColorTable_clicked()
{
    if(m_editColorTableAction){
        m_editColorTableAction->trigger();
    }
}
