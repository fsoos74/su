#include "reversespinbox.h"

ReverseSpinBox::ReverseSpinBox(QWidget* parent) : QSpinBox(parent)
{

}


void ReverseSpinBox::stepBy(int steps){

    int newValue= value() - steps * singleStep();

    if( newValue<minimum() ) newValue=minimum();
    if( newValue>maximum() ) newValue=maximum();

    setValue(newValue);
}

QAbstractSpinBox::StepEnabled ReverseSpinBox::stepEnabled() const{

    QFlags<StepEnabledFlag> flags;

    if( value()>minimum()) flags|=QAbstractSpinBox::StepUpEnabled;
    if( value()<maximum()) flags|=QAbstractSpinBox::StepDownEnabled;

    return flags;
}
