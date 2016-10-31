#include "sectionscaledialog.h"
#include "ui_sectionscaledialog.h"

#include<QDoubleValidator>
#include<QKeyEvent>

const double CM_PER_INCH=2.54;

const int INDEX_CM=0;
const int INDEX_INCH=1;

SectionScaleDialog::SectionScaleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SectionScaleDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    validator->setBottom(0);

    ui->leTracesPerUnit->setValidator(validator);
    ui->leUnitsPerSecond->setValidator(validator);

    connect( ui->leTracesPerUnit, SIGNAL(returnPressed()), this, SLOT(apply()) );
    connect( ui->leUnitsPerSecond, SIGNAL(returnPressed()), this, SLOT(apply()) );
    connect( ui->cbLock, SIGNAL(toggled(bool)), this, SIGNAL(lockedChanged(bool)) );
}

SectionScaleDialog::~SectionScaleDialog()
{
    delete ui;
}

bool SectionScaleDialog::isLocked(){
    return ui->cbLock->isChecked();
}

void SectionScaleDialog::setDPIX(qreal x){

    if( x==m_dpiX) return;

    m_dpiX=x;

    emit dpiXChanged(x);
}

void SectionScaleDialog::setDPIY(qreal y){

    if( y==m_dpiY) return;

    m_dpiY=y;

    emit dpiYChanged(y);
}

void SectionScaleDialog::setPixelPerTrace(qreal ppt){

    if( ppt==pixelPerTrace()) return;
    m_pixelPerTrace=ppt;
    pixelPerTraceToControl();
    emit pixelPerTraceChanged(pixelPerTrace());
}


void SectionScaleDialog::setPixelPerSecond(qreal pps){

    if( pps==pixelPerSecond()) return;
    m_pixelPerSecond=pps;
    pixelPerSecondToControl();
    emit pixelPerSecondChanged(pixelPerSecond());
}


void SectionScaleDialog::setUnit(Unit u){

    if(u==m_unit) return;
    m_unit=u;
    unitToControl();
    emit unitChanged(u);
}


void SectionScaleDialog::setLocked(bool on){

    ui->cbLock->setChecked(on); // checkbox does statur checking and signal
}

void SectionScaleDialog::pixelPerTraceFromControl(){

    qreal tpu=ui->leTracesPerUnit->text().toDouble();
    qreal ppt=m_dpiX/tpu;
    if( ui->cbUnit->currentIndex()==INDEX_CM){
        ppt/=CM_PER_INCH;
    }
    setPixelPerTrace(ppt);
}

void SectionScaleDialog::pixelPerTraceToControl(){

    qreal tpu= m_dpiX / m_pixelPerTrace;
    if( ui->cbUnit->currentIndex()==INDEX_CM){
        tpu/=CM_PER_INCH;
    }

    ui->leTracesPerUnit->setText(QString::number(tpu));
}

void SectionScaleDialog::pixelPerSecondFromControl(){

    qreal mmps=ui->leUnitsPerSecond->text().toDouble();
    qreal ppu=mmps*m_dpiY;
    if( ui->cbUnit->currentIndex()==INDEX_CM){
        ppu/=CM_PER_INCH;
    }

    setPixelPerSecond(ppu);
}

void SectionScaleDialog::pixelPerSecondToControl(){

    qreal ups=m_pixelPerSecond/m_dpiY;
    if( ui->cbUnit->currentIndex()==INDEX_CM){
        ups*=CM_PER_INCH;
    }
    ui->leUnitsPerSecond->setText(QString::number(ups));
}


void SectionScaleDialog::unitFromControl(){

    switch(ui->cbUnit->currentIndex()){
    case 0: m_unit=Unit::CENTIMETER;break;
    case 1: m_unit=Unit::INCH; break;
    default: qFatal("Unhandled unit");break;
    }
}


void SectionScaleDialog::unitToControl(){
    int idx=0;
    switch( m_unit ){
    case Unit::CENTIMETER: idx=0; break;
    case Unit::INCH: idx=1; break;
    }

    ui->cbUnit->setCurrentIndex(idx);
}

void SectionScaleDialog::apply(){
    pixelPerTraceFromControl();
    pixelPerSecondFromControl();
}


void SectionScaleDialog::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}

void SectionScaleDialog::on_cbUnit_currentIndexChanged(int index)
{
    pixelPerSecondToControl();
    pixelPerTraceToControl();
}
