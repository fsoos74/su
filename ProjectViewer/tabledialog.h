#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <QDialog>
#include <table.h>
#include<avoproject.h>
#include<memory>

namespace Ui {
class TableDialog;
}

class TableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableDialog(QWidget *parent = 0);
    ~TableDialog();

public slots:

    void setProject(AVOProject* project);
    void loadTable( QString name);
    void saveTable();


private slots:

    void on_pbAdd_clicked();
    void on_pbRemove_clicked();

private:

    void tableToView(const Table& table);
    Table tableFromView();

    Ui::TableDialog *ui;

    AVOProject* m_project=nullptr;
    QString m_name;
    bool m_multi;   // everything else is stored in tablewidget model
};

#endif // TABLEDIALOG_H
