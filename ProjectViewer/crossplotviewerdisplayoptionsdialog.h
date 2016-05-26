#ifndef CROSSPLOTVIEWERDISPLAYOPTIONSDIALOG_H
#define CROSSPLOTVIEWERDISPLAYOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class CrossplotViewerDisplayOptionsDialog;
}

class CrossplotViewerDisplayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrossplotViewerDisplayOptionsDialog(QWidget *parent = 0);
    ~CrossplotViewerDisplayOptionsDialog();

    int datapointSize();

public slots:
    void setDatapointSize(int);

signals:
    void datapointSizeChanged(int);

private:

    void saveSettings();
    void loadSettings();

    Ui::CrossplotViewerDisplayOptionsDialog *ui;
};

#endif // CROSSPLOTVIEWERDISPLAYOPTIONSDIALOG_H
