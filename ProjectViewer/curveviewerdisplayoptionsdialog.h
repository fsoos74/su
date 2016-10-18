#ifndef CURVEVIEWERDISPLAYOPTIONSDIALOG_H
#define CURVEVIEWERDISPLAYOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class CurveViewerDisplayOptionsDialog;
}

class CurveViewerDisplayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CurveViewerDisplayOptionsDialog(QWidget *parent = 0);
    ~CurveViewerDisplayOptionsDialog();
    int datapointSize();

public slots:
    void setDatapointSize(int);

signals:
    void datapointSizeChanged(int);
    void showRegressionLinesChanged(bool);

private:
    Ui::CurveViewerDisplayOptionsDialog *ui;
};

#endif // CURVEVIEWERDISPLAYOPTIONSDIALOG_H
