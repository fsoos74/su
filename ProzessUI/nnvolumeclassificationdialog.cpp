#include "nnvolumeclassificationdialog.h"
#include "ui_nnvolumeclassificationdialog.h"

#include<QPushButton>
#include<QFileDialog>

#include<xnlreader.h>


NNVolumeClassificationDialog::NNVolumeClassificationDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::NNVolumeClassificationDialog)
{
    ui->setupUi(this);

    connect( ui->leInputFile, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

NNVolumeClassificationDialog::~NNVolumeClassificationDialog()
{
    delete ui;
}


QMap<QString,QString> NNVolumeClassificationDialog::params(){

    QMap<QString, QString> p;

    p.insert( "input-file", ui->leInputFile->text());
    p.insert( "output-volume", ui->leOutput->text());

    return p;
}



void NNVolumeClassificationDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;
}


void NNVolumeClassificationDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getOpenFileName(0, "Select nn param file");

    if( fn.isNull()) return;        // cancelled

    ui->leInputFile->setText(fn);
}


void NNVolumeClassificationDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;
    auto iname=ui->leInputFile->text();
    auto oname=ui->leOutput->text();

    if( iname.isEmpty() ) ok=false;

    QPalette palette;
    if( oname.isEmpty() || m_project->volumeList().contains(oname)){
           ok=false;
           palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}


void NNVolumeClassificationDialog::on_leInputFile_textChanged(const QString & name)
{
    QFile f(name);

    ui->teInfo->clear();

   if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    NN nn;
    QStringList inames;
    std::vector<std::pair<double,double>> Xmm;
    std::pair<double,double> Ymm;
    XNLReader reader(nn,inames, Xmm, Ymm);
    if (!reader.read(&f)) {
        return;
    }

    ui->teInfo->clear();
    QString text;
    text+="Required volumes:\n";
    text+=inames.join("\n");
    ui->teInfo->appendPlainText(text);
}


