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

QToolButton* MouseModeSelector::zoomButton(){
    return ui->tbZoom;
}

QToolButton* MouseModeSelector::selectButton(){
    return ui->tbSelect;
}

MouseMode MouseModeSelector::mode(){

    if( ui->tbExplore->isChecked() ){
        return MouseMode::Explore;
    }
    else if( ui->tbSelect->isChecked()){
        return MouseMode::Select;
    }
    else if( ui->tbZoom->isChecked()){
        return  MouseMode::Zoom;
    }
    else if( ui->tbPick->isChecked()){
        return MouseMode::Pick;
    }
    else{
        return MouseMode::DeletePick;
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
    case MouseMode::Pick:
        ui->tbPick->setChecked(true);
        break;
    case MouseMode::DeletePick:
        ui->tbDeletePick->setChecked(true);
        break;
    }
}

void MouseModeSelector::onButtonBox(){

    emit modeChanged( mode() );

}
