#include "gridmathdialog.h"
#include "ui_gridmathdialog.h"
#include<QComboBox>
#include<QPushButton>
#include<QDoubleValidator>

#include<mathprocessor.h>
#include<iostream>

GridMathDialog::GridMathDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::GridMathDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leValue1->setValidator(validator);
    ui->leValue2->setValidator(validator);

    connect( ui->cbOutputType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbInput1Type, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputGrids()) );
    connect( ui->cbInput2Type, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputGrids()) );

    QStringList gtypes;
    gtypes<<toQString(GridType::Horizon);
    gtypes<<toQString(GridType::Attribute);
    gtypes<<toQString(GridType::Other);
    ui->cbOutputType->addItems(gtypes);
    ui->cbInput1Type->addItems(gtypes);
    ui->cbInput2Type->addItems(gtypes);

    ui->cbFunction->addItems(MathProcessor::opList("Grid"));
}

GridMathDialog::~GridMathDialog()
{
    delete ui;
}

QMap<QString,QString> GridMathDialog::params(){

    QMap<QString, QString> p;

    p.insert( "function", MathProcessor::toFunctionString(ui->cbFunction->currentText(), "Grid") );

    p.insert( "output-type", ui->cbOutputType->currentText() );
    p.insert( "output-name", ui->leOutput->text() );

    p.insert( "input-grid1-type", ui->cbInput1Type->currentText() );
    p.insert( "input-grid1", ui->cbInput1->currentText() );

    bool input2=ui->cbFunction->currentText().contains("Grid2");
    p.insert( "input-grid2-type", ui->cbInput2Type->currentText() );
    p.insert( "input-grid2", (input2) ? ui->cbInput2->currentText() : "" );
    p.insert( "value1", ui->leValue1->text());
    p.insert( "value2", ui->leValue2->text());

    return p;
}


void GridMathDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    updateInputGrids();
}


void GridMathDialog::updateInputGrids(){

    if(!m_project) return;

    auto itype1=toGridType(ui->cbInput1Type->currentText());
    ui->cbInput1->clear();
    auto items1=m_project->gridList(itype1);
    if(!items1.isEmpty()) ui->cbInput1->addItems(items1);

    auto itype2=toGridType(ui->cbInput2Type->currentText());
    ui->cbInput2->clear();
    auto items2=m_project->gridList(itype2);
    if(!items2.isEmpty()) ui->cbInput2->addItems(items2);

    updateOkButton();
}


void GridMathDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;

    auto otype=toGridType(ui->cbOutputType->currentText());
    auto oname=ui->leOutput->text();

    QPalette palette;
    if( oname.isEmpty() || m_project->existsGrid(otype, oname) ){    // make sure not to call existsgrid with empty name!!!
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
