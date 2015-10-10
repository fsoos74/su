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
    explicit DisplayRangeDialog(std::pair<double, double> range, QWidget *parent = 0);
    ~DisplayRangeDialog();

    std::pair<double,double> range()const{
        return m_range;
    }

public slots:
    void setRange(std::pair<double,double>);

signals:
    void displayRangeChanged( std::pair<double, double>);


private slots:
    void on_pbMin_clicked();

    void on_pbMax_clicked();

    void on_pbApply_clicked();

    void on_pbOk_clicked();

private:

    void updateMinControl();
    void updateMaxControl();
    void updateMinFromControl();
    void updateMaxFromControl();

    Ui::DisplayRangeDialog *ui;

    QDoubleValidator* m_validator;

    std::pair<double,double > m_startRange;
    std::pair<double, double> m_range;
};

#endif // DISPLAYRANGEDIALOG_H
