#ifndef CDPDATABASEVIEWER_H
#define CDPDATABASEVIEWER_H

#include <QDialog>
#include <memory>
#include<cdpdatabase.h>

namespace Ui {
class CDPDatabaseViewer;
}

class CDPDatabaseViewer : public QDialog
{
    Q_OBJECT

public:
    explicit CDPDatabaseViewer(QWidget *parent = 0);
    ~CDPDatabaseViewer();

public slots:

    void setDatabase( std::shared_ptr<CDPDatabase>);

private:

    void updateModel();

    Ui::CDPDatabaseViewer *ui;

    std::shared_ptr<CDPDatabase> m_db;
};

#endif // CDPDATABASEVIEWER_H
