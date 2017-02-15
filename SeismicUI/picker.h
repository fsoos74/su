#ifndef PICKER_H
#define PICKER_H

#include <QObject>
#include <gather.h>
#include <grid2d.h>
#include <memory>
#include <functional>

#include "picktype.h"
#include "pickmode.h"

class Picker : public QObject
{
    Q_OBJECT
public:

    explicit Picker(QObject *parent = 0);

    std::shared_ptr<seismic::Gather> gather()const{
        return m_gather;
    }

    std::shared_ptr<Grid2D<float>> picks()const{
        return m_picks;
    }

    PickMode mode()const{
        return m_mode;
    }

    PickType type()const{
        return m_type;
    }

    bool isConservative()const{
        return m_conservative;
    }

    bool isDirty()const{
        return m_dirty;
    }

signals:

    void gatherChanged();
    void picksChanged();
    void modeChanged(PickMode);
    void typeChanged(PickType);
    void conservativeChanged( bool );

public slots:

    void setGather( std::shared_ptr<seismic::Gather> );
    void setPicks( std::shared_ptr<Grid2D<float>> );
    void setMode( PickMode );
    void setType( PickType );
    void setConservative( bool );

    void pick( int traceNo, float t );
    void deletePick( int traceNo );

    void setDirty(bool);

private:

    float pick1( int traceNo, float secs);  // helper, does not check valid traceno and does not emit change
    void pickSingle( int firstTraceNo, float firstTraceTime);
    void pickFillLeft( int firstTraceNo, float firstTraceTime);
    void pickFillRight( int firstTraceNo, float firstTraceTime);
    void pickFillAll( int firstTraceNo, float firstTraceTime);
    bool delete1(int traceNo);  // helper, does not check valid traceno and does not emit change
    void deleteSingle( int traceNo);
    void deleteLeft( int traceNo );
    void deleteRight( int traceNo );
    void deleteAll( int traceNo );

    float adjustPick( const seismic::Trace&, float t);
    float adjustMinimum( const seismic::Trace&, float t);
    float adjustMaximum( const seismic::Trace&, float t);
    float adjustZero( const seismic::Trace&, float t);
    float adjustNone( const seismic::Trace&, float t);

    void updateModeFuncs();
    void updateTypeFunc();

    std::shared_ptr<seismic::Gather> m_gather;
    std::shared_ptr<Grid2D<float>>   m_picks;

    PickMode m_mode=PickMode::Single;
    PickType m_type=PickType::Minimum;
    bool m_conservative=true;
    std::function<void (int,float)> pickFunc;
    std::function<void (int)> deletePickFunc;
    std::function<float( const seismic::Trace&, float)> adjustFunc;

    bool m_dirty=false;
};


#endif // PICKER_H
