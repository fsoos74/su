#include "instantaneousattributesdialog.h"
#include "ui_instantaneousattributesdialog.h"

#include<QPushButton>
#include<avoproject.h>

InstantaneousAttributesDialog::InstantaneousAttributesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::InstantaneousAttributesDialog)
{
    ui->setupUi(this);

    connect( ui->leAmplitude, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->lePhase, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leFrequency, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

InstantaneousAttributesDialog::~InstantaneousAttributesDialog()
{
    delete ui;
}

void InstantaneousAttributesDialog::setInputVolumes( const QStringList& h){
    ui->cbInputVolume->clear();
    ui->cbInputVolume->addItems(h);
}

QMap<QString,QString> InstantaneousAttributesDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("input-volume"), ui->cbInputVolume->currentText() );
    p.insert( QString("amplitude-volume"), ui->leAmplitude->text() );
    p.insert( QString("phase-volume"), ui->lePhase->text() );
    p.insert( QString("frequency-volume"), ui->leFrequency->text() );

    return p;
}

void InstantaneousAttributesDialog::updateOkButton(){

    bool ok=true;

    for( auto p : {ui->leAmplitude, ui->lePhase, ui->leFrequency } ){

        if(p->text().isEmpty()) ok=false;

        QPalette palette;
        if( reservedVolumes().contains(p->text())){
            ok=false;
            palette.setColor(QPalette::Text, Qt::red);
        }
        p->setPalette(palette);
    }

    for( auto p : {std::make_pair(ui->leAmplitude, ui->lePhase),
                    std::make_pair(ui->leAmplitude,ui->leFrequency),
                    std::make_pair(ui->lePhase, ui->leFrequency)}){
        if( !p.first->text().isEmpty() &&  p.first->text()==p.second->text()){
            ok=false;
            QPalette palette;
            palette.setColor(QPalette::Text, Qt::red);
            p.first->setPalette(palette);
            p.second->setPalette(palette);
        }
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

