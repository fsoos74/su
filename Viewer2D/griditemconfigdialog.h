#ifndef GRIDITEMCONFIGDIALOG_H
#define GRIDITEMCONFIGDIALOG_H

#include <QDialog>
#include <QVector>
#include <QRgb>

#include<griditem.h>
#include<colortable.h>
#include<histogram.h>

namespace Ui {
class GridItemConfigDialog;
}

class GridItemConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridItemConfigDialog(QWidget *parent = 0);
    ~GridItemConfigDialog();

    ColorTable* colorTable();
    Histogram histogram();
    bool showMesh();
    bool showLabels();
    bool showLabel();
    QString labelText();
    int inlineIncrement();
    int crosslineIncrement();
    int zValue();

signals:
    void showMeshChanged(bool);
    void showLabelsChanged(bool);
    void showLabelChanged(bool);
    void labelTextChanged(QString);
    void inlineIncrementChanged(int);
    void crosslineIncrementChanged(int);
    void zValueChanged(int);

public slots:

    void setColorTable(ColorTable*);
    void setHistogram(Histogram);
    void setShowMesh(bool);
    void setShowLabels(bool);
    void setShowLabel(bool);
    void setLabelText(QString);
    void setInlineIncrement(int);
    void setCrosslineIncrement(int);
    void setZValue(int);


private slots:
    void on_pbColortable_clicked();
    void on_pbScaling_clicked();

private:
    Ui::GridItemConfigDialog *ui;

    ColorTable* m_colorTable=nullptr;
    Histogram m_histogram;
};

#endif // GRIDITEMCONFIGDIALOG_H
