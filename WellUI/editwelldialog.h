#ifndef EDITWELLDIALOG_H
#define EDITWELLDIALOG_H

#include <QDialog>

namespace Ui {
class EditWellDialog;
}

class EditWellDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditWellDialog(QWidget *parent = 0);
    ~EditWellDialog();

    QString uwi();
    QString name();
    qreal x();
    qreal y();
    qreal z();

public slots:
    void setUWI(QString);
    void setName(QString);
    void setX(qreal);
    void setY(qreal);
    void setZ(qreal);

private:
    Ui::EditWellDialog *ui;
};

#endif // EDITWELLDIALOG_H
