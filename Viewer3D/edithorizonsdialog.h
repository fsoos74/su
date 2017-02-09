#ifndef EDITHORIZONSDIALOG_H
#define EDITHORIZONSDIALOG_H

#include <QDialog>

#include <horizonmodel.h>

#include <avoproject.h>

#include <grid2d.h>
#include <memory>


namespace Ui {
class EditHorizonsDialog;
}

class EditHorizonsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditHorizonsDialog(QWidget *parent = 0);
    ~EditHorizonsDialog();

    void setProject( AVOProject*);

public slots:

    void setHorizonModel( HorizonModel* );
    void setCurrentHorizon( QString );

private slots:

    void modelChanged();

    void horizonControlsChanged();

    void on_pbRemove_clicked();

    void on_pbAdd_clicked();

    void on_cbHorizon_currentIndexChanged(const QString &arg1);

    void on_cbColor_clicked();

private:

    void horizonToControls( QString, HorizonDef );
    HorizonDef horizonFromControls( QString );


    Ui::EditHorizonsDialog *ui;

    HorizonModel* m_horizonModel=nullptr;
    AVOProject* m_project;

    bool wait_controls=false;
    std::shared_ptr<Grid2D<float>> m_currentHorizonControl;
};

#endif // EDITHORIZONSDIALOG_H
