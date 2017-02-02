#include "editslicesdialog.h"
#include "ui_editslicesdialog.h"

#include <QMessageBox>
#include <QInputDialog>
#include<iostream>

EditSlicesDialog::EditSlicesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditSlicesDialog)
{
    ui->setupUi(this);

    connect( ui->sbValue, SIGNAL(valueChanged(int)), this, SLOT(sliceControlsChanged()) );
}

EditSlicesDialog::~EditSlicesDialog()
{
    delete ui;
}


void EditSlicesDialog::setSliceModel(SliceModel * model){

    if( model == m_sliceModel ) return;

    m_sliceModel=model;

    ui->cbName->clear();

    if( !m_sliceModel )  return;

    connect( m_sliceModel, SIGNAL(changed()), this, SLOT(modelChanged()));

    foreach( QString name, m_sliceModel->names()){
        ui->cbName->addItem(name);
    }

    ui->cbName->setCurrentIndex(0);
}


void EditSlicesDialog::modelChanged(){

    if( !m_sliceModel ) return;

    QString name = ui->cbName->currentText();
    //SliceDef current=sliceFromControls();

    wait_controls=true;

    ui->cbName->clear();
    QStringList names;
    foreach( QString s, m_sliceModel->names()){
        names.append(s);
    }
    ui->cbName->addItems(names);

    wait_controls=false;

    // if exists make same slice current as before
    if( names.contains(name)){
        ui->cbName->setCurrentText(name);
    }
    else{
        ui->cbName->setCurrentIndex(0);
    }
}


void EditSlicesDialog::setBounds(Grid3DBounds bounds){

    m_bounds=bounds;
}

void EditSlicesDialog::setCurrentSlice(QString name){

    // this will trigger update of controls via currentIndexChanged
    ui->cbName->setCurrentText(name);
}


void EditSlicesDialog::sliceControlsChanged(){

    // update of controls in progress, dont update until all controls are set to the new values
    if( wait_controls) return;

    QString name=ui->cbName->currentText();
    SliceDef slice=sliceFromControls();

    m_sliceModel->setSlice(name, slice);
}

void EditSlicesDialog::sliceToControls( QString name, SliceDef slice ){


    // prevent signals of controls before all params are set
    wait_controls=true;

    // update value range according to slice type and volume bounds
    int min=0;
    int max=100;
    int inc=1;
    switch(slice.type){
    case SliceType::INLINE: min=m_bounds.inline1(); max=m_bounds.inline2(); inc=1; break;
    case SliceType::CROSSLINE: min=m_bounds.crossline1(); max=m_bounds.crossline2(); inc=1; break;
    case SliceType::TIME: min=static_cast<int>(1000*m_bounds.ft()); max=static_cast<int>(1000*m_bounds.lt());
                            inc=static_cast<int>(1000*m_bounds.dt());break;
    }   
    ui->sbValue->setRange(min, max);
    ui->sbValue->setSingleStep(inc);

    ui->leType->setText(toQString(slice.type));

    // adjust slice to volume bounds
    if( slice.value<min) slice.value=min;
    else if(slice.value>max) slice.value=max;

    ui->sbValue->setValue(slice.value);

    ui->cbName->setCurrentText(name); // XXX

    // value changes of controls can trigger signals again
    wait_controls=false;

    //std::cout<<"slice to controls: name="<<name.toStdString()<<" type="<<toQString(slice.type).toStdString()<<" value="<<slice.value<<std::endl;

}

SliceDef EditSlicesDialog::sliceFromControls(){

    SliceDef slice;

    slice.type = toSliceType(ui->leType->text());
    slice.value = ui->sbValue->value();

    return slice;
}



void EditSlicesDialog::on_pbAdd_clicked()
{
    QStringList types;
    types.append(toQString(SliceType::INLINE));
    types.append(toQString(SliceType::CROSSLINE));
    types.append(toQString(SliceType::TIME));
    bool ok=false;

    QString s = QInputDialog::getItem(this, tr("Add Slice"), tr("Select Type:"), types, 0, false, &ok );
    if( s.isNull() || !ok ) return;
    SliceType type = toSliceType(s);

    // find central value for slice
    int value=0;
    switch( type ){
    case SliceType::INLINE: value=(m_bounds.inline1() + m_bounds.inline2() ) / 2; break;
    case SliceType::CROSSLINE: value=(m_bounds.crossline1() + m_bounds.crossline2() ) / 2; break;
    case SliceType::TIME: value=static_cast<int>(1000 * (m_bounds.ft() + m_bounds.lt() ) / 2 ); break;
    }

    SliceDef def{ type, value };

    QString name=m_sliceModel->generateName();
    m_sliceModel->addSlice( name, def );

    ui->cbName->setCurrentText(name);

    // make it easier to change the value
    ui->sbValue->setFocus();
}

void EditSlicesDialog::on_pbDelete_clicked()
{
    QString current=ui->cbName->currentText();

    // find new current
    int newCurrent=0;
    foreach( QString s, m_sliceModel->names()){
        if( s!=current) break;
        newCurrent++;
    }

    // make newCurrent the current slice before removing the current to avoid ix up of controls
    // items in combobox are same order as names in model
    ui->cbName->setCurrentIndex(newCurrent);

    // finally remove it
    m_sliceModel->removeSlice(current);
}

void EditSlicesDialog::on_cbName_currentIndexChanged(const QString& name)
{

    if( ! m_sliceModel->contains(name)) return;

    SliceDef slice=m_sliceModel->slice(name);

    sliceToControls(name, slice);
}
