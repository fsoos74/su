#include "volumenavigationdialog.h"
#include "ui_volumenavigationdialog.h"

#include<QDoubleValidator>
#include <QKeyEvent>

#include <QVector3D>

VolumeNavigationDialog::VolumeNavigationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeNavigationDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->lePositionX->setValidator(validator);
    ui->lePositionY->setValidator(validator);
    ui->lePositionZ->setValidator(validator);

    ui->leRotationX->setValidator(validator);
    ui->leRotationY->setValidator(validator);
    ui->leRotationZ->setValidator(validator);

    ui->leScaleX->setValidator(validator);
    ui->leScaleY->setValidator(validator);
    ui->leScaleZ->setValidator(validator);

    connect( ui->lePositionX, SIGNAL(returnPressed()), this, SLOT(positionEntered()));
    connect( ui->lePositionY, SIGNAL(returnPressed()), this, SLOT(positionEntered()));
    connect( ui->lePositionZ, SIGNAL(returnPressed()), this, SLOT(positionEntered()));

    connect( ui->leRotationX, SIGNAL(returnPressed()), this, SLOT(rotationEntered()));
    connect( ui->leRotationY, SIGNAL(returnPressed()), this, SLOT(rotationEntered()));
    connect( ui->leRotationZ, SIGNAL(returnPressed()), this, SLOT(rotationEntered()));

    connect( ui->leScaleX, SIGNAL(returnPressed()), this, SLOT(scaleEntered()));
    connect( ui->leScaleY, SIGNAL(returnPressed()), this, SLOT(scaleEntered()));
    connect( ui->leScaleZ, SIGNAL(returnPressed()), this, SLOT(scaleEntered()));
}


VolumeNavigationDialog::~VolumeNavigationDialog()
{
    delete ui;
}


QVector3D VolumeNavigationDialog::position(){

    return QVector3D(
                ui->lePositionX->text().toDouble(),
                ui->lePositionY->text().toDouble(),
                ui->lePositionZ->text().toDouble()
    );
}

QVector3D VolumeNavigationDialog::rotation(){

    return QVector3D(
                ui->leRotationX->text().toDouble(),
                ui->leRotationY->text().toDouble(),
                ui->leRotationZ->text().toDouble()
    );
}

QVector3D VolumeNavigationDialog::scale(){

    return QVector3D(
                ui->leScaleX->text().toDouble(),
                ui->leScaleY->text().toDouble(),
                ui->leScaleZ->text().toDouble()
    );
}


void VolumeNavigationDialog::setPosition(QVector3D p){

    if( p==position()) return;

    ui->lePositionX->setText(QString::number(p.x()));
    ui->lePositionY->setText(QString::number(p.y()));
    ui->lePositionZ->setText(QString::number(p.z()));

    emit positionChanged(position());
}

void VolumeNavigationDialog::setRotation(QVector3D p){

    if( p==rotation()) return;

    ui->leRotationX->setText(QString::number(p.x()));
    ui->leRotationY->setText(QString::number(p.y()));
    ui->leRotationZ->setText(QString::number(p.z()));

    emit rotationChanged(rotation());
}

void VolumeNavigationDialog::setScale(QVector3D p){

    if( p==scale()) return;

    ui->leScaleX->setText(QString::number(p.x()));
    ui->leScaleY->setText(QString::number(p.y()));
    ui->leScaleZ->setText(QString::number(p.z()));

    emit scaleChanged(scale());
}

void VolumeNavigationDialog::positionEntered(){

    emit positionChanged(position());
}

void VolumeNavigationDialog::rotationEntered(){

    emit rotationChanged(rotation());
}

void VolumeNavigationDialog::scaleEntered(){

    emit scaleChanged(scale());
}

void VolumeNavigationDialog::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}


