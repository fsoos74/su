#ifndef TWOCOMBOSDIALOG_H
#define TWOCOMBOSDIALOG_H

#include <QDialog>

namespace Ui {
class TwoCombosDialog;
}

class TwoCombosDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TwoCombosDialog(QWidget *parent = 0);
    ~TwoCombosDialog();

    QString selection1()const;
    QString selection2()const;

public slots:

    void setLabelText1(const QString&);
    void setLabelText2(const QString&);
    void setItems1( const QStringList&);
    void setItems2( const QStringList&);

private:
    Ui::TwoCombosDialog *ui;
};

#endif // TWOCOMBOSDIALOG_H
