#include "histogramrangeselectiondialog.h"
#include "ui_histogramrangeselectiondialog.h"

#include <QDoubleValidator>
#include <QKeyEvent>

HistogramRangeSelectionDialog::HistogramRangeSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistogramRangeSelectionDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leMinimum->setValidator(validator);
    ui->leMaximum->setValidator(validator);

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

void HistogramRangeSelectionDialog::setHistogram(const Histogram & h){
    ui->histogramWidget->setHistogram(h);
}

void HistogramRangeSelectionDialog::setColorTable(ColorTable * ct){
    ui->histogramWidget->setColorTable(ct);
    connect( ct, SIGNAL(lockedChanged(bool)), this, SLOT(setLocked(bool)) );
    connect( this, SIGNAL(lockChanged(bool)), ct, SLOT(setLocked(bool)) );
    setDefaultRange(ct->range());
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
}

void HistogramRangeSelectionDialog::setDefaultRange(std::pair<double, double> r){

    m_defaultRange = r;

    setRange(r);
}

void HistogramRangeSelectionDialog::setLocked(bool on){

    ui->cbLock->setChecked(on);
    ui->leMinimum->setEnabled(!on);
    ui->leMaximum->setEnabled(!on);
    ui->dsPower->setEnabled(!on);
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
    setRange( std::make_pair( m_defaultRange.first, range().second));
}

void HistogramRangeSelectionDialog::on_pbResetMaximum_clicked()
{
    setRange( std::make_pair( range().first, m_defaultRange.second ) );
}
