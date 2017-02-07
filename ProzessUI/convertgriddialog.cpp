#include "convertgriddialog.h"
#include "ui_convertgriddialog.h"


#include<QPushButton>


ConvertGridDialog::ConvertGridDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ConvertGridDialog)
{
    ui->setupUi(this);

    connect( ui->cbInputType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputGrids()) );
    connect( ui->cbOutputType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateOkButton()) );
}

ConvertGridDialog::~ConvertGridDialog()
{
    delete ui;
}


QMap<QString,QString> ConvertGridDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("input-type"), ui->cbInputType->currentText() );
    p.insert( QString("input-name"), ui->cbInputName->currentText() );

    p.insert( QString("output-type"), ui->cbOutputType->currentText() );
    p.insert( QString("output-name"), ui->leOutputName->text() );

    return p;
}


void ConvertGridDialog::setInputGrids( QString type, const QStringList& l){

    if( !m_inputGrids.contains(type)){
        ui->cbInputType->addItem(type);
        ui->cbOutputType->addItem(type);
    }

    m_inputGrids.insert(type, l);

    if( type == ui->cbInputType->currentText()){
        updateInputGrids();
    }

}

void ConvertGridDialog::updateInputGrids(){

    ui->cbInputName->clear();
    ui->cbInputName->addItems(m_inputGrids.value(ui->cbInputType->currentText()));

    updateOkButton();
}


void ConvertGridDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputName->text().isEmpty()){
        ok=false;
    }

    QString otype=ui->cbOutputType->currentText();
    QPalette palette;
    if( m_inputGrids.contains(otype) && m_inputGrids.value(otype).contains(ui->leOutputName->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputName->setPalette(palette);

    ui->buttonBox->button( QDialogButtonBox::Ok)->setEnabled(ok);
}

