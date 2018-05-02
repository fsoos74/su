#ifndef PICKER_H
#define PICKER_H

#include <QObject>
#include <gather.h>
#include <table.h>
#include <memory>
#include <functional>

#include "picktype.h"
#include "pickmode.h"
#include "pickfillmode.h"


class Picker : public QObject
{
    Q_OBJECT
public:

    explicit Picker(QObject *parent = 0);

    std::shared_ptr<seismic::Gather> gather()const{
        return m_gather;
    }

    std::shared_ptr<Table> picks()const{
        return m_picks;
    }

    PickMode mode()const{
        return m_mode;
    }

    PickType type()const{
        return m_type;
    }

    PickFillMode fillMode()const{
        return m_fillMode;
    }

    bool isConservative()const{
        return m_conservative;
    }

    bool isDirty()const{
        return m_dirty;
    }

    std::vector<std::pair<int,float>> bufferedPoints()const{
        return m_bufferedPoints;
    }

    void deleteSingle( int traceNo, float secs);

signals:

    void gatherChanged();
    void picksChanged();
    void bufferedPointsChanged();
    void modeChanged(PickMode);
    void typeChanged(PickType);
    void fillModeChanged(PickFillMode);
    void conservativeChanged( bool );

public slots:

    void newPicks( const QString& key1, const QString& key2, bool multi);
    void setGather( std::shared_ptr<seismic::Gather> );
    void setPicks( std::shared_ptr<Table> );
    void setMode( PickMode );
    void setType( PickType );
    void setFillMode( PickFillMode );
    void setConservative( bool );

    void pick( int traceNo, float t );
    void deletePick( int traceNo, float t );
    void setDirty(bool);
    void clearBuffer();
    void finishedBuffer();

private:

    float pick1( int traceNo, float secs, bool adjust=true);  // helper, does not check valid traceno and does not emit change
    void pickSingle( int firstTraceNo, float firstTraceTime);
    void pickFillLeftNext( int firstTraceNo, float firstTraceTime);
    void pickFillLeftNearest( int firstTraceNo, float firstTraceTime);
    void pickFillRightNearest( int firstTraceNo, float firstTraceTime);
    void pickFillRightNext( int firstTraceNo, float firstTraceTime);
    void pickLines( int traceNo, float secs);
    void pickOutline( int traceNo, float secs);
    void fillLines();
    void fillOutline();
    bool delete1(int traceNo, float secs);  // helper, does not check valid traceno and does not emit change
    void deleteLeft( int traceNo, float secs );
    void deleteRight( int traceNo, float secs );
    void deleteBuffered( int traceNo, float secs);

    float adjustPick( const seismic::Trace&, float t);
    float adjustMinimum( const seismic::Trace&, float t);
    float adjustMaximum( const seismic::Trace&, float t);
    float adjustZero( const seismic::Trace&, float t);
    float adjustNone( const seismic::Trace&, float t);

    void fillILXLBuffer();
    void updateModeFuncs();
    void updateTypeFunc();

    int traceIline(int i)const{ return std::get<0>(m_ilxluseBuffer[i]);}
    int traceXline(int i)const{ return std::get<1>(m_ilxluseBuffer[i]);}
    bool useTrace(int i)const{ return std::get<2>(m_ilxluseBuffer[i]);}

    std::shared_ptr<seismic::Gather> m_gather;
    std::shared_ptr<Table>  m_picks;

    PickMode m_mode=PickMode::Single;
    PickType m_type=PickType::Minimum;
    PickFillMode m_fillMode=PickFillMode::Nearest;
    bool m_conservative=false;
    int m_correlationWindow=11;
    int m_searchWindow=11;

    std::vector<std::tuple<int,int,bool>> m_ilxluseBuffer;
    std::function<void (int,float)> pickFunc;
    std::function<void (int,float)> deletePickFunc;
    std::function<float( const seismic::Trace&, float)> adjustFunc;

    bool m_dirty=false;

    bool m_outline=false;
    std::vector<std::pair<int,float>> m_bufferedPoints;
};


#endif // PICKER_H
