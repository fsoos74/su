#include "volumedisplayoptionsdialog.h"
#include "ui_volumedisplayoptionsdialog.h"

#include<QKeyEvent>
#include<QAction>

#include<colortabledialog.h>


VolumeDisplayOptionsDialog::VolumeDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeDisplayOptionsDialog)
{
    ui->setupUi(this);

    connect( ui->sbOpacity, SIGNAL(valueChanged(int)), this, SLOT(setOpacity(int)) );
}

VolumeDisplayOptionsDialog::~VolumeDisplayOptionsDialog()
{
    delete ui;
}

int VolumeDisplayOptionsDialog::opacity(){
    return ui->sbOpacity->value();
}

void VolumeDisplayOptionsDialog::setEditColorTableAction(QAction* action){

    m_editColorTableAction=action;
}

void VolumeDisplayOptionsDialog::setVolumeDisplayRangeAction(QAction* action){

    m_volumeDisplayRangeAction=action;
}


void VolumeDisplayOptionsDialog::setOpacity(int o){

    // no check if value is equal, this is done be spinbox
    ui->sbOpacity->setValue(o);

    emit opacityChanged(o);
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

void VolumeDisplayOptionsDialog::on_pbDisplayRange_clicked()
{
    if( m_volumeDisplayRangeAction){
        m_volumeDisplayRangeAction->trigger();
    }
}
