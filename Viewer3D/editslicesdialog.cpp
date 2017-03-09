#include "editslicesdialog.h"
#include "ui_editslicesdialog.h"

#include <QMessageBox>
#include <QInputDialog>
#include<iostream>

std::ostream& operator<<(std::ostream& os, const SliceDef& def ){

    os<<"[ volume="<<def.volume.toStdString()<<" type="<<toQString(def.type).toStdString()<<" value="<<def.value<<" ]";
    return os;
}

EditSlicesDialog::EditSlicesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditSlicesDialog)
{
    ui->setupUi(this);

    connect( ui->cbName , SIGNAL(currentIndexChanged(QString)), this, SLOT(sliceControlsChanged()) );
    connect( ui->sbValue, SIGNAL(valueChanged(int)), this, SLOT(sliceControlsChanged()) );
    connect( ui->cbVolume, SIGNAL(currentIndexChanged(int)), this, SLOT(sliceControlsChanged()) );
}

EditSlicesDialog::~EditSlicesDialog()
{
    delete ui;
}


void EditSlicesDialog::setDimensions(VolumeDimensions dims){

    if( dims==m_dimensions ) return;

    m_dimensions=dims;


}

void EditSlicesDialog::setSliceModel(SliceModel * model){

    if( model == m_sliceModel ) return;
/*
    std::cout<<"MODEL:"<<std::endl;
    foreach( QString name, model->names()){
        std::cout<<name.toStdString()<<" : "<<model->slice(name)<<std::endl<<std::flush;
    }
*/
    wait_controls=true;

    m_sliceModel=model;

    ui->cbName->clear();

    if( !m_sliceModel )  return;

    connect( m_sliceModel, SIGNAL(changed()), this, SLOT(modelChanged()));

    foreach( QString name, m_sliceModel->names()){
        ui->cbName->addItem(name);
    }

    wait_controls=false;

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
    if( !name.isEmpty() && names.contains(name)){
        ui->cbName->setCurrentText(name);
    }
    else{
        ui->cbName->setCurrentIndex(0);
    }
}


void EditSlicesDialog::setVolumes(QStringList l){

    QString cur=ui->cbVolume->currentText();

    ui->cbVolume->clear();

    ui->cbVolume->addItems(l);

    if( l.contains(cur)){
        ui->cbVolume->setCurrentText(cur);
    }
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

    ui->cbVolume->setCurrentText(slice.volume);

    // adjust bounds to current volume bounds
    // update value range according to slice type and volume bounds
    int min=0;
    int max=100;
    int inc=1;
    switch(slice.type){
    case SliceType::INLINE: min=m_dimensions.inline1; max=m_dimensions.inline2; inc=1; break;
    case SliceType::CROSSLINE: min=m_dimensions.crossline1; max=m_dimensions.crossline2; inc=1; break;
    case SliceType::TIME: min=m_dimensions.msec1; max=m_dimensions.msec2; inc=1;break;              // need a clever way to make this settable
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

}

SliceDef EditSlicesDialog::sliceFromControls(){

    SliceDef slice{"", SliceType::INLINE, 0};

    if( ui->leType->text().isEmpty()) return slice;

    slice.type = toSliceType(ui->leType->text());
    slice.value = ui->sbValue->value();
    slice.volume = ui->cbVolume->currentText();

    return slice;
}



void EditSlicesDialog::on_pbAdd_clicked()
{
    QStringList types;
    types.append(toQString(SliceType::INLINE));
    types.append(toQString(SliceType::CROSSLINE));
    types.append(toQString(SliceType::TIME));
    bool ok=false;

    QString volume=ui->cbVolume->currentText();

    QString s = QInputDialog::getItem(this, tr("Add Slice"), tr("Select Type:"), types, 0, false, &ok );

    if( s.isNull() || !ok ) return;
    SliceType type = toSliceType(s);

    // find central value for slice
    int value=0;
    switch( type ){
    case SliceType::INLINE: value=(m_dimensions.inline1 + m_dimensions.inline2 ) / 2; break;
    case SliceType::CROSSLINE: value=(m_dimensions.crossline1 + m_dimensions.crossline2 ) / 2; break;
    case SliceType::TIME: value= (m_dimensions.msec1+m_dimensions.msec2 ) / 2; break;
    }

    SliceDef slice{ volume, type, value };

    QString name=m_sliceModel->generateName();

    wait_controls=true;

    m_sliceModel->addSlice( name, slice );

    wait_controls=false;

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
