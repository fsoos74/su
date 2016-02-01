#include "interceptgradientvolumedialog.h"
#include "ui_interceptgradientvolumedialog.h"

#include<QDoubleValidator>
#include<QPushButton>

#include<avoproject.h>

InterceptGradientVolumeDialog::InterceptGradientVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::InterceptGradientVolumeDialog)
{
    ui->setupUi(this);

    windowValidator=new QIntValidator(this);
    ui->leWindowStart->setValidator(windowValidator);
    ui->leWindowEnd->setValidator(windowValidator);

    QDoubleValidator* doubleValidator=new QDoubleValidator(this);
    doubleValidator->setBottom(0);
    ui->leMaxOffset->setValidator(doubleValidator);
    ui->leMinAzimuth->setValidator(doubleValidator);
    ui->leMaxAzimuth->setValidator(doubleValidator);

    connect( ui->leGradient, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leIntercept, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leWindowStart, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leWindowEnd, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaxOffset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbRestrictOffset, SIGNAL( toggled(bool)), this, SLOT(updateOkButton()) );
    connect( ui->cbTimeWindow, SIGNAL( toggled(bool)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

InterceptGradientVolumeDialog::~InterceptGradientVolumeDialog()
{
    delete ui;
}

void InterceptGradientVolumeDialog::setDatasets( const QStringList& h){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(h);
}

void InterceptGradientVolumeDialog::setWindowStartMS( int t ){
    ui->leWindowStart->setText(QString::number(t));
}

void InterceptGradientVolumeDialog::setWindowEndMS( int t ){
    ui->leWindowEnd->setText(QString::number(t));
}

QMap<QString,QString> InterceptGradientVolumeDialog::params(){

    QMap<QString, QString> p;


    p.insert( QString("intercept"), ui->leIntercept->text());

    p.insert( QString("gradient"), ui->leGradient->text() );

    p.insert( QString("dataset"), ui->cbDataset->currentText());

    double maxOffset=std::numeric_limits<double>::max();
    if( ui->cbRestrictOffset->isChecked()){
        maxOffset=ui->leMaxOffset->text().toDouble();
    }
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


void InterceptGradientVolumeDialog::updateOkButton(){

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

    if( ui->cbRestrictOffset->isChecked()){
        if( ui->leMaxOffset->text().isEmpty()){
            ok=false;
        }
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
