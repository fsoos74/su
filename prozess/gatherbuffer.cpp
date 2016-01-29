#include "gatherbuffer.h"

GatherBuffer::GatherBuffer(int firstIline, int firstXline, int ilineCount, int xlineCount):
    m_firstIline(firstIline), m_firstXline(firstXline), m_ilineCount(ilineCount), m_xlineCount(xlineCount), m_buf(ilineCount*xlineCount)
{

}


seismic::Gather GatherBuffer::gather( int iline, int xline, int ilSize, int xlSize){

    seismic::Gather gather;

    int beginIline=std::max( m_firstIline, iline);
    int endIline=std::min(m_firstIline+m_ilineCount, iline+ilSize);
    for( int il=beginIline; il<endIline; il++){

        int beginXline=std::max(m_firstXline, xline);
        int endXLine=std::min(m_firstXline+m_xlineCount, xline+xlSize);
        for( int xl=beginXline; xl<endXLine; xl++){


            if( !(*this)(il, xl) ) continue;

            const seismic::Gather& gt=*(*this)(il, xl);
            for( const seismic::Trace& trace : gt ){
                gather.push_back(trace);
            }
        }
    }

    return gather;
}

void GatherBuffer::advanceTo( int newFirstIline){

    // do nothing if no advance
    if( newFirstIline==m_firstIline) return;
//std::cout<<"ADVANCE firstIline="<<m_firstIline<<" new="<<newFirstIline<<std::endl;
    // copy overlap
    int nOverlap=m_firstIline+m_ilineCount-newFirstIline;
//std::cout<<"noverlap="<<nOverlap<<std::endl;
    if( nOverlap>0){
        int start=toIndex(newFirstIline, m_firstXline);
        int count=nOverlap*m_xlineCount;
//std::cout<<"start= "<<start<<" count="<<count<<std::endl;
        for( int i=0; i<count; i++){
            m_buf[i]=m_buf[start+i];
        }
    }

    // clear non overlapping fields
    int start=(nOverlap<1)? 0 : toIndex(m_firstIline+nOverlap, m_firstXline);
    for( int i=start; i<m_ilineCount*m_xlineCount; i++){
        m_buf[i].reset();
    }

    m_firstIline=newFirstIline;
}
