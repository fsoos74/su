#ifndef CROSSPLOTGRIDSINPUTDIALOG_H
#define CROSSPLOTGRIDSINPUTDIALOG_H

#include <QDialog>
#include <QStringList>
#include<QComboBox>

#include<avoproject.h>

namespace Ui {
class CrossplotGridsInputDialog;
}

class CrossplotGridsInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrossplotGridsInputDialog(QWidget *parent = 0);
    ~CrossplotGridsInputDialog();

    GridType xType();
    QString xName();

    GridType yType();
    QString yName();

    bool useHorizon();
    QString horizonName();

    bool useAttribute();
    GridType attributeType();
    QString attributeName();

public slots:
    void setHorizons(QStringList);
    void setAttributeGrids(QStringList);
    void setOtherGrids(QStringList);

private slots:
    void updateCombos();
    void updateXCombo();
    void updateYCombo();
    void updateHorizonCombo();
    void updateAttributeCombo();

private:
    Ui::CrossplotGridsInputDialog *ui;

    void setComboNames(GridType type, QComboBox* cb);

    QStringList horizons;
    QStringList attributeGrids;
    QStringList otherGrids;
};

#endif // CROSSPLOTGRIDSINPUTDIALOG_H
