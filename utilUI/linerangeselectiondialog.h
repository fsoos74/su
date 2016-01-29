#ifndef LINERANGESELECTIONDIALOG_H
#define LINERANGESELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class LineRangeSelectionDialog;
}

class LineRangeSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LineRangeSelectionDialog(QWidget *parent = 0);
    ~LineRangeSelectionDialog();

    int minInline()const;
    int maxInline()const;
    int minCrossline()const;
    int maxCrossline()const;

public slots:

    void setMinInline(int);
    void setMaxInline(int);
    void setMinCrossline(int);
    void setMaxCrossline(int);

private slots:

    void checkOk();

private:
    Ui::LineRangeSelectionDialog *ui;
};

#endif // LINERANGESELECTIONDIALOG_H
