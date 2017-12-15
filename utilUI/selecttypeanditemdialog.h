#ifndef SELECTTYPEANDITEMDIALOG_H
#define SELECTTYPEANDITEMDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
class SelectTypeAndItemDialog;
}

class SelectTypeAndItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectTypeAndItemDialog(QWidget *parent = 0);
    ~SelectTypeAndItemDialog();

    QString selectedType();
    QString selectedItem();

public slots:
    void addType( QString name, QStringList items);
    void setTypeLabelText(QString);
    void setItemLabelText(QString);

private slots:
    void on_cbType_currentIndexChanged(const QString &arg1);

private:
    Ui::SelectTypeAndItemDialog *ui;

    QMap<QString, QStringList> m_typeItems;
};

#endif // SELECTTYPEANDITEMDIALOG_H
