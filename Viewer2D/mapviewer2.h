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
#include<reversedoublespinbox.h>

namespace Ui {
class MapViewer2;
}

class MapViewer2 : public BaseViewer
{
    Q_OBJECT

public:

    enum class ItemType : int { Area, Volume, HorizonGrid, AttributeGrid, OtherGrid, Well };

    explicit MapViewer2(QWidget *parent = 0);
    ~MapViewer2();

    QStringList itemList( ItemType );

    bool showProjectArea()const{
        return m_showProjectArea;
    }

    qreal wellRefDepth()const{
        return m_wellRefDepth;
    }

public slots:
    void setProject(AVOProject* );
    void setShowProjectArea(bool);
    void setWellRefDepth(qreal);

signals:
    void showProjectAreaChanged(bool);
    void wellRefDepthChanged(qreal);

protected:
    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);


private slots:
    void on_actionSetup_Wells_triggered();
    void onMouseDoubleClick(QPointF);
    void onMouseOver(QPointF);
    void updateItemsFromTree();
    void updateScene();

    void on_actionSet_Well_Reference_Depth_triggered();

private:

    void configWellItem( WellItem* );
    void configGridItem( GridItem* );
    void configVolumeItem( VolumeItem* );

    void fillTree();
    void addVolumeItem(QString);
    void addHorizonItem(QString);
    void addWellItem(QString);
    void setupToolBarControls();
    void addItemDockWidget( QString title, QGraphicsItem*, QWidget* );
    void removeItemDockWidget( QGraphicsItem* );

    QGraphicsItem* findItem(ItemType,QString);
    void setupMouseModes();
    QRectF addMargins(const QRectF&);

    Ui::MapViewer2 *ui;
    ReverseDoubleSpinBox* m_sbRefDepth;
    AVOProject* m_project=nullptr;
    bool m_showProjectArea=true;
    QStringList m_grids;
    QStringList m_wells;
    qreal m_wellRefDepth=0;
    QMap<QGraphicsItem*, QDockWidget*> m_dockWidgets;
    WellItem m_defaultWellItem;   // use this to store configuration of well display
};

#endif // MAPVIEWER2_H
