#ifndef CROSSPLOTVOLUMESINPUTDIALOG_H
#define CROSSPLOTVOLUMESINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class CrossplotVolumesInputDialog;
}

class CrossplotVolumesInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrossplotVolumesInputDialog(QWidget *parent = 0);
    ~CrossplotVolumesInputDialog();

    QString xName();
    QString yName();

    bool useAttribute();
    QString attributeName();

public slots:
    void setVolumes(QStringList);
    void setUseGrids(bool);

private:
    Ui::CrossplotVolumesInputDialog *ui;
};

#endif // CROSSPLOTVOLUMESINPUTDIALOG_H
