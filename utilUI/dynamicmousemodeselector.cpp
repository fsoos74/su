#include "dynamicmousemodeselector.h"

#include<QHBoxLayout>

#include<iostream>

DynamicMouseModeSelector::DynamicMouseModeSelector(QWidget *parent) : QWidget(parent)
{
    layout=new QHBoxLayout;
    setLayout(layout);

    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);

    connect( buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onButtonBox()));
}


MouseMode DynamicMouseModeSelector::mode()const{

    foreach( MouseMode m, buttons.keys() ){
        if( buttons.value(m)->isChecked() ) return m;
    }

    return MouseMode::Explore;      // need to raise exception, this chould never happen
}

QToolButton* DynamicMouseModeSelector::button(MouseMode m)const{
    return buttons.value( m, nullptr);
}

void DynamicMouseModeSelector::addMode(MouseMode m){

    if( buttons.contains(m)) return;

    QToolButton* button = new QToolButton;
    button->setIcon(QIcon(modePixmap(m)));
    button->setCheckable(true);
    button->setMinimumSize(24,24);
    buttons.insert(m, button);
    layout->addWidget(button);
    buttonGroup->addButton(button);

    button->setChecked(buttons.size()==1);  // first button (and only button) is checked
}

void DynamicMouseModeSelector::removeMode(MouseMode m){

    if( !buttons.contains(m)) return;

    QToolButton* button=buttons.value(m);
    buttonGroup->removeButton(button);
    delete button;
    buttons.remove(m);
};

void DynamicMouseModeSelector::setMode(MouseMode m){

    if( !buttons.contains(m)) return;

    buttons.value(m)->setChecked(true);
}

void DynamicMouseModeSelector::onButtonBox(){

    MouseMode m = mode();

    emit modeChanged(m);
}
