#include "rotatinggatherbuffer.h"

#include<iostream>

RotatingGatherBuffer::RotatingGatherBuffer( int firstInline, int firstCrossline, int crosslineCount, int inlineBinSize, int crosslineBinSize, QObject* parent):
    QObject(parent),
    m_firstInline(firstInline),
    m_firstCrossline(firstCrossline),
    m_crosslineCount(crosslineCount),
    m_inlineBinSize(inlineBinSize),
    m_crosslineBinSize(crosslineBinSize),
    m_buffer( inlineBinSize*crosslineCount)
{

}

std::shared_ptr<seismic::Gather> RotatingGatherBuffer::buildGather(int iline, int xline)const{

    // frequent special case of 1 by 1 supergather, i.e. regular gather
    if( m_inlineBinSize==1 && m_crosslineBinSize==1){
        int idx=(iline-m_firstInline)*m_crosslineCount + xline-m_firstCrossline;
        std::cout<<"iline="<<iline<<"/"<<m_firstInline<<" xline="<<xline<<" idx="<<idx<<std::endl<<std::flush;
        return m_buffer[ idx ];
    }

    std::shared_ptr<seismic::Gather> gather(new seismic::Gather);

    for( int ii=0; ii<m_inlineBinSize; ii++){

        for( int jj=0; jj<m_crosslineBinSize; jj++){

            std::shared_ptr<seismic::Gather> gt=m_buffer[ ii*m_crosslineCount + xline-m_firstCrossline + jj];
            if( ! gt ) continue;
            for( const seismic::Trace& trc: *gt ){
                gather->push_back(trc);
            }
        }
    }

    return gather;
}

void RotatingGatherBuffer::add( std::shared_ptr<seismic::Gather> gather){

    if( !gather ) return;

    const seismic::Header& header=gather->front().header();
    int iline=header.at("iline").intValue();
    int xline=header.at("xline").intValue();

    if( iline<m_firstInline ){
        throw BufferException(QString("Dataset is not inline sorted! "
                                "inline=%1 crossline=%2 ").arg(iline).arg(xline) );
    }

    while( iline>=m_firstInline + m_inlineBinSize ){

        std::cout<<"WE ARE FULL: iline="<<iline<<" first="<<m_firstInline<<std::endl<<std::flush;
        emit full();
        std::cout<<"Before advance"<<std::endl<<std::flush;
        advance();
        iline++;
        std::cout<<"After advance"<<std::endl<<std::flush;
    }

    int idx=(iline-m_firstInline)*m_crosslineCount+xline-m_firstCrossline;
    if( idx<0 || idx>=m_buffer.size()){
        std::cout<<"iline="<<iline<<"/"<<m_firstInline<<" xline="<<xline<<" idx="<<idx<<" outside of buffer!"<<std::endl;
        return;
    }
    m_buffer[idx]=gather;
}


void RotatingGatherBuffer::advance(){

    for( int ii=1; ii<m_inlineBinSize; ii++){

        for(int jj=0; jj<m_crosslineCount; jj++){

            int idxOld=ii*m_crosslineCount+jj;
            int idxNew=(ii-1)*m_crosslineCount+jj;

            m_buffer[idxNew]=m_buffer[idxOld];
        }
    }

    m_firstInline++;
}

