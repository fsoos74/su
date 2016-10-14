#ifndef SEISMICDATASELECTOR_H
#define SEISMICDATASELECTOR_H

#include <QDialog>
#include<QKeyEvent>
#include<memory>
#include <seismicdatasetreader.h>
#include <gather.h>
#include <gathersortkey.h>


namespace Ui {
class SeismicDataSelector;
}

class SeismicDataSelector : public QDialog
{
    Q_OBJECT

public:

     static const int MAXIMUM_TRACES_PER_SCREEN=9999;

    enum Order{
        ORDER_NONE=0,
        ORDER_INLINE_ASCENDING,
        ORDER_INLINE_DESCENDING,
        ORDER_CROSSLINE_ASCENDING,
        ORDER_CROSSLINE_DESCENDING,
        ORDER_OFFSET_ASCENDING,
        ORDER_OFFSET_DESCENDING
    };
/*
    enum SortKey{
        SORT_NONE=0,
        SORT_INLINE,
        SORT_CROSSLINE,
        SORT_OFFSET
    };
*/
    explicit SeismicDataSelector(QWidget *parent = 0);
    ~SeismicDataSelector();

    GatherSortKey primarySort()const{
        return m_primarySortKey;
    }

    QString selectionDescription();

    bool isLocked()const{
        return m_lock;
    }

public slots:

    void setReader(std::shared_ptr<SeismicDatasetReader>);
    void setInlineRange( int min, int max);
    void setCrosslineRange( int min, int max);
    void setInlineCountRange( int min, int max);
    void setCrosslineCountRange( int min, int max);
    void setInline(int);
    void setCrossline(int);
    void setInlineCount( int count );
    void setCrosslineCount( int count );
    void setOrder(int, int, int);

    void providePoint(int iline, int xline);
    void provideRandomLine( QVector<QPoint> polyline);
    void providePerpendicularLine(int, int);

    void setLock(bool);
    void apply();

signals:

    void lockChanged(bool);
    void gatherChanged(std::shared_ptr<seismic::Gather>);
    void primarySortChanged(GatherSortKey);
    void descriptionChanged(QString);

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);


private slots:

    void updatePrimarySort();
    void setPrimarySort(GatherSortKey);

    void readGather();
    void changeOrder();

    void on_sbIlineCount_valueChanged(int arg1);

    void on_sbXlineCount_valueChanged(int arg1);

    void on_rbRandomLine_toggled(bool checked);

    void on_tbInline_clicked();

    void on_tbXLine_clicked();

    void on_pbLock_toggled(bool checked);

private:

    void indexToKey( int idx, QString& key, bool& ascending);

    Ui::SeismicDataSelector *ui;

    std::shared_ptr<SeismicDatasetReader>   m_reader;
    std::shared_ptr<seismic::Gather>        m_gather;

    bool m_lock=false;

    GatherSortKey m_primarySortKey=GatherSortKey::Inline;
};

#endif // SEISMICDATASELECTOR_H
