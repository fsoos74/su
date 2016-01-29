#ifndef ROTATINGGATHERBUFFER_H
#define ROTATINGGATHERBUFFER_H

#include<QObject>

#include<QVector>
#include<gather.h>
#include<memory>

#include<stdexcept>

class RotatingGatherBuffer: public QObject
{
    Q_OBJECT
public:

    class BufferException : public std::runtime_error{
    public:
        BufferException( QString msg) : std::runtime_error(msg.toStdString()){}
    };

    RotatingGatherBuffer( int firstInline, int firstCrossline, int crosslineCount, int inlineBinSize=1, int crosslineBinSize=1, QObject* parent=nullptr);

    int firstInline()const{
        return m_firstInline;
    }

    int firstCrossline()const{
        return m_firstCrossline;
    }

    int crosslineCount()const{
        return m_crosslineCount;
    }

    int inlineBinSize()const{
        return m_inlineBinSize;
    }

    int crosslineBinSize()const{
        return m_crosslineBinSize;
    }

    std::shared_ptr<seismic::Gather> buildGather( int iline, int xline)const;

public slots:

    void add( std::shared_ptr<seismic::Gather>);

signals:

    void full();

private:

    void advance();

    int m_firstInline;
    int m_firstCrossline;
    int m_crosslineCount;
    int m_inlineBinSize;
    int m_crosslineBinSize;
    QVector<std::shared_ptr<seismic::Gather>> m_buffer;
};

#endif // ROTATINGGATHERBUFFER_H
