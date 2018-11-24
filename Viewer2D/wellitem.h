#ifndef WELLITEM_H
#define WELLITEM_H

//#include<QGraphicsItemGroup>
#include<QGraphicsEllipseItem>
#include<QFont>
#include<wellinfo.h>
#include<wellpath.h>
#include"alignedtextgraphicsitem.h"
#include<memory>
#include<QObject>

class WellItem : public QObject,public QGraphicsEllipseItem //QGraphicsItemGroup
{
    Q_OBJECT
public:

    enum LabelType{ UWI=0, WELL_NAME};

    WellItem( QGraphicsItem* parentGraphicsItem=nullptr, QObject* parentQObject=nullptr);

    WellInfo info()const{
        return m_info;
    }

    std::shared_ptr<WellPath> path()const{
        return m_path;
    }

    LabelType labelType()const{
        return m_labelType;
    }

    QFont font()const;
    int size() const;
    QString text() const;

public slots:

    void setInfo(const WellInfo&);
    void setPath(std::shared_ptr<WellPath>);
    void setRefDepth(qreal);
    void setLabelType(LabelType);

    void setFont(QFont);
    void setSize(int);
    void setText(QString);

private:

    QString labelText();

    WellInfo m_info;
    std::shared_ptr<WellPath> m_path;
    qreal m_refDepth=0;
    LabelType m_labelType=LabelType::UWI;

    AlignedTextGraphicsItem* m_textItem;
    int m_size;
};

#endif // WELLITEM_H
