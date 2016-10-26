#ifndef REVERSESPINBOX_H
#define REVERSESPINBOX_H

#include <QObject>
#include <QSpinBox>


class ReverseSpinBox : public QSpinBox
{
public:
    ReverseSpinBox(QWidget* parent=nullptr);

protected:
    virtual void stepBy(int steps);
    virtual StepEnabled stepEnabled() const;
};

#endif // REVERSESPINBOX_H
