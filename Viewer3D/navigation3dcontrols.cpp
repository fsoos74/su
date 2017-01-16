#include "navigation3dcontrols.h"
#include "ui_navigation3dcontrols.h"

Navigation3DControls::Navigation3DControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Navigation3DControls)
{
    ui->setupUi(this);

    connect( ui->tbMoveLeft, SIGNAL(pressed()), this, SIGNAL(moveLeft()) );
    connect( ui->tbMoveRight, SIGNAL(pressed()), this, SIGNAL(moveRight()) );
    connect( ui->tbMoveUp, SIGNAL(pressed()), this, SIGNAL(moveUp()) );
    connect( ui->tbMoveDown, SIGNAL(pressed()), this, SIGNAL(moveDown()) );
    connect( ui->tbMoveBack, SIGNAL(pressed()), this, SIGNAL(moveBack()) );
    connect( ui->tbMoveFront, SIGNAL(pressed()), this, SIGNAL(moveFront()) );

    connect( ui->tbRotateXNeg, SIGNAL(pressed()), this, SIGNAL(rotateXNeg()) );
    connect( ui->tbRotateXPos, SIGNAL(pressed()), this, SIGNAL(rotateXPos()) );
    connect( ui->tbRotateYNeg, SIGNAL(pressed()), this, SIGNAL(rotateYNeg()) );
    connect( ui->tbRotateYPos, SIGNAL(pressed()), this, SIGNAL(rotateYPos()) );
    connect( ui->tbRotateZNeg, SIGNAL(pressed()), this, SIGNAL(rotateZNeg()) );
    connect( ui->tbRotateZPos, SIGNAL(pressed()), this, SIGNAL(rotateZPos()) );

    connect( ui->tbScaleXNeg, SIGNAL(pressed()), this, SIGNAL(scaleXNeg()) );
    connect( ui->tbScaleXPos, SIGNAL(pressed()), this, SIGNAL(scaleXPos()) );
    connect( ui->tbScaleYNeg, SIGNAL(pressed()), this, SIGNAL(scaleYNeg()) );
    connect( ui->tbScaleYPos, SIGNAL(pressed()), this, SIGNAL(scaleYPos()) );
    connect( ui->tbScaleZNeg, SIGNAL(pressed()), this, SIGNAL(scaleZNeg()) );
    connect( ui->tbScaleZPos, SIGNAL(pressed()), this, SIGNAL(scaleZPos()) );
}

Navigation3DControls::~Navigation3DControls()
{
    delete ui;
}
