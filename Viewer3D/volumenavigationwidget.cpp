#include "volumenavigationwidget.h"
#include "ui_volumenavigationwidget.h"

#include<QDoubleValidator>
#include <QKeyEvent>


VolumeNavigationWidget::VolumeNavigationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumeNavigationWidget)
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

VolumeNavigationWidget::~VolumeNavigationWidget()
{
    delete ui;
}


QVector3D VolumeNavigationWidget::position(){

    return QVector3D(
                ui->lePositionX->text().toDouble(),
                ui->lePositionY->text().toDouble(),
                ui->lePositionZ->text().toDouble()
    );
}

QVector3D VolumeNavigationWidget::rotation(){

    return QVector3D(
                ui->leRotationX->text().toDouble(),
                ui->leRotationY->text().toDouble(),
                ui->leRotationZ->text().toDouble()
    );
}

QVector3D VolumeNavigationWidget::scale(){

    return QVector3D(
                ui->leScaleX->text().toDouble(),
                ui->leScaleY->text().toDouble(),
                ui->leScaleZ->text().toDouble()
    );
}


void VolumeNavigationWidget::setPosition(QVector3D p){

    if( p==position()) return;

    ui->lePositionX->setText(QString::number(p.x()));
    ui->lePositionY->setText(QString::number(p.y()));
    ui->lePositionZ->setText(QString::number(p.z()));

    emit positionChanged(position());
}

void VolumeNavigationWidget::setRotation(QVector3D p){

    if( p==rotation()) return;

    ui->leRotationX->setText(QString::number(p.x()));
    ui->leRotationY->setText(QString::number(p.y()));
    ui->leRotationZ->setText(QString::number(p.z()));

    emit rotationChanged(rotation());
}

void VolumeNavigationWidget::setScale(QVector3D p){

    if( p==scale()) return;

    ui->leScaleX->setText(QString::number(p.x()));
    ui->leScaleY->setText(QString::number(p.y()));
    ui->leScaleZ->setText(QString::number(p.z()));

    emit scaleChanged(scale());
}

void VolumeNavigationWidget::positionEntered(){

    emit positionChanged(position());
}

void VolumeNavigationWidget::rotationEntered(){

    emit rotationChanged(rotation());
}

void VolumeNavigationWidget::scaleEntered(){

    emit scaleChanged(scale());
}

void VolumeNavigationWidget::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}


