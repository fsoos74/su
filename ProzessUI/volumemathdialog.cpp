#include "volumemathdialog.h"
#include "ui_volumemathdialog.h"
#include<QComboBox>
#include<QPushButton>
#include<QDoubleValidator>
#include<mathprocessor.h>

VolumeMathDialog::VolumeMathDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::VolumeMathDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leValue1->setValidator(validator);
    ui->leValue2->setValidator(validator);

    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    ui->cbFunction->addItems(MathProcessor::opList("Volume"));
}

VolumeMathDialog::~VolumeMathDialog()
{
    delete ui;
}

QMap<QString,QString> VolumeMathDialog::params(){

    QMap<QString, QString> p;

    p.insert( "function", MathProcessor::toFunctionString(ui->cbFunction->currentText(), "Volume" ) );
    p.insert( "output-volume", ui->leOutput->text() );
    p.insert( "input-volume1", ui->cbInput1->currentText() );
    bool input2=ui->cbFunction->currentText().contains("Volume2");
    p.insert( "input-volume2", (input2) ? ui->cbInput2->currentText() : "" );
    p.insert("value1", ui->leValue1->text());
    p.insert("value2", ui->leValue2->text());
    p.insert( "top-horizon", ui->cbTopHorizon->currentText() );
    p.insert( "bottom-horizon", ui->cbBottomHorizon->currentText() );

    return p;
}


void VolumeMathDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    updateInputVolumes();
    updateHorizons();
}


void VolumeMathDialog::updateInputVolumes(){

    if( !m_project) return;
    ui->cbInput1->clear();
    ui->cbInput1->addItems(m_project->volumeList());
    ui->cbInput2->clear();
    ui->cbInput2->addItems(m_project->volumeList());
}

void VolumeMathDialog::updateHorizons(){

    if( !m_project) return;
    QStringList items;
    items<<"NONE";
    items<<m_project->gridList(GridType::Horizon);
    ui->cbTopHorizon->clear();
    ui->cbTopHorizon->addItems(items);
    ui->cbBottomHorizon->clear();
    ui->cbBottomHorizon->addItems(items);
}

void VolumeMathDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutput->text().isEmpty()){
          ok=false;
    }

    QPalette palette;
    if( reservedVolumes().contains(ui->leOutput->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
