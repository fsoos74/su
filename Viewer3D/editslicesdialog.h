#ifndef EDITSLICESDIALOG_H
#define EDITSLICESDIALOG_H

#include <QDialog>
#include <QVector>
#include <slicedef.h>
#include <slicemodel.h>
#include <volumedimensions.h>

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


public slots:

    void setDimensions( VolumeDimensions );
    void setVolumes( QStringList );

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
    SliceDef sliceFromControls();

    Ui::EditSlicesDialog *ui;

    SliceModel* m_sliceModel=nullptr;
    VolumeDimensions m_dimensions;

    bool wait_controls=false;
};

#endif // EDITSLICESDIALOG_H
