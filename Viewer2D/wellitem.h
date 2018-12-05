#ifndef WELLITEM_H
#define WELLITEM_H

#include<QGraphicsItemGroup>
#include<QGraphicsEllipseItem>
#include<QFont>
#include<wellinfo.h>
#include<wellpath.h>
#include"alignedtextgraphicsitem.h"
#include<memory>
#include<QObject>

class WellItem : public QObject,public QGraphicsItemGroup
{
    Q_OBJECT
public:

    enum LabelType{ NO_LABEL=0, UWI, WELL_NAME};

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

    QColor symbolColor()const{
        return m_symbolColor;
    }

    QColor pathColor()const{
        return m_pathColor;
    }

    QFont font()const;
    int size() const;
    QString label() const;

public slots:

    void setInfo(const WellInfo&);
    void setPath(std::shared_ptr<WellPath>);
    void setRefDepth(qreal);
    void setLabelType(LabelType);
    void setSymbolColor(QColor);
    void setPathColor(QColor);
    void setFont(QFont);
    void setSize(int);
    void setText(QString);

private:
    void updateLabel();
    void updatePathItem();

    WellInfo m_info;
    std::shared_ptr<WellPath> m_path;
    qreal m_refDepth=0;

    LabelType m_labelType=LabelType::UWI;
    int m_size=3;
    QColor m_symbolColor=Qt::darkGreen;
    QColor m_pathColor=Qt::darkGreen;

    QGraphicsEllipseItem* m_symbol=nullptr;
    AlignedTextGraphicsItem* m_label=nullptr;
    QGraphicsPathItem* m_pathItem=nullptr;
};

QString toQString(WellItem::LabelType);
WellItem::LabelType toLabelType(QString);

#endif // WELLITEM_H
