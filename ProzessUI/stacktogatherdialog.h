#ifndef STACKTOGATHERDIALOG_H
#define STACKTOGATHERDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class StackToGatherDialog;
}

class StackToGatherDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit StackToGatherDialog(QWidget *parent = 0);
    ~StackToGatherDialog();

    QMap<QString,QString> params()override;

public slots:
    void setInputStacks(QStringList);

private slots:
    void updateInputStackControls();
    void updateOkButton();

private:
    Ui::StackToGatherDialog *ui;

    AVOProject* m_project=nullptr;
    QStringList m_inputStacks;
    QVector<QComboBox*> inputCB;
};

#endif // STACKTOGATHERDIALOG_H
