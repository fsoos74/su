#include "convertlogdepthtimedialog.h"
#include "ui_convertlogdepthtimedialog.h"

#include <QIntValidator>
#include <QPushButton>

ConvertLogDepthTimeDialog::ConvertLogDepthTimeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConvertLogDepthTimeDialog)
{
    ui->setupUi(this);

    QIntValidator* ivalidator=new QIntValidator(this);
    ivalidator->setBottom(0);
    ui->leStart->setValidator(ivalidator);
    ui->leStop->setValidator(ivalidator);
    ui->cbInterval->setValidator(ivalidator);

    connect(ui->leStart, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leStop, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->cbInterval, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
}

ConvertLogDepthTimeDialog::~ConvertLogDepthTimeDialog()
{
    delete ui;
}


int ConvertLogDepthTimeDialog::start(){
    return ui->leStart->text().toInt();
}



int ConvertLogDepthTimeDialog::stop(){
    return ui->leStop->text().toInt();
}


int ConvertLogDepthTimeDialog::interval(){
    return ui->cbInterval->currentText().toInt();
}


qreal ConvertLogDepthTimeDialog::depth0(){
    return ui->leDepth0->text().toDouble();
}

QString ConvertLogDepthTimeDialog::velocityLog(){
    return ui->cbVelocity->currentText();
}


void ConvertLogDepthTimeDialog::setStart(int i){
    ui->leStart->setText(QString::number(i));
}


void ConvertLogDepthTimeDialog::setStop(int i){
    ui->leStop->setText(QString::number(i));
}


void ConvertLogDepthTimeDialog::setInterval(int i){
    ui->cbInterval->setCurrentText(QString::number(i));
}


void ConvertLogDepthTimeDialog::setDepth0(qreal d){
    ui->leDepth0->setText(QString::number(d));
}

void ConvertLogDepthTimeDialog::setVelocityLogs(QStringList l){
    ui->cbVelocity->clear();
    ui->cbVelocity->addItems(l);
}

void ConvertLogDepthTimeDialog::setVelocityLog(QString l){
    ui->cbVelocity->setCurrentText(l);
}

void ConvertLogDepthTimeDialog::updateOkButton(){

    bool ok=true;

    // test range
    {
    QPalette palette;
    if( start()>=stop() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leStart->setPalette(palette);
    ui->leStop->setPalette(palette);
    }

    // test interval
    {
    QPalette palette;
    if( interval()<=0 ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->cbInterval->setPalette(palette);
    }


    ui->buttonBox->button( QDialogButtonBox::Ok)->setEnabled(ok);
}

