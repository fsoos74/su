#include "exportvolumedialog.h"
#include "ui_exportvolumedialog.h"

#include<QIntValidator>
#include<QDoubleValidator>
#include<QFileDialog>

ExportVolumeDialog::ExportVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ExportVolumeDialog)
{
    ui->setupUi(this);

    QIntValidator* ivalidator=new QIntValidator(this);
    ui->leMinInline->setValidator(ivalidator);
    ui->leMaxInline->setValidator(ivalidator);
    ui->leMinCrossline->setValidator(ivalidator);
    ui->leMaxCrossline->setValidator(ivalidator);

    QDoubleValidator* dvalidator=new QDoubleValidator(this);
    ui->leMinTime->setValidator(dvalidator);
    ui->leMaxTime->setValidator(dvalidator);

    connect(ui->leMinInline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxInline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMinCrossline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxCrossline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMinTime, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxTime, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

ExportVolumeDialog::~ExportVolumeDialog()
{
    delete ui;
}

void ExportVolumeDialog::setVolumes( const QStringList& h){
    ui->cbVolume->clear();
    ui->cbVolume->addItems(h);
    updateOkButton();
}

void ExportVolumeDialog::setFixedVolume(const QString & s){

    // cannot select other volume because non-editable and no other items in list
    ui->cbVolume->clear();
    ui->cbVolume->addItem(s);
    updateOkButton();
}

void ExportVolumeDialog::setMinimumInline(int i){

    ui->leMinInline->setText(QString::number(i));
}

void ExportVolumeDialog::setMaximumInline(int i){

    ui->leMaxInline->setText(QString::number(i));
}

void ExportVolumeDialog::setMinimumCrossline(int i){

    ui->leMinCrossline->setText(QString::number(i));
}

void ExportVolumeDialog::setMaximumCrossline(int i){

    ui->leMaxCrossline->setText(QString::number(i));
}

void ExportVolumeDialog::setMinimumTime(double t){

    ui->leMinTime->setText(QString::number(t));
}

void ExportVolumeDialog::setMaximumTime(double t){

    ui->leMaxTime->setText(QString::number(t));
}


void ExportVolumeDialog::on_pbBrowse_clicked()
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


QMap<QString,QString> ExportVolumeDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("volume"), ui->cbVolume->currentText());

    p.insert( QString("output-file"), ui->leOutputFile->text() );

    if( ui->cbRestrictVolume->isChecked()){
        p.insert( QString("min-inline" ), ui->leMinInline->text() );
        p.insert( QString("max-inline" ), ui->leMaxInline->text() );
        p.insert( QString("min-crossline" ), ui->leMinCrossline->text() );
        p.insert( QString("max-crossline" ), ui->leMaxCrossline->text() );
        p.insert( QString("min-time" ), ui->leMinTime->text() );
        p.insert( QString("max-time" ), ui->leMaxTime->text() );
    }

    return p;
}


void ExportVolumeDialog::updateOkButton(){

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

    QPalette timePalette;
    if(ui->leMinTime->text().toDouble()>ui->leMaxTime->text().toDouble()){

        ok=false;
        timePalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinTime->setPalette(timePalette);
    ui->leMaxTime->setPalette(timePalette);


    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
