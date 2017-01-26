#ifndef EDITHORIZONSDIALOG_H
#define EDITHORIZONSDIALOG_H

#include <QDialog>

#include <QVector>

#include <horizondef.h>

#include <horizonmanager.h>

#include <avoproject.h>

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

    void setHorizonManager( HorizonManager* );
    void setProject( std::shared_ptr<AVOProject>);

public slots:

    void refreshControls();
    void horizonChanged(QString);

private slots:

    void on_pbRemove_clicked();

    void on_pbAdd_clicked();

    void updateParams();

    void on_cbColor_clicked();

    void on_cbHorizon_currentIndexChanged(const QString &arg1);

    void horizonParamsToControls( QString );
    void horizonParamsFromControls( QString );



private:
    Ui::EditHorizonsDialog *ui;

    HorizonManager* horizonManager=nullptr;
    std::shared_ptr<AVOProject> m_project;
};

#endif // EDITHORIZONSDIALOG_H
