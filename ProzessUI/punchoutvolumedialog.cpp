#include "punchoutvolumedialog.h"
#include "ui_punchoutvolumedialog.h"

#include <QPushButton>


PunchOutVolumeDialog::PunchOutVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::PunchOutVolumeDialog)
{
    ui->setupUi(this);

    connect( ui->leOutputVolume, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

PunchOutVolumeDialog::~PunchOutVolumeDialog()
{
    delete ui;
}


void PunchOutVolumeDialog::setProject(AVOProject* p){

    if( !p || p==m_project) return;
    m_project=p;
    ui->cbInputVolume->clear();
    ui->cbInputVolume->addItems(m_project->volumeList());
    ui->cbPoints->clear();
    ui->cbPoints->addItems(m_project->tableList());
    updateOkButton();
}



QMap<QString,QString> PunchOutVolumeDialog::params(){

    QMap<QString, QString> p;

    p.insert( "output-volume", ui->leOutputVolume->text() );
    p.insert( "input-volume", ui->cbInputVolume->currentText() );
    p.insert( "points-table", ui->cbPoints->currentText() );
    p.insert( "keep-points", QString::number(static_cast<int>(ui->cbMode->currentIndex()==0)));

    return p;
}

void PunchOutVolumeDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputVolume->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( m_project->volumeList().contains( ui->leOutputVolume->text())) {  // output name equals one of inputs
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputVolume->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

