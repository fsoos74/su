#include "computeinterceptgradientdialog.h"
#include "ui_computeinterceptgradientdialog.h"

#include<QDoubleValidator>
#include<QPushButton>

#include<avoproject.h>

ComputeInterceptGradientDialog::ComputeInterceptGradientDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ComputeInterceptGradientDialog)
{
    ui->setupUi(this);

    QDoubleValidator* doubleValidator=new QDoubleValidator(this);
    doubleValidator->setBottom(0);
    ui->leMaxOffset->setValidator(doubleValidator);
    ui->leDepth->setValidator(doubleValidator);

    connect( ui->leIntercept, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leGradient, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leDepth, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaxOffset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbRestrictOffset, SIGNAL( toggled(bool)), this, SLOT(updateOkButton()) );
    connect( ui->cbAVA, SIGNAL( toggled(bool)), this, SLOT(updateOkButton()) );
    updateOkButton();
}

ComputeInterceptGradientDialog::~ComputeInterceptGradientDialog()
{
    delete ui;
}

void ComputeInterceptGradientDialog::setDatasets( const QStringList& h){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(h);
}

void ComputeInterceptGradientDialog::setHorizons( const QStringList& h){
    ui->cbHorizon->clear();
    ui->cbHorizon->addItems(h);
}

void ComputeInterceptGradientDialog::setReductionMethods( const QStringList& h){
    ui->cbMethod->clear();
    ui->cbMethod->addItems(h);
}

void ComputeInterceptGradientDialog::setNumberOfSamples( int n ){
    ui->sbSamples->setValue(n);
}

QMap<QString,QString> ComputeInterceptGradientDialog::params(){

    QMap<QString, QString> p;

    QString fullInterceptName=createFullGridName( GridType::Attribute, ui->leIntercept->text() );
    QString fullGradientName=createFullGridName( GridType::Attribute, ui->leGradient->text() );

    p.insert( QString("intercept"), fullInterceptName );

    p.insert( QString("gradient"), fullGradientName );

    p.insert( QString("dataset"), ui->cbDataset->currentText());

    p.insert( QString("horizon"), ui->cbHorizon->currentText() );

    p.insert( QString("reduction-method"), ui->cbMethod->currentText() );

    p.insert( QString("samples"), QString::number(ui->sbSamples->value() ) );

    double maxOffset=std::numeric_limits<double>::max();
    if( ui->cbRestrictOffset->isChecked()){
        maxOffset=ui->leMaxOffset->text().toDouble();
    }
    p.insert( QString("maximum-offset"), QString::number(maxOffset));


    if( ui->cbSuperGather->isChecked()){

        p.insert("supergathers","true");
        int ilsize=ui->sbInlines->value();
        int xlsize=ui->sbCrosslines->value();
        p.insert( QString("sg-inline-size"), QString::number(ilsize) );
        p.insert( QString("sg-crossline-size"), QString::number(xlsize));
    }

    if( ui->cbAVA->isChecked()){
        p.insert("AVA", "true");
        double depth=ui->leDepth->text().toDouble();
        p.insert("depth", QString::number(depth));
    }

    if( ui->cbDisplayIntercept->isChecked()){
        p.insert(QString("display-grid"), fullInterceptName );
    }

    if( ui->cbDisplayGradient->isChecked()){
        p.insertMulti(QString("display-grid"), fullGradientName );
    }



    return p;
}

void ComputeInterceptGradientDialog::updateOkButton(){

    bool ok=true;

    if( ui->leIntercept->text().isEmpty()){
        ok=false;
    }

    QPalette interceptPalette;
    if( reservedGrids().contains(ui->leIntercept->text())){
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
    if( reservedGrids().contains(ui->leGradient->text())){
        ok=false;
        gradientPalette.setColor(QPalette::Text, Qt::red);
    }
    if( ui->leIntercept->text() == ui->leGradient->text()){
        ok=false;
        gradientPalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leGradient->setPalette(gradientPalette);

    if(ui->cbAVA->isChecked() ){
        if( ui->leDepth->text().isEmpty()){
            ok=false;
        }
    }

    if( ui->cbRestrictOffset->isChecked()){
        if(ui->leMaxOffset->text().isEmpty()){
            ok=false;
        }
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
