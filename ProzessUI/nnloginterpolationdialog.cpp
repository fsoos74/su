#include "nnloginterpolationdialog.h"
#include "ui_nnloginterpolationdialog.h"

#include<QPushButton>
#include<QFileDialog>

//#include<xnlreader.h>
#include<xmlpreader.h>

NNLogInterpolationDialog::NNLogInterpolationDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::NNLogInterpolationDialog)
{
    ui->setupUi(this);

    connect( ui->leInputFile, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->lwWells->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

NNLogInterpolationDialog::~NNLogInterpolationDialog()
{
    delete ui;
}


QMap<QString,QString> NNLogInterpolationDialog::params(){

    QMap<QString, QString> p;

    p.insert( "input-file", ui->leInputFile->text());
    //p.insert( "well", ui->cbWell->currentText() );
    auto wells=selectedWells();
    p.insert("wells", packParamList(wells));
    p.insert( "output-log", ui->leOutput->text());

    return p;
}



void NNLogInterpolationDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;
}

void NNLogInterpolationDialog::updateInputWells(QStringList requiredLogs){

    ui->lwWells->clear();

    if( !m_project ) return;

    auto wells=m_project->wellList();

    for( auto well : wells){

        int nok=0;
        for( auto log : requiredLogs){
            if(!m_project->existsLog(well, log)) break;
            nok++;
        }
        if(nok==requiredLogs.size()) ui->lwWells->addItem(well);
    }

    updateOkButton();
}

void NNLogInterpolationDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getOpenFileName(0, "Select nn param file");

    if( fn.isNull()) return;        // cancelled

    ui->leInputFile->setText(fn);
}


void NNLogInterpolationDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;
    auto iname=ui->leInputFile->text();
    auto oname=ui->leOutput->text();

    if( iname.isEmpty() ) ok=false;

    if( selectedWells().isEmpty()) ok=false;

    if( oname.isEmpty()){
        ok=false;
    }
    else {
       QPalette palette;
       for( auto well : selectedWells()){
            if( m_project->existsLog(well, oname) ){
               ok=false;
               palette.setColor(QPalette::Text, Qt::red);
               break;
            }
       }
       ui->leOutput->setPalette(palette);
    }


    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}


void NNLogInterpolationDialog::on_leInputFile_textChanged(const QString & name)
{
    QFile f(name);

    ui->teInfo->clear();
    ui->lwWells->clear();

   if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    SimpleMLP mlp;
    QStringList inames;
    int ilaper;
    int xlaper;
    XMLPReader reader(mlp,inames,ilaper,xlaper);
    if (!reader.read(&f)) {
        return;
    }

    ui->teInfo->clear();
    QString text;
    text+="Required Logs:\n";
    text+=inames.join("\n");
    ui->teInfo->appendPlainText(text);

    updateInputWells(inames);
}


QStringList NNLogInterpolationDialog::selectedWells(){

    QStringList l;
    auto ids = ui->lwWells->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        l<<ui->lwWells->item(idx.row())->text();
    }
    return l;
}
