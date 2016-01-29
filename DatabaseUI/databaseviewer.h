#ifndef DATABASEVIEWER_H
#define DATABASEVIEWER_H

#include <QDialog>

#include <QtSql/QtSql>

namespace Ui {
class DatabaseViewer;
}

class DatabaseViewer : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseViewer(QWidget *parent = 0);
    ~DatabaseViewer();

    void setData( QSqlTableModel* table);

private:



    Ui::DatabaseViewer *ui;
};

#endif // DATABASEVIEWER_H
