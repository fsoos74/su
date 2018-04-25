#ifndef FMCDP2DDIALOG_H
#define FMCDP2DDIALOG_H

#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class FMCDP2DDialog;
}

class FMCDP2DDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit FMCDP2DDialog(QWidget *parent = 0);
    ~FMCDP2DDialog();

    QMap<QString,QString> params();

    QString wavelet();
    double f0();
    double delay();
    double offset1();
    double offset2();
    bool isSplitSpread();
    double dxz();
    unsigned nrc();
    double tmax();
    QString dataset();
    QList<std::pair<double,double>> layers();

public slots:
    void setProject( AVOProject* project);
    void setWavelet(QString);
    void setF0(double);
    void setDelay(double);
    void setOffset1(double);
    void setOffset2(double);
    void setSplitSpread(bool);
    void setDXZ(double);
    void setNRC(unsigned);
    void setTMax(double);

private slots:
    void updateOkButton();
    void setLayerCount(int);


private:
    Ui::FMCDP2DDialog *ui;

    AVOProject* m_project=nullptr;      // parent must own project and not delete before dialog is finished!!!!
};


#endif // FMCDP2DDIALOG_H
