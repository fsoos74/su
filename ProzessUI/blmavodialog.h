#ifndef BLMAVODIALOG_H
#define BLMAVODIALOG_H

#include <QDialog>
#include<processparametersdialog.h>

namespace Ui {
class BLMAVODialog;
}

class BLMAVODialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit BLMAVODialog(QWidget *parent = nullptr);
    ~BLMAVODialog();

    QString lineName()const;
    QString exportPath()const;
    int maxTime()const;

    QMap<QString,QString> params();

public slots:
    void setLineName(QString);
    void setExportPath(QString);
    void setMaxTime(int);

private:
    Ui::BLMAVODialog *ui;
};

#endif // BLMAVODIALOG_H
