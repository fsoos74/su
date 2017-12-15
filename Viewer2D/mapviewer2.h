#ifndef MAPVIEWER2_H
#define MAPVIEWER2_H

#include<baseviewer.h>
#include <QDialog>
#include <QGraphicsPixmapItem>
#include <avoproject.h>
#include<memory>
#include<griditem.h>
#include<volumeitem.h>
#include<wellitem.h>
#include<QDockWidget>

namespace Ui {
class MapViewer2;
}

class MapViewer2 : public BaseViewer
{
    Q_OBJECT

public:

    enum class ItemType : int { Volume, HorizonGrid, AttributeGrid, OtherGrid, Well };

    explicit MapViewer2(QWidget *parent = 0);
    ~MapViewer2();

    QStringList itemList( ItemType );

    qreal wellRefDepth()const{
        return m_wellRefDepth;
    }

public slots:
    void setProject(AVOProject* );
    void setWellRefDepth(qreal);

signals:
    void wellRefDepthChanged(qreal);

protected:
    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);


private slots:
    void onMouseDoubleClick(QPointF);
    void onMouseOver(QPointF);
    void updateScene();

    void on_actionAdd_Grid_triggered();
    void on_actionAdd_Wells_triggered();
    void on_actionSelect_Volumes_triggered();
    void on_actionSetup_Wells_triggered();
    void on_actionSetup_Grids_triggered();
    void on_actionSetup_Volume_triggered();
    void on_actionSet_Well_Reference_Depth_triggered();

    void on_actionSelect_Horizons_triggered();

    void on_actionSelect_Attribute_Grids_triggered();

private:

    void selectGrids(GridType, ItemType);
    void configWellItem( WellItem* );
    void configGridItem( GridItem* );
    void configVolumeItem( VolumeItem* );

    void addItemDockWidget( QString title, QGraphicsItem*, QWidget* );
    void removeItemDockWidget( QGraphicsItem* );

    QGraphicsItem* findItem(ItemType,QString);
    void setupMouseModes();
    QRectF addMargins(const QRectF&);

    Ui::MapViewer2 *ui;
    AVOProject* m_project=nullptr;
    QStringList m_grids;
    QStringList m_wells;
    qreal m_wellRefDepth=0;

    QMap<QGraphicsItem*, QDockWidget*> m_dockWidgets;

    WellItem m_defaultWellItem;   // use this to store configuration of well display
};

#endif // MAPVIEWER2_H
