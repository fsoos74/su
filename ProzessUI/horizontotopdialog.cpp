#include "horizontotopdialog.h"
#include "ui_horizontotopdialog.h"

#include<QComboBox>
#include<QPushButton>


HorizonToTopDialog::HorizonToTopDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::HorizonToTopDialog)
{
    ui->setupUi(this);


    connect( ui->leTop, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->lwWells->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

HorizonToTopDialog::~HorizonToTopDialog()
{
    delete ui;
}


QMap<QString,QString> HorizonToTopDialog::params(){

    QMap<QString, QString> p;

    p.insert("top", ui->leTop->text());
    p.insert("horizon", ui->cbHorizon->currentText());
    auto wells=selectedWells();
    p.insert("wells", packParamList(wells));
    return p;
}


void HorizonToTopDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    auto horizons=m_project->gridList(GridType::Horizon);
    ui->cbHorizon->clear();
    ui->cbHorizon->addItems(horizons);

    auto wells=m_project->wellList();
    ui->lwWells->clear();
    ui->lwWells->addItems(wells);

    updateOkButton();
}


void HorizonToTopDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;

    auto wells=selectedWells();

    if(ui->leTop->text().isEmpty()) ok=false;
    if(wells.isEmpty()) ok=false;
    if(ui->cbHorizon->currentText().isEmpty()) ok=false;

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}


QStringList HorizonToTopDialog::selectedWells(){

    QStringList wells;
    auto ids = ui->lwWells->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        wells<<ui->lwWells->item(idx.row())->text();
    }
    return wells;
}
