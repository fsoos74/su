#include "sectionscaledialog.h"
#include "ui_sectionscaledialog.h"

#include<QDoubleValidator>
#include<QKeyEvent>

const double MM_PER_INCH=25.4;

SectionScaleDialog::SectionScaleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SectionScaleDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    validator->setBottom(0);

    ui->leMMPerTrace->setValidator(validator);
    ui->leMMPerSecond->setValidator(validator);

    connect( ui->leMMPerTrace, SIGNAL(returnPressed()), this, SLOT(apply()) );
    connect( ui->leMMPerSecond, SIGNAL(returnPressed()), this, SLOT(apply()) );
}

SectionScaleDialog::~SectionScaleDialog()
{
    delete ui;
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



void SectionScaleDialog::pixelPerTraceFromControl(){

    qreal mmpt=ui->leMMPerTrace->text().toDouble();
    qreal ppt=mmpt*m_dpiX/MM_PER_INCH;
    setPixelPerTrace(ppt);
}

void SectionScaleDialog::pixelPerTraceToControl(){

    qreal mmpt= MM_PER_INCH * m_pixelPerTrace/m_dpiX;
    ui->leMMPerTrace->setText(QString::number(mmpt));
}

void SectionScaleDialog::pixelPerSecondFromControl(){

    qreal mmps=ui->leMMPerSecond->text().toDouble();
    qreal pps=mmps*m_dpiY/MM_PER_INCH;
    setPixelPerSecond(pps);
}

void SectionScaleDialog::pixelPerSecondToControl(){

    qreal mmps= MM_PER_INCH * m_pixelPerSecond/m_dpiY;
    ui->leMMPerSecond->setText(QString::number(mmps));
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
