#ifndef CROSSPLOTFILTERDIALOG_H
#define CROSSPLOTFILTERDIALOG_H

#include <QDialog>

namespace Ui {
class CrossplotFilterDialog;
}

class CrossplotFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrossplotFilterDialog(QWidget *parent = 0);
    ~CrossplotFilterDialog();

    int minInline();
    int maxInline();
    int minCrossline();
    int maxCrossline();
    int minZ();
    int maxZ();
    double minAttribute();
    double maxAttribute();

    void setMinInline(int);
    void setMaxInline(int);
    void setMinCrossline(int);
    void setMaxCrossline(int);
    void setMinZ(int);
    void setMaxZ(int);
    void setMinAttribute(double);
    void setMaxAttribute(double);

private:
    Ui::CrossplotFilterDialog *ui;
};

#endif // CROSSPLOTFILTERDIALOG_H
