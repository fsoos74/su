#include "editslicesdialog.h"
#include "ui_editslicesdialog.h"

#include<iostream>

EditSlicesDialog::EditSlicesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditSlicesDialog)
{
    ui->setupUi(this);

    ui->cbType->addItem(toQString(SliceType::INLINE));
    ui->cbType->addItem(toQString(SliceType::CROSSLINE));
    ui->cbType->addItem(toQString(SliceType::TIME));

    connect( ui->cbType, SIGNAL(currentIndexChanged(int)), this, SLOT(sliceControlsChanged()) );
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
    SliceDef current=sliceFromControls(name);

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
    SliceDef slice=sliceFromControls(name);

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

    ui->cbType->setCurrentText(toQString(slice.type));

    // adjust slice to volume bounds
    if( slice.value<min) slice.value=min;
    else if(slice.value>max) slice.value=max;

    ui->sbValue->setValue(slice.value);

    // value changes of controls can trigger signals again
    wait_controls=false;

    std::cout<<"slice to controls: name="<<name.toStdString()<<" type="<<toQString(slice.type).toStdString()<<" value="<<slice.value<<std::endl;

}

SliceDef EditSlicesDialog::sliceFromControls( QString name){

    SliceDef slice;

    slice.type = toSliceType(ui->cbType->currentText());
    slice.value = ui->sbValue->value();

    return slice;
}



void EditSlicesDialog::on_pbAdd_clicked()
{
    static int nextId=0;
    QString name=QString("Slice %1").arg(++nextId);

    // duplicate current slice
    SliceDef def;
    def.type=toSliceType(ui->cbType->currentText());
    def.value = ui->sbValue->value();

    m_sliceModel->addSlice(name, def);

    // make new slice the current
    ui->cbName->setCurrentText(name);
}

void EditSlicesDialog::on_pbDelete_clicked()
{
    QString name=ui->cbName->currentText();
    m_sliceModel->removeSlice(name);
}

void EditSlicesDialog::on_cbName_currentIndexChanged(const QString& name)
{

    if(wait_controls) return;

    Q_ASSERT( m_sliceModel->contains(name));

    SliceDef slice=m_sliceModel->slice(name);

    sliceToControls(name, slice);
}
