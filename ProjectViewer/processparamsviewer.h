#ifndef PROCESSPARAMSVIEWER_H
#define PROCESSPARAMSVIEWER_H

#include <QDialog>
#include <processparams.h>
namespace Ui {
class ProcessParamsViewer;
}

class ProcessParamsViewer : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessParamsViewer(QWidget *parent = 0);
    ~ProcessParamsViewer();

public slots:
    void setParams(const ProcessParams&);

private:
    Ui::ProcessParamsViewer *ui;
};

#endif // PROCESSPARAMSVIEWER_H
