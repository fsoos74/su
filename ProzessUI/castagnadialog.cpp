#include "castagnadialog.h"
#include "ui_castagnadialog.h"

#include<QComboBox>
#include<QPushButton>
#include<QDoubleValidator>

CastagnaDialog::CastagnaDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::CastagnaDialog)
{
    ui->setupUi(this);

    auto validator=new QDoubleValidator(this);
    ui->leA0->setValidator(validator);
    ui->leA1->setValidator(validator);
    ui->leA2->setValidator(validator);

    connect( ui->cbVP, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateInputWells(QString)) );
    connect( ui->leVS, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbCoefficients, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateCoefficients()));
    connect( ui->cbUnit, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateCoefficients()));

    updateOkButton();
}

CastagnaDialog::~CastagnaDialog()
{
    delete ui;
}


QMap<QString,QString> CastagnaDialog::params(){

    QMap<QString, QString> p;

    auto wells=selectedWells();
    p.insert("wells", packParamList(wells));
    p.insert( "vp-log", ui->cbVP->currentText() );
    p.insert( "vs-log", ui->leVS->text() );
    p.insert( "a0", ui->leA0->text() );
    p.insert( "a1", ui->leA1->text() );
    p.insert( "a2", ui->leA2->text() );
    return p;
}


void CastagnaDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    QSet<QString> logs;
    for( auto well : m_project->wellList()){
        for(auto log : m_project->logList(well)){
            logs.insert(log);
        }
    }
    QStringList list(logs.toList());
    std::sort(list.begin(),list.end());
    ui->cbVP->addItems(list);
    auto wells=m_project->wellList();
    ui->lwWells->clear();
    ui->lwWells->addItems(wells);
}


void CastagnaDialog::updateInputWells(QString log){

    ui->lwWells->clear();
    if( !m_project) return;

    QStringList wells;
    for( auto well : m_project->wellList()){
        if( m_project->existsLog(well,log)){
            wells.append(well);
        }
    }
    std::sort(wells.begin(),wells.end());
    ui->lwWells->addItems(wells);
}

void CastagnaDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;
    QStringList onames;

    auto wells=selectedWells();

    QSet<QString> usedLogs;
    for( auto well : selectedWells()){
        usedLogs.unite(m_project->logList(well).toSet());
    }

    {
    QPalette palette;
    auto vsname=ui->leVS->text();
    if( vsname.isEmpty() ){
        ok=false;
    }
    else if(usedLogs.contains(vsname)){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    onames<<vsname;
    ui->leVS->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}


QStringList CastagnaDialog::selectedWells(){

    QStringList wells;
    auto ids = ui->lwWells->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        wells<<ui->lwWells->item(idx.row())->text();
    }
    return wells;
}

void CastagnaDialog::updateCoefficients()
{
    auto type=ui->cbCoefficients->currentText();
    if(type=="Custom") return;
    auto unit=ui->cbUnit->currentText();
    double a0,a1,a2;
    if(type=="Sandstone"){
       a2=0; a1=0.80416; a0=-0.85588;
    }
    else if(type=="Limestone"){
        a2=-0.05508; a1=1.01677; a0=-1.03049;
    }
    else if(type=="Dolomite"){
        a2=0; a1=0.58321; a0=-0.07775;
    }
    else if(type=="Shale"){
        a2=0; a1=0.76969; a0=-0.86735;
    }
    auto fac=1.;
    if(unit=="Meters/Second"){
        fac=1000.;
    }
    else if(unit=="Feet/Second"){
        fac=3280.84;
    }
    a0*=fac;  // only additive needs to be adjusted
    ui->leA0->setText(QString::number(a0));
    ui->leA1->setText(QString::number(a1));
    ui->leA2->setText(QString::number(a2));
}
