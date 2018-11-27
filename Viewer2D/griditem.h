#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <alignedtextgraphicsitem.h>
#include <avoproject.h>
#include <grid2d.h>
#include <colortable.h>
#include <memory>

class GridItem : public QObject, public QGraphicsPixmapItem
{
Q_OBJECT

public:

    GridItem(AVOProject* project=nullptr, QGraphicsItem* parentGraphicsItemItem=nullptr, QObject* parentQObject=nullptr );

    AVOProject* project()const{
        return m_project;
    }

    std::shared_ptr<Grid2D<float>> grid()const{
        return m_grid;
    }

    ColorTable* colorTable()const{
        return m_colorTable;
    }

    QString label()const{
        return m_label;
    }

    bool showMesh()const{
        return m_mesh;
    }

    bool showLabel()const{
        return m_showLabel;
    }

    bool showLineLabels()const{
        return m_showLineLabels;
    }

    int inlineIncrement()const{
        return m_inlineIncrement;
    }

    int crosslineIncrement()const{
        return m_crosslineIncrement;
    }

public slots:
    void setProject( AVOProject*);
    void setGrid( std::shared_ptr<Grid2D<float>> );
    void setColorTable( ColorTable* );
    void setLabel(QString);
    void setShowMesh(bool);
    void setShowLabel(bool);
    void setShowLineLabels(bool);
    void setInlineIncrement(int);
    void setCrosslineIncrement(int);
    void setZValueWrapper(int);

signals:
    void dataChanged(float*,size_t,float);

protected slots:
    void updatePixmap();
    void updateFrame();
    void updateMesh();
    void updateLineLabels();
    void updateLabel();
    void updateGeometry();

private:

    AVOProject* m_project=nullptr;
    std::shared_ptr<Grid2D<float>> m_grid;
    ColorTable* m_colorTable=nullptr;
    QString m_label;

    int m_inlineIncrement=20;
    int m_crosslineIncrement=20;
    bool m_mesh=true;
    bool m_showLabel=true;
    bool m_showLineLabels=true;
    QGraphicsItem* m_frameItem=nullptr;
    QVector<QGraphicsItem*> m_meshItems;
    QVector<QGraphicsItem*> m_lineLabelItems;
    QGraphicsItem* m_labelItem=nullptr;
};

#endif // GRIDITEM_H
