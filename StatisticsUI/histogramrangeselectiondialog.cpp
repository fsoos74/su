#include "histogramrangeselectiondialog.h"
#include "ui_histogramrangeselectiondialog.h"

#include <QDoubleValidator>
#include <QKeyEvent>
#include<QButtonGroup>
#include<iostream>
#include<iomanip>

HistogramRangeSelectionDialog::HistogramRangeSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistogramRangeSelectionDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leMinimum->setValidator(validator);
    ui->leMaximum->setValidator(validator);

    QButtonGroup* btGroup=new QButtonGroup(this);
    btGroup->addButton(ui->rbMinMax);
    btGroup->addButton(ui->rb3Sigma);
    btGroup->addButton(ui->rb6Sigma);
    btGroup->setExclusive(true);
    connect( btGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(updateRange()) );

    connect( ui->histogramWidget, SIGNAL(rangeChanged(std::pair<double,double>)),
             this, SLOT(setRange(std::pair<double,double>)) );                      // colortable via this

    connect( this, SIGNAL(rangeChanged(std::pair<double,double>)),
             ui->histogramWidget, SLOT(setRange(std::pair<double,double>)) );       // colortable via this

    connect( ui->histogramWidget, SIGNAL(powerChanged(double)),
             ui->dsPower, SLOT(setValue(double)) );

    connect( ui->dsPower, SIGNAL(valueChanged(double)),
             ui->histogramWidget, SLOT(setPower(double)) );

    connect( ui->cbLock, SIGNAL(toggled(bool)),
             this, SLOT(setLocked(bool)) );

    connect( ui->histogramWidget, SIGNAL(instantUpdatesChanged(bool)),
             this, SLOT(setInstantUpdates(bool)) );

    connect( ui->cbInstantUpdates, SIGNAL(toggled(bool)),
             ui->histogramWidget, SLOT(setInstantUpdates(bool)) );

}

HistogramRangeSelectionDialog::~HistogramRangeSelectionDialog()
{
    delete ui;
}


std::pair<double, double> HistogramRangeSelectionDialog::range(){

    return std::make_pair( ui->leMinimum->text().toDouble(), ui->leMaximum->text().toDouble());
}

bool HistogramRangeSelectionDialog::isLocked(){
    return ui->cbLock->isChecked();
}

bool HistogramRangeSelectionDialog::isInstantUpdates(){
    return ui->cbInstantUpdates->isChecked();
}

void HistogramRangeSelectionDialog::updateRange(){

    auto h=ui->histogramWidget->histogram();

    double rmin=0;
    double rmax=0;
    if( ui->rbMinMax->isChecked()){
        rmin=h.minimum();
        rmax=h.maximum();
    }
    else if( ui->rb3Sigma->isChecked()){
        rmin=h.mean()-3*h.sigma();
        rmax=h.mean()+3*h.sigma();
    }
    else if( ui->rb6Sigma->isChecked()){
        rmin=h.mean()-6*h.sigma();
        rmax=h.mean()+6*h.sigma();
    }

    setRange(std::make_pair(std::max(rmin,h.minimum()), std::min(rmax, h.maximum())));
}

void HistogramRangeSelectionDialog::setHistogram(const Histogram & h){

    ui->histogramWidget->setHistogram(h);
    /*
    std::cout<<"HistogramRangeSelectionDialog::setHistogram"<<std::endl;
    std::cout<<"min="<<h.minimum()<<std::endl;
    std::cout<<"max="<<h.maximum()<<std::endl;
    std::cout<<"mean="<<h.mean()<<std::endl;
    std::cout<<"variance="<<h.variance()<<std::endl;
    std::cout<<"sigma="<<h.sigma()<<std::endl;
    std::cout<<std::flush;
*/
    updateRange();
}

void HistogramRangeSelectionDialog::setColorTable(ColorTable * ct){
    ui->histogramWidget->setColorTable(ct);
    connect( ct, SIGNAL(lockedChanged(bool)), this, SLOT(setLocked(bool)) );
    connect( this, SIGNAL(lockChanged(bool)), ct, SLOT(setLocked(bool)) );
    connect( ct, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(setRange(std::pair<double,double>)) );
    connect( this, SIGNAL(rangeChanged(std::pair<double,double>)), ct, SLOT(setRange(std::pair<double,double>)) );

    setRange(ct->range());
    setLocked(ct->isLocked());
}

void HistogramRangeSelectionDialog::setRange(std::pair<double, double> r){

    if( r==range()) return;

    if( isLocked() ){
        return;
    }

    ui->leMinimum->setText(QString::number(r.first));
    ui->leMaximum->setText(QString::number(r.second));

    emit rangeChanged(r);
    emit rangeChanged(r.first, r.second);
}

void HistogramRangeSelectionDialog::setRange(double min, double max){
    setRange(std::make_pair(min, max));
}

void HistogramRangeSelectionDialog::setLocked(bool on){

//    std::cout<<"HistogramRangeSelectionDialog::setLocked "<<std::boolalpha<<on<<std::endl;
//    if( on == ui->cbLock->isChecked()) return;

    ui->cbLock->setChecked(on);
    ui->leMinimum->setEnabled(!on);
    ui->leMaximum->setEnabled(!on);
    ui->dsPower->setEnabled(!on);
    emit lockChanged(on);
}

void HistogramRangeSelectionDialog::setInstantUpdates(bool on){
    ui->cbInstantUpdates->setChecked(on);
}

void HistogramRangeSelectionDialog::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}

void HistogramRangeSelectionDialog::on_leMinimum_returnPressed()
{
    // range is already in controls, no set necessary, need to inform others only
    emit rangeChanged(range());
}

void HistogramRangeSelectionDialog::on_leMaximum_returnPressed()
{
    // range is already in controls, no set necessary, need to inform others only
    emit rangeChanged(range());
}

void HistogramRangeSelectionDialog::on_pbResetMinimum_clicked()
{
    setRange( std::make_pair( ui->histogramWidget->histogram().minimum(), range().second));
}

void HistogramRangeSelectionDialog::on_pbResetMaximum_clicked()
{
    setRange( std::make_pair( range().first, ui->histogramWidget->histogram().maximum() ) );
}

