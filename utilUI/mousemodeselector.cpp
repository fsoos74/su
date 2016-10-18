#include "mousemodeselector.h"
#include "ui_mousemodeselector.h"

MouseModeSelector::MouseModeSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MouseModeSelector)
{
    ui->setupUi(this);

    connect( ui->buttonGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(onButtonBox()) );
}

MouseModeSelector::~MouseModeSelector()
{
    delete ui;
}

MouseMode MouseModeSelector::mode(){

    if( ui->tbExplore->isChecked() ){
        return MouseMode::Explore;
    }
    else if( ui->tbSelect->isChecked()){
        return MouseMode::Select;
    }
    else{
        return  MouseMode::Zoom;
    }
}

void MouseModeSelector::setMode(MouseMode m){

    // signals emitted when toggled by buttongroup

    switch(m){

    case MouseMode::Explore:
        ui->tbExplore->setChecked(true);
        break;
    case MouseMode::Select:
        ui->tbSelect->setChecked(true);
        break;
    case MouseMode::Zoom:
        ui->tbZoom->setChecked(true);
        break;

    }
}

void MouseModeSelector::onButtonBox(){

    emit modeChanged( mode() );

}
