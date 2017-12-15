#include "smoothgriddialog.h"
#include "ui_smoothgriddialog.h"

#include<QPushButton>
#include<avoproject.h>

SmoothGridDialog::SmoothGridDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::SmoothGridDialog)
{
    ui->setupUi(this);

    connect( ui->cbGridType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputGrids()) );
    connect( ui->leOutputName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbInputName, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

SmoothGridDialog::~SmoothGridDialog()
{
    delete ui;
}


QMap<QString,QString> SmoothGridDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("grid-type"), ui->cbGridType->currentText() );
    p.insert( QString("input-name"), ui->cbInputName->currentText() );
    p.insert( QString("output-name"), ui->leOutputName->text() );
    p.insert( QString("method"), QString::number(ui->cbMethod->currentIndex() ) );
    p.insert( QString("half-ilines"), QString::number(ui->sbInlines->value()/2) );
    p.insert( QString("half-xlines"), QString::number(ui->sbCrosslines->value()/2) );
    p.insert( QString("retain-values"), QString::number((ui->cbRetain->isChecked()) ? 1 : 0));
    return p;
}

void SmoothGridDialog::setInputGrids( QString type, const QStringList& l){

    if( !m_inputGrids.contains(type)){
        ui->cbGridType->addItem(type);
    }

    m_inputGrids.insert(type, l);

    if( type == ui->cbGridType->currentText()){
        updateInputGrids();
    }

    updateOkButton();
}

void SmoothGridDialog::updateInputGrids(){

    ui->cbInputName->clear();
    ui->cbInputName->addItems(m_inputGrids.value(ui->cbGridType->currentText()));

    updateOkButton();
}

void SmoothGridDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputName->text().isEmpty()){
        ok=false;
    }

    QString otype=ui->cbGridType->currentText();
    QPalette palette;
    if( m_inputGrids.contains(otype) && m_inputGrids.value(otype).contains(ui->leOutputName->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputName->setPalette(palette);

    ui->buttonBox->button( QDialogButtonBox::Ok)->setEnabled(ok);

}
