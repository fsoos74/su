#include "isolinedialog.h"
#include "ui_isolinedialog.h"


#include<QIntValidator>
#include<QDoubleValidator>
#include<QColorDialog>
#include<QKeyEvent>
#include<iostream>

IsoLineDialog::IsoLineDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::IsoLineDialog)
{
    ui->setupUi(this);

    QIntValidator* ivalid=new QIntValidator(1, MAX_STEPS, this);
    ui->leSteps->setValidator(ivalid);

    QDoubleValidator* dvalid=new QDoubleValidator(this);
    ui->leFirst->setValidator(dvalid);
    ui->leLast->setValidator(dvalid);

    connect( ui->rbNoContours, SIGNAL(toggled(bool)), this, SLOT(computeContours()));
    connect( ui->rbSameAsColorBar, SIGNAL(toggled(bool)), this, SLOT(computeContours()));
    connect( ui->rbUserDefined, SIGNAL(toggled(bool)), this, SLOT(computeContours()));
    connect( ui->leFirst, SIGNAL(returnPressed()), this, SLOT(computeContours()));
    connect( ui->leLast, SIGNAL(returnPressed()), this, SLOT(computeContours()));
    connect( ui->leSteps, SIGNAL(returnPressed()), this, SLOT(computeContours()));

}

IsoLineDialog::~IsoLineDialog()
{
    delete ui;
}

int IsoLineDialog::lineWidth()const{
    return ui->sbLineWidth->value();
}

const QColor& IsoLineDialog::lineColor()const{
    return ui->cbLineColor->color();
}

QVector<double> IsoLineDialog::contours(){
    return m_contours;
}

double IsoLineDialog::first()const{
    return ui->leFirst->text().toDouble();
}

double IsoLineDialog::last()const{
    return ui->leLast->text().toDouble();
}

double IsoLineDialog::steps()const{
    return ui->leSteps->text().toInt();
}


IsoLineDialog::ContourMode IsoLineDialog::contourMode()const{

    if( ui->rbNoContours->isChecked()){
        return MODE_NO_CONTOURS;
    }
    else if( ui->rbSameAsColorBar->isChecked()){
        return MODE_DEFAULT;
    }
    else if( ui->rbUserDefined->isChecked()){
        return MODE_USER_DEFINED;
    }
    else{
        qFatal("It can never happen that no radiobuttion is checked!!!");
    }
}


void IsoLineDialog::setContourMode(IsoLineDialog::ContourMode m){

    if( m==contourMode()) return;

    mode2Buttons( m );

    emit contourModeChanged(m);

    computeContours();

    emit contoursChanged(m_contours);
}

void IsoLineDialog::setFirst( double f ){

    if(f==first())return;

    ui->leFirst->setText(QString::number(f));

    emit firstChanged(f);


}


void IsoLineDialog::setLast( double l ){

    if( l==last() )return;

    ui->leLast->setText(QString::number(l));

    emit lastChanged(l);

    computeContours();

    emit contoursChanged(m_contours);
}


void IsoLineDialog::setSteps( int n ){

    if(n==steps())return;

    ui->leSteps->setText(QString::number(n));

    emit stepsChanged(n);

    computeContours();

    emit contoursChanged(m_contours);
}


void IsoLineDialog::setUserDefinedParams(double first, double last, int steps){

    setFirst(first);
    setLast(last);
    setSteps(steps);
}


void IsoLineDialog::setColorBarSteps( const QVector<double>& cbs){

    colorBarSteps=cbs;

    // emit new steps if used
    if( contourMode()==MODE_DEFAULT ){
        computeContours();
    }
}


void IsoLineDialog::setLineColor( const QColor& col){

    if( col == lineColor()) return;

    ui->cbLineColor->setColor(col);

    emit lineColorChanged( col );

}

void IsoLineDialog::setLineWidth(int w){

    if(w==lineWidth()) return;

    ui->sbLineWidth->setValue(w);

    emit lineWidthChanged(w);
}

void IsoLineDialog::on_cbLineColor_clicked()
{
    const QColor color = QColorDialog::getColor( lineColor(), this, "Select Isoline Color");


    if (color.isValid()) {

      setLineColor(color);

    }
}

void IsoLineDialog::on_sbLineWidth_valueChanged(int)
{
    emit lineWidthChanged(ui->sbLineWidth->value());
}

void IsoLineDialog::computeContours(){

    QVector<double> cont;

    if( ui->rbNoContours->isChecked() ){
        cont=QVector<double>();
    }
    else if(ui->rbSameAsColorBar->isChecked()){

        cont=colorBarSteps;
    }
    else if(ui->rbUserDefined->isChecked()){

        double f=first();
        double l=last();
        int s=steps();
        cont.reserve( s );
        cont.push_back(f);
        for( int i=1; i<s; i++ ){
            double x=f + i*(l-f)/(s-1);
            cont.push_back(x);
        }
    }
    else{
        qFatal("Radio button confusion!!!");
    }

    m_contours = cont;

    emit contoursChanged(m_contours);
}


void IsoLineDialog::mode2Buttons( IsoLineDialog::ContourMode m){

    switch( m ){
    case MODE_NO_CONTOURS: ui->rbNoContours->setChecked(true);
        break;
    case MODE_DEFAULT: ui->rbSameAsColorBar->setChecked(true);
        break;
    case MODE_USER_DEFINED:ui->rbUserDefined->setChecked(true);
        break;
    default:
        qFatal("Illegal mode for IsoLineDialog!");
        break;
    }
}

void IsoLineDialog::on_pbClose_clicked()
{
    hide();
}

void IsoLineDialog::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}
