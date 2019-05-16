#include "playerdialog.h"
#include "ui_playerdialog.h"

namespace sliceviewer {


PlayerDialog::PlayerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayerDialog)
{
    ui->setupUi(this);

    m_timer=new QTimer(this);
    m_timer->setSingleShot(true);
    connect( m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    ui->cbType->addItem( tr("Inline"), static_cast<int>(VolumeView::SliceType::Inline));
    ui->cbType->addItem( tr("Crossline"), static_cast<int>(VolumeView::SliceType::Crossline));
    ui->cbType->addItem( tr("depth/time"), static_cast<int>(VolumeView::SliceType::Z));

    connect( ui->slCurrent, SIGNAL(valueChanged(int)), ui->sbCurrent, SLOT(setValue(int)) );
    connect( ui->sbCurrent, SIGNAL(valueChanged(int)), ui->slCurrent, SLOT(setValue(int)) );
}

PlayerDialog::~PlayerDialog()
{
    delete ui;
}


VolumeView::SliceType PlayerDialog::type(){
    return static_cast<VolumeView::SliceType>(ui->cbType->currentData().toInt());
}

int PlayerDialog::start(){
    return ui->sbStart->value();
}

int PlayerDialog::stop(){
    return ui->sbStop->value();
}

int PlayerDialog::step(){
    return ui->sbStep->value();
}

int PlayerDialog::current(){
    return ui->slCurrent->value();
}

int PlayerDialog::delay(){
    return ui->sbDelay->value();
}

void PlayerDialog::setType(VolumeView::SliceType t){
    ui->cbType->setCurrentIndex(ui->cbType->findData(static_cast<int>(t)));
}

void PlayerDialog::setStart(int i){
    ui->sbStart->setValue(i);
}

void PlayerDialog::setStop(int i){
    ui->sbStop->setValue(i);
}

void PlayerDialog::setStep(int i){
    ui->sbStep->setValue(i);
}

void PlayerDialog::setCurrent(int i){
    ui->slCurrent->setValue(i);
}

void PlayerDialog::setDelay(int d){
    ui->sbDelay->setValue(d);
}

void PlayerDialog::setBounds(Grid3DBounds bounds){
    if( bounds==m_bounds) return;
    m_bounds=bounds;
    updateControls();
}

void PlayerDialog::pauseAnimation(){

    m_running=false;
    m_timer->stop();
    //ui->pbStart->setText("Start");
    ui->pbStart->setIcon(QIcon(":/icons/images/play-32x32.png"));
    enableControls(false);
}

void PlayerDialog::startAnimation(){

    m_running=true;
    m_timer->start(delay());
    //ui->pbStart->setText("Pause");
    ui->pbStart->setIcon(QIcon(":/icons/images/pause-32x32.png"));
    enableControls(false);
}

void PlayerDialog::stopAnimation()
{
    m_running=false;
    m_timer->stop();
    //ui->pbStart->setText("Start");
    ui->pbStart->setIcon(QIcon(":/icons/images/play-32x32.png"));
    setCurrent(start());
    enableControls(true);
}


void PlayerDialog::updateControls(){

    int min=0, max=0;
    switch(type()){
        case VolumeView::SliceType::Inline: min=m_bounds.i1(); max=m_bounds.i2(); break;
        case VolumeView::SliceType::Crossline: min=m_bounds.j1(); max=m_bounds.j2(); break;
        case VolumeView::SliceType::Z: min=static_cast<int>(1000*m_bounds.ft()); max=static_cast<int>(1000*m_bounds.lt()); break;
    }
    ui->sbStart->setRange(min,max);
    ui->sbStart->setValue(min);
    ui->sbStop->setRange(min,max);
    ui->sbStop->setValue(max);
    ui->sbCurrent->setRange(min,max);
    ui->sbCurrent->setValue(min);
    ui->sbStep->setRange(1, max-min+1);
    ui->sbStep->setValue(1);
}

void PlayerDialog::on_sbCurrent_valueChanged(int arg1)
{
    VolumeView::SliceDef def;
    def.type=type();
    def.value=arg1;
    emit sliceRequested(def);
}

void PlayerDialog::on_sbStart_valueChanged(int arg1)
{
    ui->slCurrent->setRange(arg1, ui->slCurrent->maximum());
}

void PlayerDialog::on_sbStop_valueChanged(int arg1)
{
    ui->slCurrent->setRange(ui->slCurrent->minimum(), arg1);
}

void PlayerDialog::on_sbStep_valueChanged(int arg1)
{
    ui->slCurrent->setSingleStep(arg1);
}

void PlayerDialog::on_cbType_currentIndexChanged(const QString &)
{
    updateControls();
}

void PlayerDialog::onTimeout(){
    if( !m_running) return;
    int i = current()+step();
    if( i>stop()) i=start();
    setCurrent(i);
    m_timer->start(delay());
}

void PlayerDialog::on_pbStart_clicked()
{
    if( m_timer->isActive()){
        pauseAnimation();
    }
    else{
        startAnimation();
    }

}

void PlayerDialog::on_pbStop_clicked()
{
   stopAnimation();
}

void PlayerDialog::enableControls(bool on){
    ui->sbStart->setEnabled(on);
    ui->sbStop->setEnabled(on);
    ui->sbStep->setEnabled(on);
    ui->sbDelay->setEnabled(on);
    ui->sbCurrent->setEnabled(on);
    ui->slCurrent->setEnabled(on);
    ui->cbType->setEnabled(on);
}

void PlayerDialog::on_pbClose_clicked()
{
    stopAnimation();
    hide();
}

}
