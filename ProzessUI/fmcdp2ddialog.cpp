#include "fmcdp2ddialog.h"
#include "ui_fmcdp2ddialog.h"

#include <QPushButton>
#include <QIntValidator>
#include <QDoubleValidator>

#include<iostream>

FMCDP2DDialog::FMCDP2DDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::FMCDP2DDialog)
{
    ui->setupUi(this);

    QIntValidator* ivalid=new QIntValidator(this);
    ivalid->setBottom(1);

    QDoubleValidator* fvalid=new QDoubleValidator(this);
    fvalid->setBottom(0.);
    ui->leF0->setValidator(fvalid);
    ui->leDelay->setValidator(fvalid);
    ui->leOffset1->setValidator(fvalid);
    ui->leOffset2->setValidator(fvalid);
    ui->leDXZ->setValidator(fvalid);
    ui->leTMax->setValidator(fvalid);
    connect( ui->leOffset1, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOffset2, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->twLayers, SIGNAL(cellChanged(int,int)), this, SLOT(updateOkButton()) );
    ui->twLayers->setColumnCount(2);
    QStringList labels;
    labels<<"Bottom [m]"<<"VP";
    ui->twLayers->setHorizontalHeaderLabels(labels);
    ui->twLayers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    setLayerCount(1);

    connect( ui->sbLayers, SIGNAL(valueChanged(int)), this, SLOT(setLayerCount(int)) );

    updateOkButton();
}

FMCDP2DDialog::~FMCDP2DDialog()
{
    delete ui;
}

QString FMCDP2DDialog::wavelet(){
    return ui->cbWavelet->currentText();
}

double FMCDP2DDialog::f0(){
    return ui->leF0->text().toDouble();
}

double FMCDP2DDialog::delay(){
    return ui->leDelay->text().toDouble();
}

double FMCDP2DDialog::offset1(){
    return ui->leOffset1->text().toDouble();
}

double FMCDP2DDialog::offset2(){
    return ui->leOffset2->text().toDouble();
}

bool FMCDP2DDialog::isSplitSpread(){
    return ui->cbSplitSpread->isChecked();
}

double FMCDP2DDialog::dxz(){
    return ui->leDXZ->text().toDouble();
}

double FMCDP2DDialog::tmax(){
    return ui->leTMax->text().toDouble();
}

unsigned FMCDP2DDialog::nrc(){
    return ui->sbTraces->value();
}

QString FMCDP2DDialog::dataset(){
    return ui->leOutput->text();
}

QList<std::pair<double,double>> FMCDP2DDialog::layers(){

    QList<std::pair<double, double>> list;

    for( int row = 0; row<ui->twLayers->rowCount(); row++){
        auto ditem=ui->twLayers->item(row,0);
        if( !ditem) continue;
        auto dep=ditem->text().toDouble();
        auto vitem=ui->twLayers->item(row,1);
        if( !vitem) continue;
        auto vel=vitem->text().toDouble();
        list.push_back(std::make_pair(dep, vel));
    }

    return list;
}

QMap<QString, QString> FMCDP2DDialog::params(){

    QMap<QString, QString> p;

    p.insert( "output", dataset());
    p.insert( "wavelet", wavelet() );
    p.insert( "f0", QString::number(f0()));
    p.insert( "delay", QString::number(delay()));
    p.insert( "dxz", QString::number(dxz()));
    p.insert( "tmax", QString::number(tmax()));
    p.insert( "offset1", QString::number(offset1()));
    p.insert( "offset2", QString::number(offset2()));
    p.insert( "nrc", QString::number(nrc()));
    p.insert( "split-spread", QString::number(static_cast<int>(isSplitSpread())));
    QStringList l;
    auto depvels = layers();
    for( auto depvel : depvels){
        l.append(QString("%1 : %2").arg(QString::number(depvel.first), QString::number(depvel.second)));
    }
    auto model=l.join(", ");
    p.insert( "model", model);

    return p;
}

void FMCDP2DDialog::setProject(AVOProject *project){

    if( project==m_project) return;

    m_project=project;
}

void FMCDP2DDialog::setWavelet(QString s){
    ui->cbWavelet->setCurrentText(s);
}

void FMCDP2DDialog::setDelay(double f){
    ui->leDelay->setText(QString::number(f));
}

void FMCDP2DDialog::setF0(double f){
    ui->leF0->setText(QString::number(f));
}

void FMCDP2DDialog::setOffset1(double d){
    ui->leOffset1->setText(QString::number(d));
}

void FMCDP2DDialog::setOffset2(double d){
    ui->leOffset2->setText(QString::number(d));
}

void FMCDP2DDialog::setSplitSpread(bool on){
    ui->cbSplitSpread->setChecked(on);
}

void FMCDP2DDialog::setNRC(unsigned n){
    ui->sbTraces->setValue(n);
}

void FMCDP2DDialog::setDXZ(double d){
    ui->leDXZ->setText(QString::number(d));
}

void FMCDP2DDialog::setTMax(double d){
    ui->leTMax->setText(QString::number(d));
}

void FMCDP2DDialog::updateOkButton(){

    bool ok=true;

    {
    auto depvel=layers();
    QPalette palette;
    for( int row = 1; row<depvel.size(); row++){
        if( depvel[row-1].first>=depvel[row].first){
            ok=false;
            palette.setColor(QPalette::Text, Qt::red);
        }
    }
    ui->twLayers->setPalette(palette);
    }

    {
    QPalette palette;
    if( dataset().isEmpty() || m_project->seismicDatasetList().contains(dataset())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);
    }

    {
    QPalette palette;
    if( offset1()>offset2()){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOffset1->setPalette(palette);
    ui->leOffset2->setPalette(palette);
    }

    ui->buttonBox->button( QDialogButtonBox::Ok)->setEnabled(ok);
}

#include<iostream>
void FMCDP2DDialog::setLayerCount(int nlayers)
{
    ui->twLayers->setRowCount(nlayers);

    for( int row=0; row<ui->twLayers->rowCount(); row++){
        if( !ui->twLayers->item(row,0)) ui->twLayers->setItem(row, 0, new QTableWidgetItem(""));
        if( !ui->twLayers->item(row,1)) ui->twLayers->setItem(row, 1, new QTableWidgetItem(""));
    }
}
