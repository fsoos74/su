#include "exportseismicdialog.h"
#include "ui_exportseismicdialog.h"

#include<QIntValidator>
#include<QDoubleValidator>

#include<QFileDialog>

ExportSeismicDialog::ExportSeismicDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ExportSeismicDialog)
{
    ui->setupUi(this);

    QIntValidator* validator=new QIntValidator(this);
    ui->leMinInline->setValidator(validator);
    ui->leMaxInline->setValidator(validator);
    ui->leMinCrossline->setValidator(validator);
    ui->leMaxCrossline->setValidator(validator);
    ui->leShiftIl->setValidator(validator);
    ui->leShiftXl->setValidator(validator);

    QDoubleValidator* dvalidator=new QDoubleValidator(this);
    ui->leShiftX->setValidator(dvalidator);
    ui->leShiftY->setValidator(dvalidator);


    connect(ui->leMinInline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxInline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMinCrossline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxCrossline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

ExportSeismicDialog::~ExportSeismicDialog()
{
    delete ui;
}

void ExportSeismicDialog::setDatasets( const QStringList& h){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(h);
    updateOkButton();
}

void ExportSeismicDialog::setFixedDataset(const QString & s){

    // cannot select other ds because non-editable and no other items in list
    ui->cbDataset->clear();
    ui->cbDataset->addItem(s);
    updateOkButton();
}

void ExportSeismicDialog::setMinimumInline(int i){

    ui->leMinInline->setText(QString::number(i));
}

void ExportSeismicDialog::setMaximumInline(int i){

    ui->leMaxInline->setText(QString::number(i));
}

void ExportSeismicDialog::setMinimumCrossline(int i){

    ui->leMinCrossline->setText(QString::number(i));
}

void ExportSeismicDialog::setMaximumCrossline(int i){

    ui->leMaxCrossline->setText(QString::number(i));
}

void ExportSeismicDialog::on_pbBrowse_clicked()
{
    QString filter = "SEG-Y Files (*.sgy)";

    QFileDialog dialog(this, tr("Export Seismic Dataset"), QDir::homePath(), filter);
    dialog.setDefaultSuffix(tr(".sgy"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if( dialog.exec()==QDialog::Accepted && !dialog.selectedFiles().empty()){

        QString fn=dialog.selectedFiles().front();

       if( !fn.isNull()){
            ui->leOutputFile->setText(fn);
            updateOkButton();
        }
    }
}


QMap<QString,QString> ExportSeismicDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("dataset"), ui->cbDataset->currentText());

    p.insert( QString("output-file"), ui->leOutputFile->text() );

    if( ui->cbObscure->isChecked()){
        p.insert( QString("delta-iline" ), ui->leShiftIl->text() );
        p.insert( QString("delta-xline" ), ui->leShiftXl->text() );
        p.insert( QString("delta-x" ), ui->leShiftX->text() );
        p.insert( QString("delta-y" ), ui->leShiftY->text() );
    }

    if( ui->cbRestrictArea->isChecked()){
        p.insert( QString("min-inline" ), ui->leMinInline->text() );
        p.insert( QString("max-inline" ), ui->leMaxInline->text() );
        p.insert( QString("min-crossline" ), ui->leMinCrossline->text() );
        p.insert( QString("max-crossline" ), ui->leMaxCrossline->text() );
    }

    return p;
}


void ExportSeismicDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputFile->text().isEmpty()){
        ok=false;
    }

    QPalette inlinePalette;
    if(ui->leMinInline->text().toLong()>ui->leMaxInline->text().toLong()){

        ok=false;
        inlinePalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinInline->setPalette(inlinePalette);
    ui->leMaxInline->setPalette(inlinePalette);

    QPalette crosslinePalette;
    if(ui->leMinCrossline->text().toLong()>ui->leMaxCrossline->text().toLong()){

        ok=false;
        crosslinePalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinCrossline->setPalette(crosslinePalette);
    ui->leMaxCrossline->setPalette(crosslinePalette);


    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
