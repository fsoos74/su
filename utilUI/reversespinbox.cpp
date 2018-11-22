#include "reversespinbox.h"

ReverseSpinBox::ReverseSpinBox(QWidget* parent) : QSpinBox(parent)
{

}

void ReverseSpinBox::setReverse(bool on){
    if( on==m_reverse ) return;
    m_reverse=on;
    emit reverseChanged(on);
}

void ReverseSpinBox::stepBy(int steps){

    if( !m_reverse){
        QSpinBox::stepBy(steps);
        return;
    }

    int newValue= value() - steps * singleStep();

    if( newValue<minimum() ) newValue=minimum();
    if( newValue>maximum() ) newValue=maximum();

    setValue(newValue);
}

QAbstractSpinBox::StepEnabled ReverseSpinBox::stepEnabled() const{

    if( !m_reverse ) return QSpinBox::stepEnabled();

    QFlags<StepEnabledFlag> flags;

    if( value()>minimum()) flags|=QAbstractSpinBox::StepUpEnabled;
    if( value()<maximum()) flags|=QAbstractSpinBox::StepDownEnabled;

    return flags;
}
