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

    bool showMesh()const{
        return m_mesh;
    }

    bool showLabels()const{
        return m_labels;
    }

    int inlineIncrement()const{
        return m_inlineIncrement;
    }

    int crosslineIncrement()const{
        return m_crosslineIncrement;
    }

    void setProject( AVOProject*);
    void setGrid( std::shared_ptr<Grid2D<float>> );
    void setColorTable( ColorTable* );

public slots:

    void setShowMesh(bool);
    void setShowLabels(bool);
    void setInlineIncrement(int);
    void setCrosslineIncrement(int);

protected slots:

    void updatePixmap();
    void updateFrame();
    void updateMesh();
    void updateLabels();
    void updateGeometry();


private:

    AVOProject* m_project=nullptr;
    std::shared_ptr<Grid2D<float>> m_grid;
    ColorTable* m_colorTable=nullptr;

    int m_inlineIncrement=20;
    int m_crosslineIncrement=20;
    bool m_mesh=true;
    bool m_labels=true;
    QGraphicsItem* m_frameItem=nullptr;
    QVector<QGraphicsItem*> m_meshItems;
    QVector<QGraphicsItem*> m_labelItems;

};

#endif // GRIDITEM_H
