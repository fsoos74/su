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
    bool flattenTrend();

public slots:
    void setDatapointSize(int);
    void setFlattenTrend(bool);

signals:
    void datapointSizeChanged(int);
    void flattenTrendChanged(bool);

private:

    void saveSettings();
    void loadSettings();

    Ui::CrossplotViewerDisplayOptionsDialog *ui;
};

#endif // CROSSPLOTVIEWERDISPLAYOPTIONSDIALOG_H
