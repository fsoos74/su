#ifndef GRIDSELECTDIALOG_H
#define GRIDSELECTDIALOG_H

#include <QDialog>

#include<avoproject.h>

namespace Ui {
class GridSelectDialog;
}

class GridSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridSelectDialog(QWidget *parent = 0);
    ~GridSelectDialog();

    void setProject( const AVOProject& project);

    GridType type();
    QString name();

private slots:
    void updateNames();
    void updateOkButton();

private:
    Ui::GridSelectDialog *ui;

    QStringList m_horizonGrids;
    QStringList m_attributeGrids;
    QStringList m_otherGrids;

    GridType m_type=GridType::Horizon;
};

#endif // GRIDSELECTDIALOG_H
