#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QVariant>
#include <QList>
#include <QString>
#include <QGridLayout>

class OptionsDialog: public QDialog
{
Q_OBJECT
public:

    OptionsDialog(QWidget* parent=nullptr, Qt::WindowFlags flags=0);
    int indexOf(QString name);
    void clear();
    void addItem(QString name, QVariant::Type itype, QVariant options=QVariant(), QString ltext=QString());
    void removeItem(QString name);
    void setValue(QString name, QVariant value);
    void setValue(int row, QVariant value);
    void setValues(QList<QVariant> vals);
    QVariant getValue(QString name);
    QVariant getValue(int row);
    QList<QVariant> getValues();
    static QList<QVariant> getValues(QList<std::tuple<QString,QVariant,QVariant>> defs);

protected:
    QWidget* createStringControl(QVariant options);
    QWidget* createIntControl(QVariant options);
    QWidget* createDoubleControl(QVariant options);
    QWidget* createBoolControl(QVariant options);
    QWidget* createColorControl(QVariant options);

private:
    QGridLayout* m_controlLayout=nullptr;
    QList<std::tuple<QString,QVariant::Type,QWidget*>> m_controls;  // use list to keep order
};

#endif // OPTIONSDIALOG_H
