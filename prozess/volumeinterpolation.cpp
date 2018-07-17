#include "volumeinterpolation.h"

//#define DUMP
#ifdef DUMP
#include<iostream>
#endif

// compute intermediate values for a volume that has values on w_lines/w_samples intervals
void interpolateIntermediate(Volume& v, int w_lines, int w_samples){

    auto bounds=v.bounds();

 #ifdef DUMP
    std::cout<<"w_lines:"<<w_lines<<" w_samples:"<<w_samples<<std::endl;
#endif

    for( int ii=0; ii<bounds.ni(); ii++){
        int i=bounds.i1()+ii;
        int i0=bounds.i1()+(ii/w_lines)*w_lines;
        int i1=std::min(bounds.i2(),i0+w_lines);

        for( int jj=0; jj<bounds.nj(); jj++){
            int j=bounds.j1()+jj;
            int j0=bounds.j1()+(jj/w_lines)*w_lines;
            int j1=std::min(bounds.j2(),j0+w_lines);

            for( int k=0; k<bounds.nt(); k++){      // k starts at 0, no need for kk
                int k0=(k/w_samples)*w_samples;
                int k1=std::min(bounds.nt()-1, k0+w_samples);

                // skip if we are on a computed point
                if( i==i0 && j==j0 && k==k0) continue;  // retain computed points

                auto v000=v(i0,j0,k0);
                if(v000==v.NULL_VALUE) continue;
                auto v100=v(i1,j0,k0);
                if(v100==v.NULL_VALUE) continue;
                auto v010=v(i0,j1,k0);
                if(v010==v.NULL_VALUE) continue;
                auto v110=v(i1,j1,k0);
                if(v110==v.NULL_VALUE) continue;
                auto v001=v(i0,j0,k1);
                if(v001==v.NULL_VALUE) continue;
                auto v101=v(i1,j0,k1);
                if(v101==v.NULL_VALUE) continue;
                auto v011=v(i0,j1,k1);
                if(v011==v.NULL_VALUE) continue;
                auto v111=v(i1,j1,k1);
                if(v111==v.NULL_VALUE) continue;

                double fi=double(i1-i)/(i1-i0);
                double vi00=fi*v000 + (1.-fi)*v100;
                double vi10=fi*v010 + (1.-fi)*v110;
                double vi01=fi*v001 + (1.-fi)*v101;
                double vi11=fi*v011 + (1.-fi)*v111;

                double fj=double(j1-j)/(j1-j0);
                double vij0=fj*vi00 + (1.-fj)*vi10;
                double vij1=fj*vi01 + (1.-fj)*vi11;

                double fk=double(k1-k)/(k1-k0);
                double vijk=fk*vij0 + (1.-fk)*vij1;

                v(i,j,k)=vijk;

#ifdef DUMP
                std::cout<<"i="<<i<<" j="<<j<<" k="<<k<<std::endl;
                std::cout<<"i0="<<i0<<" i1="<<i1<<" j0="<<j0<<" j1="<<j1<<" k0="<<k0<<" k1="<<k1<<std::endl;
                std::cout<<"v000="<<v000<<std::endl;
                std::cout<<"v100="<<v100<<std::endl;
                std::cout<<"v010="<<v010<<std::endl;
                std::cout<<"v110="<<v110<<std::endl;
                std::cout<<"v001="<<v001<<std::endl;
                std::cout<<"v101="<<v101<<std::endl;
                std::cout<<"v011="<<v011<<std::endl;
                std::cout<<"v111="<<v111<<std::endl;

                std::cout<<"fi="<<fi<<std::endl;
                std::cout<<"vi00="<<vi00<<std::endl;
                std::cout<<"vi10="<<vi10<<std::endl;
                std::cout<<"vi01="<<vi01<<std::endl;
                std::cout<<"vi11="<<vi11<<std::endl;

                std::cout<<"fj="<<fj<<std::endl;
                std::cout<<"vij0="<<vij0<<std::endl;
                std::cout<<"vij1="<<vij1<<std::endl;

                std::cout<<"fk="<<fk<<std::endl;
                std::cout<<"vijk="<<vijk<<std::endl;
#endif

            }
        }
    }
}

