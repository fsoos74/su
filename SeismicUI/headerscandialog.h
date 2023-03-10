#ifndef HEADERSCANDIALOG_H
#define HEADERSCANDIALOG_H

#include <QDialog>
#include <QVector>
#include <QStandardItemModel>

namespace Ui {
class HeaderScanDialog;
}

class HeaderScanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HeaderScanDialog(QWidget *parent = 0);
    ~HeaderScanDialog();

    void setModel( QStandardItemModel* model);

private:
    Ui::HeaderScanDialog *ui;
};

#endif // HEADERSCANDIALOG_H
