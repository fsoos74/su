#ifndef EDITSLICESDIALOG_H
#define EDITSLICESDIALOG_H

#include <QDialog>
#include <QVector>
#include <slicedef.h>
#include <grid3d.h>  // need bounds
#include <slicemodel.h>

namespace Ui {
class EditSlicesDialog;
}

class EditSlicesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditSlicesDialog(QWidget *parent = 0);
    ~EditSlicesDialog();

    SliceModel* sliceModel()const{
        return m_sliceModel;
    }

    void setBounds( Grid3DBounds );

signals:

    void sliceAdded( QString, SliceDef);
    void sliceRemoved( QString );
    void sliceChanged( QString, SliceDef );

public slots:

    void setSliceModel( SliceModel* );
    void setCurrentSlice( QString );

private slots:

    void modelChanged();

    void sliceControlsChanged();

    void on_pbAdd_clicked();

    void on_pbDelete_clicked();

    void on_cbName_currentIndexChanged(const QString &arg1);

private:

    void sliceToControls( QString, SliceDef );
    SliceDef sliceFromControls( QString );

    Ui::EditSlicesDialog *ui;

    SliceModel* m_sliceModel=nullptr;
    Grid3DBounds  m_bounds;

    bool wait_controls=false;
};

#endif // EDITSLICESDIALOG_H
