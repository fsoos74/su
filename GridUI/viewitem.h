#ifndef VIEWITEM_H
#define VIEWITEM_H

#include <QObject>

class ViewItem : public QObject
{
    Q_OBJECT
public:
    explicit ViewItem(QObject *parent = nullptr);

signals:

public slots:
};

#endif // VIEWITEM_H