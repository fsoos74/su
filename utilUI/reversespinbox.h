#ifndef REVERSESPINBOX_H
#define REVERSESPINBOX_H

#include <QObject>
#include <QSpinBox>

class ReverseSpinBox : public QSpinBox
{
public:
    ReverseSpinBox(QWidget* parent=nullptr);

protected:
    virtual QString textFromValue(int)const;
    virtual int valueFromText(QString)const;
};

#endif // REVERSESPINBOX_H
