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
    bool isFixedColor();
    QColor pointColor();
    QColor trendlineColor();

public slots:
    void setDatapointSize(int);
    void setFixedColor(bool);
    void setPointColor(QColor);
    void setTrendlineColor(QColor);

signals:
    void datapointSizeChanged(int);
    void fixedColorChanged(bool);
    void pointColorChanged(QColor);
    void trendlineColorChanged(QColor);

private slots:
    void on_cbFixColor_clicked();

    void on_cbTrendlineColor_clicked();

private:

    void saveSettings();
    void loadSettings();

    Ui::CrossplotViewerDisplayOptionsDialog *ui;
};

#endif // CROSSPLOTVIEWERDISPLAYOPTIONSDIALOG_H
