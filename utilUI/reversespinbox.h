#ifndef REVERSESPINBOX_H
#define REVERSESPINBOX_H

#include <QObject>
#include <QSpinBox>


class ReverseSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    ReverseSpinBox(QWidget* parent=nullptr);
    bool isReverse()const{
        return m_reverse;
    }

public slots:
    void setReverse(bool);

signals:
    void reverseChanged(bool);

protected:
    virtual void stepBy(int steps);
    virtual StepEnabled stepEnabled() const;

private:
    bool m_reverse=true;
};

#endif // REVERSESPINBOX_H
