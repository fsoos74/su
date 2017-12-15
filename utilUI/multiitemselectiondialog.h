#ifndef MULTIITEMSELECTIONDIALOG_H
#define MULTIITEMSELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class MultiItemSelectionDialog;
}

class MultiItemSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiItemSelectionDialog(QWidget *parent = 0);
    ~MultiItemSelectionDialog();

    QStringList selectedItems();

    static QStringList getItems(QWidget* parent, QString title, QString label, QStringList items, bool* ok=nullptr, QStringList selection=QStringList());

public:
    void setItems(QStringList);
    void setSelectedItems(QStringList);
    void setLabel(QString);

private slots:
    void on_pbClear_clicked();

private:
    Ui::MultiItemSelectionDialog *ui;
};

#endif // MULTIITEMSELECTIONDIALOG_H
