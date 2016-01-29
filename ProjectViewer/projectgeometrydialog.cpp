#include "projectgeometrydialog.h"
#include "ui_projectgeometrydialog.h"

#include<QIntValidator>
#include<QDoubleValidator>
#include<QPushButton>
#include<QInputDialog>

#include<seismicdatasetreader.h>
#include<QMessageBox>
#include<memory>

ProjectGeometryDialog::ProjectGeometryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProjectGeometryDialog)
{
    ui->setupUi(this);

    QIntValidator* ivalidator=new QIntValidator(this);
    ui->leIline1->setValidator(ivalidator);
    ui->leXline1->setValidator(ivalidator);
    ui->leIline2->setValidator(ivalidator);
    ui->leXline2->setValidator(ivalidator);
    ui->leIline3->setValidator(ivalidator);
    ui->leXline3->setValidator(ivalidator);

    QDoubleValidator* dvalidator=new QDoubleValidator(this);
    ui->leX1->setValidator(dvalidator);
    ui->leY1->setValidator(dvalidator);
    ui->leX2->setValidator(dvalidator);
    ui->leY2->setValidator(dvalidator);
    ui->leX3->setValidator(dvalidator);
    ui->leY3->setValidator(dvalidator);

    connect(ui->leIline1, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leIline2, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leIline3, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    connect(ui->leXline1, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leXline2, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leXline3, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    connect(ui->leX1, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leX2, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leX3, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    connect(ui->leY1, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leY2, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leY3, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

ProjectGeometryDialog::~ProjectGeometryDialog()
{
    delete ui;
}

ProjectGeometry ProjectGeometryDialog::projectGeometry(){

    ProjectGeometry geom;

    geom.setInlineAndCrossline( 0, QPoint(ui->leIline1->text().toInt(), ui->leXline1->text().toInt()));
    geom.setInlineAndCrossline( 1, QPoint(ui->leIline2->text().toInt(), ui->leXline2->text().toInt()));
    geom.setInlineAndCrossline( 2, QPoint(ui->leIline3->text().toInt(), ui->leXline3->text().toInt()));

    geom.setCoordinates( 0, QPoint(ui->leX1->text().toDouble(), ui->leY1->text().toDouble()));
    geom.setCoordinates( 1, QPoint(ui->leX2->text().toDouble(), ui->leY2->text().toDouble()));
    geom.setCoordinates( 2, QPoint(ui->leX3->text().toDouble(), ui->leY3->text().toDouble()));

    return geom;
}

void ProjectGeometryDialog::setProjectGeometry( ProjectGeometry geom){

    ui->leIline1->setText(QString::number(geom.inlineAndCrossline(0).x()));
    ui->leXline1->setText(QString::number(geom.inlineAndCrossline(0).y()));
    ui->leIline2->setText(QString::number(geom.inlineAndCrossline(1).x()));
    ui->leXline2->setText(QString::number(geom.inlineAndCrossline(1).y()));
    ui->leIline3->setText(QString::number(geom.inlineAndCrossline(2).x()));
    ui->leXline3->setText(QString::number(geom.inlineAndCrossline(2).y()));

    ui->leX1->setText(QString::number(geom.coordinates(0).x()));
    ui->leY1->setText(QString::number(geom.coordinates(0).y()));
    ui->leX2->setText(QString::number(geom.coordinates(1).x()));
    ui->leY2->setText(QString::number(geom.coordinates(1).y()));
    ui->leX3->setText(QString::number(geom.coordinates(2).x()));
    ui->leY3->setText(QString::number(geom.coordinates(2).y()));
}

void ProjectGeometryDialog::setProject(std::shared_ptr<AVOProject> prj){

    m_project=prj;
    if( m_project){
        setProjectGeometry(m_project->geometry());
    }
}

void ProjectGeometryDialog::updateOkButton(){

    ProjectGeometry geom=projectGeometry();

    bool enabled=isValid(geom);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabled);
}

void ProjectGeometryDialog::on_pbExtract_clicked()
{
    bool ok;
    QString name = QInputDialog::getItem(this, tr("Select Seismic Dataset"),
                                         tr("Select Dataset:"), m_project->seismicDatasetList(), 0, false, &ok);
    if (!ok || name.isEmpty()){
        return;
    }

    std::shared_ptr<SeismicDatasetReader> reader=m_project->openSeismicDataset(name);

    if( !reader ){
        QMessageBox::critical( this, "Extract Geometry", QString("Open dataset %1 failed!").arg(name));
        return;
    }


    ProjectGeometry geom;

    if( !reader->extractGeometry(geom)){
        QMessageBox::critical( this, "Extract Geometry", QString("Extracting from dataset %1 failed!").arg(name));
    }

    setProjectGeometry(geom);

}
