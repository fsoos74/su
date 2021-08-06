#include "blmfrequenciesdialog.h"
#include "ui_blmfrequenciesdialog.h"

BLMFrequenciesDialog::BLMFrequenciesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::BLMFrequenciesDialog)
{
    ui->setupUi(this);
}

BLMFrequenciesDialog::~BLMFrequenciesDialog()
{
    delete ui;
}


void BLMFrequenciesDialog::setExportPath(QString str){
    ui->lePath->setText(str);
}

void BLMFrequenciesDialog::setLineName(QString str){
    ui->leLine->setText(str);
}

QMap<QString,QString> BLMFrequenciesDialog::params(){

    QMap<QString, QString> p;


    p.insert( "path", ui->lePath->text() );

    p.insert( "line", ui->leLine->text() );

    return p;
}
