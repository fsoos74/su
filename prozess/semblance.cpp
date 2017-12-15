#include "semblance.h"

#include <grid3d.h>

double semblance( const seismic::Gather& g, size_t halfSamples, double tt){

    const size_t& hw=halfSamples;

    size_t N=g.size();

    if( N<2 ) return 1;

    std::vector<double> st;
    st.assign( 2*hw + 1 , 0);
    double fnt2=0;

    for( size_t n=0; n<N; n++ ){
        const seismic::Trace& trc=g[n];
        if( trc.size()<1 )continue;
        int iline=trc.header().at("iline").intValue();
        int xline=trc.header().at("xline").intValue();
        //double tt=0.001 * (*m_horizon)( iline, xline );
        size_t tn(trc.time_to_index( tt ));
        size_t t1((tn>=hw)?tn - hw : 0);
        size_t t2((tn+hw<trc.size()) ? tn+hw : trc.size()-1);
/*
    if( tn<hw || tn + hw>=trc.size() ) continue;
    size_t t1=tn-hw;
    size_t t2=tn+hw;
*/
        for( size_t t=t1; t<=t2; t++){
            const double& ft=trc[t];
            st[t-t1]+=ft;
            fnt2+=ft*ft;
        }
    }

    double s2=0;
    for( auto s : st ){
        s2+=s*s;
    }

    if( !fnt2 ) return -1;

    return ( s2 - fnt2)/( (N-1)*fnt2);
}

double semblance( const Grid3D<float>& vol, const std::vector<std::pair<int,int>>& locations, int halfSamples, int k){

    const size_t& hw=halfSamples;

    size_t N=locations.size();

    if( N<2 ) return 1;

    std::vector<double> st;
    st.assign( 2*hw + 1 , 0);
    double fnt2=0;

    int k1((k>=hw)?k - hw : 0);
    int k2((k+hw<vol.bounds().nt()) ? k+hw : vol.bounds().nt()-1);

    for( auto loc : locations ){

        const float* sam=&vol(loc.first, loc.second, 0);

        for( auto t=k1; t<=k2; t++){
            const double& ft=sam[t];
            st[t-k1]+=ft;
            fnt2+=ft*ft;
        }
    }

    double s2=0;
    for( auto s : st ){
        s2+=s*s;
    }

    if( !fnt2 ) return -1;

    return ( s2 - fnt2)/( (N-1)*fnt2);
}

double semblance( const Grid3D<float>& vol, const std::vector<std::tuple<int,int,double>>& path, int hw){

    size_t N=path.size();

    if( N<2 ) return 1;

    std::vector<double> st;//( 2*hw + 1 , 0);
    st.assign( 2*hw + 1 , 0);
    //double st[2*hw+1];
    //std::fill(st, st+2*hw,0);
    double fnt2=0;

    //int k1((k>=hw)?k - hw : 0);
    //int k2((k+hw<vol.bounds().nt()) ? k+hw : vol.bounds().nt()-1);

    for( auto loc : path ){

        for( auto it=-hw; it<=hw; it++){

            double t=std::get<2>(loc) + it*vol.dt();
            //std::cout<<"i="<<std::get<0>(loc)<<" j="<<std::get<1>(loc)<<" t="<<t<<std::endl<<std::flush;
            double ft=vol.value(std::get<0>(loc), std::get<1>(loc), t );

            //std::cout<<"i="<<std::get<0>(loc)<<" j="<<std::get<1>(loc)<<" t="<<t<<" ft="<<ft<<std::endl<<std::flush;

            if( ft==vol.NULL_VALUE) return 0;//vol.NULL_VALUE;
            st[hw + it]+=ft;
            fnt2+=ft*ft;
        }
    }

    double s2=0;
    for( auto s : st ){
        s2+=s*s;
    }

    if( !fnt2 ) return 0; //vol.NULL_VALUE;

    return ( s2 - fnt2)/( (N-1)*fnt2);
}

