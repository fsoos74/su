#include "tabletohorizondialog.h"
#include "ui_tabletohorizondialog.h"

#include<QComboBox>
#include<QPushButton>
#include<memory>

TableToHorizonDialog::TableToHorizonDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::TableToHorizonDialog)
{
    ui->setupUi(this);


    connect( ui->leHorizon, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->cbTable, SIGNAL(currentIndexChanged(QString)),ui->leHorizon,SLOT(setText(QString)));

    updateOkButton();
}

TableToHorizonDialog::~TableToHorizonDialog()
{
    delete ui;
}


QMap<QString,QString> TableToHorizonDialog::params(){

    QMap<QString, QString> p;

    p.insert("horizon", ui->leHorizon->text());
    p.insert("table", ui->cbTable->currentText());
    return p;
}


void TableToHorizonDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    ui->cbTable->clear();
    for( auto name : m_project->tableList()){
        auto tinfo = m_project->getTableInfo(name);
        bool multi = std::get<2>(tinfo);
        if(!multi) ui->cbTable->addItem(name);      // only one value per cdp allowed for horizons!
    }

    updateOkButton();
}


void TableToHorizonDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;

    QPalette palette;
    if(ui->leHorizon->text().isEmpty()){
        ok=false;
    }
    else if( m_project->existsGrid(GridType::Horizon, ui->leHorizon->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leHorizon->setPalette(palette);

    if(ui->cbTable->currentText().isEmpty()){
        ok=false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
