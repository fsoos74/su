#ifndef COLORBARCONFIGURATIONDIALOG_H
#define COLORBARCONFIGURATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ColorBarConfigurationDialog;
}

class ColorBarConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorBarConfigurationDialog(QWidget *parent = 0);
    ~ColorBarConfigurationDialog();

    std::pair<double, double> scaleRange()const;
    int scaleSteps()const;
    int precision()const;

public slots:

    void setScaleRange(std::pair<double,double>);
    void setScaleSteps( int );
    void setPrecision(int);

signals:

    void scaleRangeChanged( std::pair<double, double>);
    void scaleStepsChanged(int);
    void precisionChanged(int);

private slots:
    void applyRange();

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::ColorBarConfigurationDialog *ui;
};

#endif // COLORBARCONFIGURATIONDIALOG_H
