#ifndef GATHERBUFFER_H
#define GATHERBUFFER_H

#include<QVector>
#include<memory>
#include<gather.h>


class GatherBuffer
{
public:
    GatherBuffer( int firstIline, int firstXline, int ilineCount, int xlineCount);

    int firstIline()const{
        return m_firstIline;
    }

    int ilineCount()const{
        return m_ilineCount;
    }

    int firstXline()const{
        return m_firstXline;
    }

    int xlineCount()const{
        return m_xlineCount;
    }

    int lastIline()const{
        return m_firstIline+m_ilineCount-1;
    }

    const std::shared_ptr<seismic::Gather>& operator()(int iline, int xline)const{
        return m_buf[toIndex(iline, xline)];
    }

    std::shared_ptr<seismic::Gather>& operator()(int iline, int xline){
        return m_buf[toIndex(iline, xline)];
    }

    const std::shared_ptr<seismic::Gather>& at(int iline, int xline)const{
        return m_buf.at(toIndex(iline, xline));
    }

    seismic::Gather gather( int iline, int xline, int ilSize=1, int xlSize=1);

    void advanceTo( int newFirstIline);

private:

    int toIndex( int iline, int xline)const{
        return (iline-m_firstIline)*m_xlineCount+xline-m_firstXline;
    }

    int m_firstIline;
    int m_firstXline;
    int m_ilineCount;
    int m_xlineCount;

    QVector<std::shared_ptr<seismic::Gather>> m_buf;
};

#endif // GATHERBUFFER_H
