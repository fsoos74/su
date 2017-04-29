#include "gridmathdialog.h"
#include "ui_gridmathdialog.h"
#include<QComboBox>
#include<QPushButton>
#include<QDoubleValidator>


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

    ui->cbFunction->setCurrentIndex(0); // this will trigger update of enabled/disabled items
}

GridMathDialog::~GridMathDialog()
{
    delete ui;
}

QMap<QString,QString> GridMathDialog::params(){

    QMap<QString, QString> p;

    //QString fullGridName=createFullGridName( GridType::Attribute, ui->leResultGrid->text() );

    p.insert( QString("function"), QString::number( ui->cbFunction->currentIndex() ) );

    p.insert( QString("output-type"), ui->cbOutputType->currentText() );
    p.insert( QString("output-grid"), ui->leOutput->text() );

    p.insert( QString("input-grid1-type"), ui->cbInput1Type->currentText() );
    p.insert( QString("input-grid1"), ui->cbInput1->currentText() );

    if( ui->cbInput2Type->isEnabled()){
        p.insert( QString("input-grid2-type"), ui->cbInput2Type->currentText() );
        p.insert( QString("input-grid2"), ui->cbInput2->currentText() );
    }

    if( ui->leValue->isEnabled()){
        p.insert(QString("value"), ui->leValue->text());
    }

    return p;
}


void GridMathDialog::setInputGrids( QString type, const QStringList& l){

    if( !m_inputGrids.contains(type)){
        ui->cbOutputType->addItem(type);
        ui->cbInput1Type->addItem(type);
        ui->cbInput2Type->addItem(type);
    }

    m_inputGrids.insert(type, l);

    updateInputGrids();

}

void GridMathDialog::updateInputGrids(){

    ui->cbInput1->clear();
    ui->cbInput1->addItems(m_inputGrids.value(ui->cbInput1Type->currentText()));

    ui->cbInput2->clear();
    ui->cbInput2->addItems(m_inputGrids.value(ui->cbInput2Type->currentText()));

    updateOkButton();
}

void GridMathDialog::on_cbFunction_currentIndexChanged(int index)
{
    switch( index ){
    case 0:
    case 1:
        ui->leValue->setEnabled( true );
        ui->cbInput2Type->setEnabled( false );
        ui->cbInput2->setEnabled(false);
        break;
    case 2:
    case 3:
        ui->leValue->setEnabled( false );
        ui->cbInput2Type->setEnabled( true );
        ui->cbInput2->setEnabled(true);
        break;
    default:    // should not happen, add error code later
        break;
    }
}


void GridMathDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutput->text().isEmpty()){
        ok=false;
    }

    QString type=ui->cbOutputType->currentText();
    QPalette palette;
    if( m_inputGrids.contains(type) && m_inputGrids.value(type).contains(ui->leOutput->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
