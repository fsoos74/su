#include "headerscandialog.h"
#include "ui_headerscandialog.h"

HeaderScanDialog::HeaderScanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HeaderScanDialog)
{
    ui->setupUi(this);

    QFontMetrics fm(ui->textEdit->font());
    int w=fm.width(QString("123456789012345678901234567890123456789012345678901234567890")); // 60 chars per line
    ui->textEdit->setMinimumWidth(w);
}

HeaderScanDialog::~HeaderScanDialog()
{
    delete ui;
}

void HeaderScanDialog::setData( QStringList data){

    ui->textEdit->clear();
    for( auto s : data ){
        ui->textEdit->append(s);
    }

}
