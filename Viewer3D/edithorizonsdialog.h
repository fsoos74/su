#ifndef EDITHORIZONSDIALOG_H
#define EDITHORIZONSDIALOG_H

#include <QDialog>

#include <QVector>

#include <horizondef.h>

#include <horizonmanager.h>

namespace Ui {
class EditHorizonsDialog;
}

class EditHorizonsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditHorizonsDialog(QWidget *parent = 0);
    ~EditHorizonsDialog();

    void setHorizonManager( HorizonManager* );

signals:

    void addHorizonRequested();

public slots:

    void refreshControls();
    void horizonChanged(QString);

private slots:

    void on_pbRemove_clicked();

    void updateParams();

    void on_cbColor_clicked();

    void on_cbHorizon_currentIndexChanged(const QString &arg1);

    void horizonParamsToControls( QString );
    void horizonParamsFromControls( QString );

private:
    Ui::EditHorizonsDialog *ui;




    HorizonManager* horizonManager=nullptr;
};

#endif // EDITHORIZONSDIALOG_H
