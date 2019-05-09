#ifndef VIEWITEM_H
#define VIEWITEM_H

#include <QObject>

class ViewItem : public QObject
{
    Q_OBJECT
public:
    explicit ViewItem(QObject *parent = nullptr);
    QString name()const{
        return mName;
    }
signals:
    void changed();

public slots:
    void setName(QString);

private:
    QString mName;
};

#endif // VIEWITEM_H
