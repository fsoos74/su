#ifndef AMPLITUDECURVEDATASELECTIONDIALOG_H
#define AMPLITUDECURVEDATASELECTIONDIALOG_H

#include <QDialog>

#include "amplitudecurvedefinition.h"

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
    int inlineNumber();
    int crosslineNumber();
    double time();
    double minimumOffset();
    double maximumOffset();
    double minimumAzimuth();
    double maximumAzimuth();
    int inlineSize();
    int crosslineSize();
    double depth();

public slots:

    void setDatasetNames( const QStringList&);
    void setMinimumOffset( const double&);
    void setMaximumOffset( const double&);
    void setMinimumAzimuth( const double);
    void setMaximumAzimuth( const double);
    void setInlineNumber(int);
    void setCrosslineNumber(int);
    void setTime( double );
    void setDepth( double );

signals:

    void curveDataSelected( AmplitudeCurveDefinition);

private slots:
    void on_pbAdd_clicked();

private:
    Ui::AmplitudeCurveDataSelectionDialog *ui;
};

#endif // AMPLITUDECURVEDATASELECTIONDIALOG_H
