#ifndef GRIDDISPLAYOPTIONSDIALOG_H
#define GRIDDISPLAYOPTIONSDIALOG_H

#include <QDialog>
#include <QColor>

namespace Ui {
class GridDisplayOptionsDialog;
}

class GridDisplayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridDisplayOptionsDialog(QWidget *parent = 0);
    ~GridDisplayOptionsDialog();

    Qt::TransformationMode transformationMode()const;
    QColor backgroundColor()const;
    int highlightedCDPSize()const;
    QColor highlightedCDPColor()const;

signals:

    void transformationModeChanged( Qt::TransformationMode );
    void backgroundColorChanged( QColor );
    void highlightedCDPSizeChanged(int);
    void highlightedCDPColorChanged( QColor );


public slots:

    void setTransformationMode( Qt::TransformationMode );
    void setBackgroundColor( QColor );
    void setHighlightedCDPSize(int);
    void setHighlightedCDPColor(QColor);

private slots:

    void on_cbBackgroundColor_clicked();

    void on_cbQuality_currentIndexChanged(int index);

    void on_cbHighlightedCDPs_clicked();

    void on_sbHighlightedCDPs_valueChanged(int arg1);

private:
    Ui::GridDisplayOptionsDialog *ui;
};

#endif // GRIDDISPLAYOPTIONSDIALOG_H
