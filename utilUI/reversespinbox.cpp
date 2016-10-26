#include "reversespinbox.h"

ReverseSpinBox::ReverseSpinBox(QWidget* parent) : QSpinBox(parent)
{

}


int ReverseSpinBox::valueFromText(QString text)const{

    int i=text.toInt();
    return -i;
}


QString ReverseSpinBox::textFromValue(int i)const{
    return QString::number(-i);
}
