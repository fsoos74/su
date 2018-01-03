#ifndef SCATTERPLOTVIEWERDISPLAYOPTIONSDIALOG_H
#define SCATTERPLOTVIEWERDISPLAYOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class ScatterplotViewerDisplayOptionsDialog;
}

class ScatterplotViewerDisplayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScatterplotViewerDisplayOptionsDialog(QWidget *parent = 0);
    ~ScatterplotViewerDisplayOptionsDialog();

    int pointSize();
    bool pointOutline();

public slots:
    void setPointSize(int);
    void setPointOutline(bool);

signals:
    void pointSizeChanged(int);
    void pointOutlineChanged(bool);

private slots:

private:

    Ui::ScatterplotViewerDisplayOptionsDialog *ui;
};

#endif // SCATTERPLOTVIEWERDISPLAYOPTIONSDIALOG_H
