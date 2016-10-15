#ifndef DISPLAYRANGEDIALOG_H
#define DISPLAYRANGEDIALOG_H

#include <QDialog>


namespace Ui {
class DisplayRangeDialog;
}


class QDoubleValidator;

class DisplayRangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayRangeDialog(QWidget *parent = 0);
    ~DisplayRangeDialog();

    std::pair<double, double> range();

    double power();

    bool isLocked();

public slots:

    void setRange(std::pair<double,double>);
    void setPower( double );
    void setLocked(bool);

signals:

    void rangeChanged( std::pair<double, double>);
    void powerChanged(double);
    void lockedChanged(bool);

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);


private slots:

    void applyRange();
    void applyPower();

private:


    Ui::DisplayRangeDialog *ui;

};

#endif // DISPLAYRANGEDIALOG_H
