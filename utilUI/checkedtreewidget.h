#ifndef CHECKEDTREEWIDGET_H
#define CHECKEDTREEWIGGET_H

#include<QTreeWidget>
#include<QTreeWidgetItem>



class CheckedTreeWidget : public QTreeWidget{
    Q_OBJECT
public:
    CheckedTreeWidget(QWidget* parent);

    bool containsItem(QString);
    bool containsItem(QString,QString);
    bool isChecked(QString,QString);
    QStringList items();
    QStringList items(QString);

    void addItem(QString,bool checkable=false);
    void addItem(QString, QString, bool checked=false);

public slots:
    void setChecked(QString,QString,bool);
private slots:
    void onItemClicked(QTreeWidgetItem*,int);

private:
    QTreeWidgetItem* findItem(QString);
    QTreeWidgetItem* findItem(QTreeWidgetItem*, QString name);
    QTreeWidgetItem* findItem(QString, QString);
};

#endif
