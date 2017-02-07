#ifndef CONVERTGRIDDIALOG_H
#define CONVERTGRIDDIALOG_H

#include <processparametersdialog.h>
#include <QMap>
namespace Ui {
class ConvertGridDialog;
}

class ConvertGridDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ConvertGridDialog(QWidget *parent = 0);
    ~ConvertGridDialog();

    QMap<QString,QString> params();

public slots:
    void setInputGrids( QString type, const QStringList& l);

private slots:
    void updateInputGrids();
    void updateOkButton();

private:
    Ui::ConvertGridDialog *ui;

    QMap<QString, QStringList> m_inputGrids;
};

#endif // CONVERTGRIDDIALOG_H
