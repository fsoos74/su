#ifndef WELLITEMCONFIGDIALOG_H
#define WELLITEMCONFIGDIALOG_H

#include <QDialog>

#include <wellitem.h>

namespace Ui {
class WellItemConfigDialog;
}

class WellItemConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WellItemConfigDialog(QWidget *parent = 0);
    ~WellItemConfigDialog();

    int labelType();
    int labelSize();
    QColor symbolColor();
    QColor pathColor();
    int itemSize();
    int zValue();

    //static bool configItem( WellItem* item );

signals:
    void zValueChanged(int);

public slots:

    void setLabelType(int);
    void setLabelSize(int);
    void setSymbolColor(QColor);
    void setPathColor(QColor);
    void setItemSize(int);
    void setZValue(int);

private slots:
    void on_cbSymbolColor_clicked();
    void on_cbPathColor_clicked();

private:
    Ui::WellItemConfigDialog *ui;
};

#endif // WELLITEMCONFIGDIALOG_H
