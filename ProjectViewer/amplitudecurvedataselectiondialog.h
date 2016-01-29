#ifndef AMPLITUDECURVEDATASELECTIONDIALOG_H
#define AMPLITUDECURVEDATASELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class AmplitudeCurveDataSelectionDialog;
}

class AmplitudeCurveDataSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AmplitudeCurveDataSelectionDialog(QWidget *parent = 0);
    ~AmplitudeCurveDataSelectionDialog();

    QString datasetName();
    QString horizonName();
    int inlineNumber();
    int crosslineNumber();
    QString reductionMethod();
    int numberOfSamples();
    double maximumOffset();
    int inlineSize();
    int crosslineSize();
    double depth();

public slots:

    void setDatasetNames( const QStringList&);
    void setHorizonNames( const QStringList&);
    void setMaximumOffset( const double&);
    void setReductionMethods( const QStringList&);
    void setNumberOfSamples( int );
    void setInlineNumber(int);
    void setCrosslineNumber(int);
    void setDepth( double );

signals:

    void curveDataSelected( QString datasetName, QString HorizonName, int inlineNumber, int crosslineNumber,
                            int inlineSize, int crosslineSize, double maximumOffset, double depth,
                            QString reductionMethodName, int windowSize);

private slots:
    void on_pbAdd_clicked();

private:
    Ui::AmplitudeCurveDataSelectionDialog *ui;
};

#endif // AMPLITUDECURVEDATASELECTIONDIALOG_H
