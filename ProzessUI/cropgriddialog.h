#ifndef CROPGRIDDIALOG_H
#define CROPGRIDDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class CropGridDialog;
}

class CropGridDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit CropGridDialog(QWidget *parent = 0);
    ~CropGridDialog();

    QMap<QString,QString> params();

public slots:

    void setInputGrids( QString type, const QStringList& l);
    void setInlineRange(int min, int max);
    void setCrosslineRange(int min, int max);

private slots:

    void updateInputGrids();
    void updateOkButton();

private:
    Ui::CropGridDialog *ui;
    QMap<QString, QStringList> m_inputGrids;
};

#endif // CROPGRIDDIALOG_H
