#ifndef LOGPROPERTIESDIALOG_H
#define LOGPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class LogPropertiesDialog;
}

class LogPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogPropertiesDialog(QWidget *parent = 0);
    ~LogPropertiesDialog();

    QString name();
    QString unit();
    QString description();

public slots:
    void setName(QString);
    void setUnit(QString);
    void setDescription(QString);

private:
    Ui::LogPropertiesDialog *ui;
};

#endif // LOGPROPERTIESDIALOG_H
