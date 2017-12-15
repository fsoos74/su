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
    ui->leValue->setValidator(validator);

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

    ui->cbFunction->addItems(MathProcessor::opList());
}

GridMathDialog::~GridMathDialog()
{
    delete ui;
}

QMap<QString,QString> GridMathDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("function"), ui->cbFunction->currentText() );

    p.insert( QString("output-type"), ui->cbOutputType->currentText() );
    p.insert( QString("output-name"), ui->leOutput->text() );

    p.insert( QString("input-grid1-type"), ui->cbInput1Type->currentText() );
    p.insert( QString("input-grid1"), ui->cbInput1->currentText() );

    p.insert( QString("input-grid2-type"), ui->cbInput2Type->currentText() );
    p.insert( QString("input-grid2"), (ui->cbInput2Type->isEnabled()) ? ui->cbInput2->currentText() : "" );

    //if( ui->leValue->isEnabled()){
        p.insert(QString("value"), ui->leValue->text());
    //}

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

void GridMathDialog::on_cbFunction_currentIndexChanged(QString arg1)
{
    ui->leValue->setEnabled(arg1.contains("Value"));
    ui->cbInput2->setEnabled(arg1.contains("Input2"));
    ui->cbInput2Type->setEnabled(arg1.contains("Input2"));
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
