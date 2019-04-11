#include "mergevolumesdialog.h"
#include "ui_mergevolumesdialog.h"

#include<QPushButton>
#include<QDoubleValidator>
#include<QIntValidator>

MergeVolumesDialog::MergeVolumesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::MergeVolumesDialog)
{
    ui->setupUi(this);

    QIntValidator* ivalidator=new QIntValidator(this);

    ui->leMinIline1->setValidator(ivalidator);
    ui->leMaxIline1->setValidator(ivalidator);
    ui->leMinIline2->setValidator(ivalidator);
    ui->leMaxIline2->setValidator(ivalidator);
    ui->leMinIline1->setText("1");
    ui->leMaxIline1->setText("9999");
    ui->leMinIline2->setText("1");
    ui->leMaxIline2->setText("9999");
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMinIline1, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaxIline1, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMinIline2, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaxIline2, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

MergeVolumesDialog::~MergeVolumesDialog()
{
    delete ui;
}

void MergeVolumesDialog::setInputVolumes( const QStringList& h){
    ui->cbVolume1->clear();
    ui->cbVolume1->addItems(h);
    ui->cbVolume2->clear();
    ui->cbVolume2->addItems(h);
}

QMap<QString,QString> MergeVolumesDialog::params(){

    QMap<QString, QString> p;

    p.insert("overlap", QString::number(ui->cbOverlap->currentIndex()));
    p.insert("volume1", ui->cbVolume1->currentText());
    p.insert( QString("min-iline1"), QString::number(ui->leMinIline1->text().toInt()));
    p.insert( QString("max-iline1"), QString::number(ui->leMaxIline1->text().toInt()));
    p.insert("volume2", ui->cbVolume2->currentText());
    p.insert( QString("min-iline2"), QString::number(ui->leMinIline2->text().toInt()));
    p.insert( QString("max-iline2"), QString::number(ui->leMaxIline2->text().toInt()));
    p.insert("output", ui->leOutput->text() );

    return p;
}


void MergeVolumesDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutput->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( reservedVolumes().contains(ui->leOutput->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);
    }

    // text inline range
    {
    QPalette palette;
    if( ui->leMinIline1->text().toInt() > ui->leMaxIline1->text().toInt() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinIline1->setPalette(palette);
    ui->leMaxIline1->setPalette(palette);
    }

    // text inline range
    {
    QPalette palette;
    if( ui->leMinIline2->text().toInt() > ui->leMaxIline2->text().toInt() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinIline2->setPalette(palette);
    ui->leMaxIline2->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

