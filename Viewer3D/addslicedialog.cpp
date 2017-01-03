#include "addslicedialog.h"
#include "ui_addslicedialog.h"

#include<QIntValidator>

AddSliceDialog::AddSliceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSliceDialog)
{
    ui->setupUi(this);

    QIntValidator* validator=new QIntValidator(this);
    ui->leValue->setValidator(validator);

    ui->cbType->addItem(toQString(SliceType::INLINE));
    ui->cbType->addItem(toQString(SliceType::CROSSLINE));
    ui->cbType->addItem(toQString(SliceType::SAMPLE));
}

AddSliceDialog::~AddSliceDialog()
{
    delete ui;
}


SliceDef AddSliceDialog::sliceDef(){

    SliceDef def;

    def.type=toSliceType( ui->cbType->currentText());
    def.value=ui->leValue->text().toInt();

    return def;
}


void AddSliceDialog::setSliceDef(SliceDef def){

    QString str=toQString(def.type);
    int idx=ui->cbType->findText(str);
    Q_ASSERT( idx>=0);
    ui->cbType->setCurrentIndex(idx);

    ui->leValue->setText(QString::number(def.value));
}
