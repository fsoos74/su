#include "creategriddialog.h"
#include "ui_creategriddialog.h"

#include<QIntValidator>
#include<QPushButton>
#include <gridtype.h>


CreateGridDialog::CreateGridDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::CreateGridDialog)
{
    ui->setupUi(this);

    QIntValidator* ivalidator=new QIntValidator(this);
    ui->leMinIline->setValidator(ivalidator);
    ui->leMaxIline->setValidator(ivalidator);
    ui->leMinXline->setValidator(ivalidator);
    ui->leMaxXline->setValidator(ivalidator);

    connect(ui->leMinIline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxIline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMinXline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxXline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    QStringList gtypes;
    gtypes<<toQString(GridType::Horizon);
    gtypes<<toQString(GridType::Attribute);
    gtypes<<toQString(GridType::Other);
    ui->cbType->addItems(gtypes);

    updateOkButton();
}

CreateGridDialog::~CreateGridDialog()
{
    delete ui;
}


void CreateGridDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    if( m_project){
        auto geom=m_project->geometry();
        ui->leMinIline->setText(QString::number(geom.bboxLines().left()));
        ui->leMaxIline->setText(QString::number(geom.bboxLines().right()));
        ui->leMinXline->setText(QString::number(geom.bboxLines().top()));
        ui->leMaxXline->setText(QString::number(geom.bboxLines().bottom()));
    }
}

QMap<QString, QString> CreateGridDialog::params(){

    QMap<QString, QString> p;

    p.insert( "type", ui->cbType->currentText());
    p.insert( QString("name"), ui->leName->text());
    p.insert( QString("min-iline"), ui->leMinIline->text());
    p.insert( QString("max-iline"), ui->leMaxIline->text());
    p.insert( QString("min-xline"), ui->leMinXline->text());
    p.insert( QString("max-xline"), ui->leMaxXline->text());

    return p;
}

void CreateGridDialog::updateOkButton(){

    bool ok=true;

    // test name
    auto otype=toGridType(ui->cbType->currentText());
    auto oname=ui->leName->text();

    {
    QPalette palette;
    if( oname.isEmpty() || m_project->existsGrid(otype, oname) ){    // make sure not to call existsgrid with empty name!!!
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leName->setPalette(palette);
    }

    // test inline range
    {
    QPalette palette;
    if( ui->leMinIline->text().toInt() >= ui->leMaxIline->text().toInt() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinIline->setPalette(palette);
    ui->leMaxIline->setPalette(palette);
    }

    // test crossline range
    {
    QPalette palette;
    if( ui->leMinXline->text().toInt() >=ui->leMaxXline->text().toInt() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinXline->setPalette(palette);
    ui->leMaxXline->setPalette(palette);
    }

    ui->buttonBox->button( QDialogButtonBox::Ok)->setEnabled(ok);
}

