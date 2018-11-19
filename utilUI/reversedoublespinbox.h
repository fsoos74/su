#ifndef REVERSEDOUBLESPINBOX_H
#define REVERSEDOUBLESPINBOX_H

#include <QObject>
#include <QDoubleSpinBox>


class ReverseDoubleSpinBox : public QDoubleSpinBox
{
public:
    ReverseDoubleSpinBox(QWidget* parent=nullptr);

protected:
    virtual void stepBy(int steps);
    virtual StepEnabled stepEnabled() const;
};

#endif // REVERSEDOUBLESPINBOX_H
