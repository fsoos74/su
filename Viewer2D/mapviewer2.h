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
#include<reversespinbox.h>
#include<QComboBox>
#include<QMdiSubWindow>

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

    Domain domain()const{
        return toDomain(m_cbDomain->currentText());
    }

    double sliceValue()const{
        return m_sbSliceValue->value();
    }

    double wellReferenceDepth()const{
        return m_wellReferenceDepth;
    }

public slots:
    void setProject(AVOProject* );
    void setShowProjectArea(bool);
    void setDomain(Domain);
    void setDomain(QString);
    void setSliceValue(int);
    void setWellReferenceDepth(double);

signals:
    void showProjectAreaChanged(bool);
    void domainChanged(Domain);
    void sliceValueChanged(int);
    void wellReferenceDepthChanged(double);

protected:
    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);

private slots:
    void on_actionSetup_Wells_triggered();
    void onMouseDoubleClick(QPointF);
    void onMouseOver(QPointF);
    bool eventFilter(QObject *watched, QEvent *event);
    void updateItemsFromTree();
    void updateScene();

private:

    void configWellItem( WellItem* );
    void configGridItem( GridItem* );
    void configVolumeItem( VolumeItem* );

    void fillTree();
    void addVolumeItem(QString);
    void addHorizonItem(QString);
    void addWellItem(QString);
    void setupToolBarControls();
    void addItemLegendWidget(QGraphicsItem*, QWidget*, QString );
    void removeItemLegendWidget( QGraphicsItem* );

    QGraphicsItem* findItem(ItemType,QString);
    void setupMouseModes();
    QRectF addMargins(const QRectF&);

    Ui::MapViewer2 *ui;
    QComboBox* m_cbDomain;
    ReverseSpinBox* m_sbSliceValue;
    AVOProject* m_project=nullptr;
    bool m_showProjectArea=true;
    QStringList m_grids;
    QStringList m_wells;
    Domain m_domain=Domain::Depth;
    int m_sliceValue=0;
    double m_wellReferenceDepth=0;
    QMap<QGraphicsItem*, QMdiSubWindow*> m_legendWidgets;
    WellItem m_defaultWellItem;   // use this to store configuration of well display
};

#endif // MAPVIEWER2_H
