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
    int inlineIncrement();
    int crosslineIncrement();

signals:
    void showMeshChanged(bool);
    void showLabelsChanged(bool);
    void inlineIncrementChanged(int);
    void crosslineIncrementChanged(int);

public slots:

    void setColorTable(ColorTable*);
    void setHistogram(Histogram);
    void setShowMesh(bool);
    void setShowLabels(bool);
    void setInlineIncrement(int);
    void setCrosslineIncrement(int);



private slots:
    void on_pbColortable_clicked();
    void on_pbScaling_clicked();

private:
    Ui::GridItemConfigDialog *ui;

    ColorTable* m_colorTable=nullptr;
    Histogram m_histogram;
};

#endif // GRIDITEMCONFIGDIALOG_H
