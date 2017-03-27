#ifndef GRIDCOLORCOMPOSITEINPUTDIALOG_H
#define GRIDCOLORCOMPOSITEINPUTDIALOG_H

#include <QDialog>
#include <QStringList>
#include<QComboBox>

#include<avoproject.h>

namespace Ui {
class GridColorCompositeInputDialog;
}

class GridColorCompositeInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridColorCompositeInputDialog(QWidget *parent = 0);
    ~GridColorCompositeInputDialog();

    GridType redType();
    QString redName();

    GridType greenType();
    QString greenName();

    GridType blueType();
    QString blueName();

public slots:
    void setHorizons(QStringList);
    void setAttributeGrids(QStringList);
    void setOtherGrids(QStringList);

private slots:
    void updateCombos();
    void updateRedCombo();
    void updateGreenCombo();
    void updateBlueCombo();


private:
    Ui::GridColorCompositeInputDialog *ui;

    void setComboNames(GridType type, QComboBox* cb);

    QStringList horizons;
    QStringList attributeGrids;
    QStringList otherGrids;
};

#endif // GRIDCOLORCOMPOSITEINPUTDIALOG_H
