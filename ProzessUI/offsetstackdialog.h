#ifndef OFFSETSTACKDIALOG_H
#define OFFSETSTACKDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class OffsetStackDialog;
}

class OffsetStackDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit OffsetStackDialog(QWidget *parent = 0);
    ~OffsetStackDialog();

    QMap<QString,QString> params();

public slots:

    void setInputDatasets( const QStringList&);
    void setOffsetRange( float min, float max);

private slots:
    void updateOkButton();

private:
    Ui::OffsetStackDialog *ui;
};

#endif // OffsetStackDialog_H
