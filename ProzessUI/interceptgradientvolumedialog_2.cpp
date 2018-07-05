#include "interceptgradientvolumedialog_2.h"
#include "ui_interceptgradientvolumedialog_2.h"

#include<QDoubleValidator>
#include<QPushButton>

#include<avoproject.h>

InterceptGradientVolumeDialog_2::InterceptGradientVolumeDialog_2(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::InterceptGradientVolumeDialog_2)
{
    ui->setupUi(this);

    windowValidator=new QIntValidator(this);
    ui->leWindowStart->setValidator(windowValidator);
    ui->leWindowEnd->setValidator(windowValidator);

    QDoubleValidator* doubleValidator=new QDoubleValidator(this);
    //doubleValidator->setBottom(0);
    ui->leMinOffset->setValidator(doubleValidator);
    ui->leMaxOffset->setValidator(doubleValidator);
    ui->leMinAzimuth->setValidator(doubleValidator);
    ui->leMaxAzimuth->setValidator(doubleValidator);

    connect( ui->leGradient, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leIntercept, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leQuality, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leWindowStart, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leWindowEnd, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMinOffset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaxOffset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbRestrictOffset, SIGNAL( toggled(bool)), this, SLOT(updateOkButton()) );
    connect( ui->cbTimeWindow, SIGNAL( toggled(bool)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

InterceptGradientVolumeDialog_2::~InterceptGradientVolumeDialog_2()
{
    delete ui;
}

void InterceptGradientVolumeDialog_2::setDatasets( const QStringList& h){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(h);
}

void InterceptGradientVolumeDialog_2::setWindowStartMS( int t ){
    ui->leWindowStart->setText(QString::number(t));
}

void InterceptGradientVolumeDialog_2::setWindowEndMS( int t ){
    ui->leWindowEnd->setText(QString::number(t));
}

QMap<QString,QString> InterceptGradientVolumeDialog_2::params(){

    QMap<QString, QString> p;


    p.insert( QString("intercept"), ui->leIntercept->text());

    p.insert( QString("gradient"), ui->leGradient->text() );

    if( !ui->leQuality->text().isEmpty()){
        p.insert( QString("quality"), ui->leQuality->text() );
    }

    p.insert( QString("dataset"), ui->cbDataset->currentText());

    double minOffset=std::numeric_limits<double>::lowest();
    double maxOffset=std::numeric_limits<double>::max();
    if( ui->cbRestrictOffset->isChecked()){
        minOffset=ui->leMinOffset->text().toDouble();
        maxOffset=ui->leMaxOffset->text().toDouble();
    }
    p.insert( QString("minimum-offset"), QString::number(minOffset));
    p.insert( QString("maximum-offset"), QString::number(maxOffset));

    p.insert( "minimum-azimuth", ui->cbRestrictAzimuth->isChecked() ? ui->leMinAzimuth->text() : QString::number(0.));
    p.insert( "maximum-azimuth", ui->cbRestrictAzimuth->isChecked() ? ui->leMaxAzimuth->text() : QString::number(180.));


    if( ui->cbSuperGather->isChecked()){

        int ilsize=ui->sbInlines->value();
        int xlsize=ui->sbCrosslines->value();
        p.insert( QString("sg-inline-size"), QString::number(ilsize) );
        p.insert( QString("sg-crossline-size"), QString::number(xlsize));
    }

    if( ui->cbTimeWindow->isChecked()){

        p.insert("window-mode", "TRUE");
        p.insert("window-start", ui->leWindowStart->text());
        p.insert("window-end", ui->leWindowEnd->text());

    }
    return p;
}


void InterceptGradientVolumeDialog_2::updateOkButton(){

    bool ok=true;

    if( ui->leIntercept->text().isEmpty()){
        ok=false;
    }

    QPalette interceptPalette;
    if( reservedVolumes().contains(ui->leIntercept->text())){
        ok=false;
        interceptPalette.setColor(QPalette::Text, Qt::red);
    }
    if( ui->leIntercept->text() == ui->leGradient->text()){
        ok=false;
        interceptPalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leIntercept->setPalette(interceptPalette);

    if( ui->leGradient->text().isEmpty()){
        ok=false;
    }

    QPalette gradientPalette;
    if( reservedVolumes().contains(ui->leGradient->text())){
        ok=false;
        gradientPalette.setColor(QPalette::Text, Qt::red);
    }
    if( ui->leIntercept->text() == ui->leGradient->text()){
        ok=false;
        gradientPalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leGradient->setPalette(gradientPalette);

    QPalette qualityPalette;
    if( reservedVolumes().contains(ui->leQuality->text())){
        ok=false;
        qualityPalette.setColor(QPalette::Text, Qt::red);
    }
    if( ui->leIntercept->text() == ui->leQuality->text() || ui->leGradient->text()==ui->leQuality->text()){
        ok=false;
        qualityPalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leQuality->setPalette(qualityPalette);

    if( ui->cbTimeWindow->isChecked() ){
        if( ui->leWindowStart->text().isEmpty()){
            ok=false;
        }
        if( ui->leWindowEnd->text().isEmpty()){
            ok=false;
        }

        QPalette windowPalette;
        if( ui->leWindowStart->text().toInt()>ui->leWindowEnd->text().toInt()){
            windowPalette.setColor(QPalette::Text, Qt::red);
        }
        ui->leWindowStart->setPalette(windowPalette);
        ui->leWindowEnd->setPalette(windowPalette);

    }

    QPalette offsetPalette;
    if( ui->cbRestrictOffset->isChecked() &&  ui->leMinOffset->text().toDouble()>ui->leMaxOffset->text().toDouble()){
        ok=false;
        offsetPalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinOffset->setPalette(offsetPalette);
    ui->leMaxOffset->setPalette(offsetPalette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
