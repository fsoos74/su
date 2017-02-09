#ifndef PICKER_H
#define PICKER_H

#include <QObject>
#include <gather.h>
#include <grid2d.h>
#include <memory>
#include <functional>

class Picker : public QObject
{
    Q_OBJECT
public:

    enum Mode{ MODE_SINGLE, MODE_MULTI };
    enum Type{ TYPE_MINIMUM, TYPE_MAXIMUM };

    explicit Picker(QObject *parent = 0);

    std::shared_ptr<seismic::Gather> gather()const{
        return m_gather;
    }

    std::shared_ptr<Grid2D<float>> picks()const{
        return m_picks;
    }

    Mode mode()const{
        return m_mode;
    }

    Type type()const{
        return m_type;
    }

signals:

    void gatherChanged();
    void picksChanged();
    void modeChanged(Mode);
    void typeChanged(Type);

public slots:

    void setGather( std::shared_ptr<seismic::Gather> );
    void setPicks( std::shared_ptr<Grid2D<float>> );
    void setMode( Mode );
    void setType( Type );

    void pick( int traceNo, float t );

private:

    void pickSingle( int firstTraceNo, float firstTraceTime);
    void pickMulti( int firstTraceNo, float firstTraceTime);
    float adjustPick( const seismic::Trace&, float t);
    float adjustMinimum( const seismic::Trace&, float t);
    float adjustMaximum( const seismic::Trace&, float t);

    void updateModeFunc();
    void updateTypeFunc();

    std::shared_ptr<seismic::Gather> m_gather;
    std::shared_ptr<Grid2D<float>>   m_picks;

    Mode m_mode=MODE_SINGLE;
    Type m_type=TYPE_MINIMUM;
    std::function<void (int,float)> pickFunc;
    std::function<float( const seismic::Trace&, float)> adjustFunc;
};

#endif // PICKER_H
