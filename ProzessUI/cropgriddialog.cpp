#include "cropgriddialog.h"
#include "ui_cropgriddialog.h"

#include <QPushButton>


CropGridDialog::CropGridDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::CropGridDialog)
{
    ui->setupUi(this);

    connect( ui->cbGridType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputGrids()) );
    connect( ui->cbInputGrid, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutputGrid, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->sbMinIline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMaxIline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMinXline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMaxXline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
}

CropGridDialog::~CropGridDialog()
{
    delete ui;
}


void CropGridDialog::setInputGrids( QString type, const QStringList& l){

    if( !m_inputGrids.contains(type)){
        ui->cbGridType->addItem(type);
    }

    m_inputGrids.insert(type, l);

    if( type == ui->cbGridType->currentText()){
        updateInputGrids();
    }

    updateOkButton();
}


void CropGridDialog::setInlineRange(int min, int max){

    ui->sbMinIline->setRange(min,max);
    ui->sbMaxIline->setRange(min,max);
    ui->sbMinIline->setValue(min);
    ui->sbMaxIline->setValue(max);
}

void CropGridDialog::setCrosslineRange(int min, int max){

    ui->sbMinXline->setRange(min,max);
    ui->sbMaxXline->setRange(min,max);
    ui->sbMinXline->setValue(min);
    ui->sbMaxXline->setValue(max);
}


QMap<QString,QString> CropGridDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("grid-type"), ui->cbGridType->currentText() );
    p.insert( QString("input-name"), ui->cbInputGrid->currentText() );
    p.insert( QString("output-name"), ui->leOutputGrid->text() );

    p.insert( QString("min-iline"), QString::number(ui->sbMinIline->value()));
    p.insert( QString("max-iline"), QString::number(ui->sbMaxIline->value()));

    p.insert( QString("min-xline"), QString::number(ui->sbMinXline->value()));
    p.insert( QString("max-xline"), QString::number(ui->sbMaxXline->value()));

    return p;
}


void CropGridDialog::updateInputGrids(){

    ui->cbInputGrid->clear();
    ui->cbInputGrid->addItems(m_inputGrids.value(ui->cbGridType->currentText()));

    updateOkButton();
}

void CropGridDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputGrid->text().isEmpty()){
        ok=false;
    }

    // output grid
    {
    QString otype=ui->cbGridType->currentText();
    QPalette palette;
    if( m_inputGrids.contains(otype) && m_inputGrids.value(otype).contains(ui->leOutputGrid->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputGrid->setPalette(palette);

    }

    // test inline range
    {
    QPalette palette;
    if( ui->sbMinIline->value() > ui->sbMaxIline->value() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->sbMinIline->setPalette(palette);
    ui->sbMaxIline->setPalette(palette);
    }

    // text crossline range
    {
    QPalette palette;
    if( ui->sbMinXline->value() > ui->sbMaxXline->value() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->sbMinXline->setPalette(palette);
    ui->sbMaxXline->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

