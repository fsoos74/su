#ifndef EDITMARKERSDIALOG_H
#define EDITMARKERSDIALOG_H

#include <QDialog>
#include <wellmarkers.h>

namespace Ui {
class EditMarkersDialog;
}

class EditMarkersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditMarkersDialog(QWidget *parent = 0);
    ~EditMarkersDialog();

    WellMarkers markers();

public slots:
    void setMarkers(WellMarkers);

private slots:
    void on_pbAdd_clicked();

    void on_pbRemove_clicked();

private:
    Ui::EditMarkersDialog *ui;
};

#endif // EDITMARKERSDIALOG_H
