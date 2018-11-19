#include "reversedoublespinbox.h"

ReverseDoubleSpinBox::ReverseDoubleSpinBox(QWidget* parent) : QDoubleSpinBox(parent)
{

}


void ReverseDoubleSpinBox::stepBy(int steps){

    int newValue= value() - steps * singleStep();

    if( newValue<minimum() ) newValue=minimum();
    if( newValue>maximum() ) newValue=maximum();

    setValue(newValue);
}

QAbstractSpinBox::StepEnabled ReverseDoubleSpinBox::stepEnabled() const{

    QFlags<StepEnabledFlag> flags;

    if( value()>minimum()) flags|=QAbstractSpinBox::StepUpEnabled;
    if( value()<maximum()) flags|=QAbstractSpinBox::StepDownEnabled;

    return flags;
}
