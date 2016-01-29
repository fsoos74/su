#ifndef ISOLINEDIALOG_H
#define ISOLINEDIALOG_H

#include <QDialog>
#include<QColor>
#include<QVector>

namespace Ui {
class IsoLineDialog;
}



class IsoLineDialog : public QDialog
{
    Q_OBJECT

public:

    enum ContourMode{ MODE_NO_CONTOURS, MODE_DEFAULT, MODE_USER_DEFINED };

    const int MAX_STEPS=100;

    IsoLineDialog(QWidget* parent=nullptr );
    ~IsoLineDialog();

    int lineWidth()const;

    const QColor& lineColor()const;

    QVector<double> contours();

    double first()const;

    double last()const;

    double steps()const;

    ContourMode contourMode()const;

public slots:

    void setColorBarSteps( const QVector<double>& );
    void setContourMode( ContourMode );
    void setFirst( double );
    void setLast( double);
    void setSteps( int );
    void setUserDefinedParams( double first, double last, int steps);
    void setLineWidth(int);
    void setLineColor(const QColor&);
    void computeContours();

signals:

    void contourModeChanged(ContourMode);
    void firstChanged(double);
    void lastChanged(double);
    void stepsChanged( int );
    void contoursChanged( const QVector<double>&);
    void lineWidthChanged(int);
    void lineColorChanged(const QColor&);

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);

private slots:


    void on_cbLineColor_clicked();

    void on_sbLineWidth_valueChanged(int arg1);

    //void on_pbApply_clicked();

    void on_pbClose_clicked();

private:

    void mode2Buttons( ContourMode);

    Ui::IsoLineDialog *ui;


    QVector<double> colorBarSteps;
    QVector<double> m_contours;

};

#endif // ISOLINEDIALOG_H
