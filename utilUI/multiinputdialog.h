#ifndef MULTIINPUTDIALOG_H
#define MULTIINPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
#include <QVector>

class MultiInputDialog : public QDialog
{
public:

        enum class InputStyle{ LineEdit, ComboBox };

        MultiInputDialog( int count, QStringList labels=QStringList(), QStringList inputs=QStringList(),
                          InputStyle istyle=InputStyle::LineEdit, QWidget* parent=nullptr);

        QStringList getInputs();

        static QStringList getTexts( QWidget* parent, QString title, QStringList labels, QStringList texts=QStringList(), bool* ok=nullptr);
        static QVector<int> getInts( QWidget* parent, QString title, QStringList labels, QVector<int> vals=QVector<int>(), bool* ok=nullptr);
        static QVector<double> getDoubles( QWidget* parent, QString title, QStringList labels, QVector<double> vals=QVector<double>(), bool* ok=nullptr);
        static QStringList getItems( QWidget* parent, QString title, QStringList labels, QStringList items, bool* ok=nullptr);

private:

    InputStyle m_istyle;
    QVector<QLineEdit*> m_lineEdits;
    QVector<QComboBox*> m_combos;
};

#endif // MULTIINPUTDIALOG_H
