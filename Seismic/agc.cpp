#include<agc.h>

#include<limits>

namespace seismic{

#include<iostream>
void agc(seismic::Trace::Samples& out, const seismic::Trace::Samples& in, size_t len){

    if( in.size()<len ){
        len=out.size();
    }

    auto len2=len>>1;

    double asum=0;
    size_t n=0;
    // add start
    for( size_t i=0; i<len2;i++){
        if(!in[i]) continue;
        asum+=std::abs(in[i]);
        n++;
    }

    for( size_t i=0; i<out.size(); i++){

        if(i>len2 && in[i-len2]){
            asum-=std::abs(in[i-len2]);
            n--;
        }
        if( i+len2<in.size() && in[i+len2]){
            asum+=std::abs(in[i+len2]);
            n++;
        }

        if( n>0 ){
            out[i]=n*in[i]/asum;
        }else{
            out[i]=in[i];
        }

        /* testing qc
        if( i==1000){
            double asum2=0;
            size_t n2=0;
            for( size_t j=0; j<len; j++){
                auto k=i+j-len2;
                if(in[k]){
                    asum2+=std::abs(in[k]);
                    n2++;
                }
            }
            std::cout<<"REF: i="<<i<<" asum="<<asum<<" n="<<n<<" asum2="<<asum2<<" n2="<<n2<<std::endl<<std::flush;
        }
        */
    }

}

/*original, works BUT SLOW
void agc(seismic::Trace::Samples& out, const seismic::Trace::Samples& in, size_t len){

    auto len2=len>>1;

    for( size_t i=0; i<out.size(); i++){

        size_t minj=(i>=len2) ? i-len2 : 0;
        size_t maxj=(i+len2<in.size()) ? i+len2 : in.size();
        size_t n=0;
        double asum=0;
        for( size_t j=minj; j<maxj; j++){
            if(in[j]!=0){
                asum+=std::abs(in[j]);
                n++;
            }
        }
        if( n>0 ){
            out[i]=n*in[i]/asum;
        }
        else{
            out[i]=in[i];
        }
    }

}
*/

std::shared_ptr<seismic::Trace> agc(const seismic::Trace& in, size_t len){

    auto res=std::make_shared<seismic::Trace>(in);
    if( res ) agc(res->samples(), in.samples(), len);
    return res;
}


std::shared_ptr<seismic::Gather> agc(const seismic::Gather& in, size_t len){

    auto res=std::make_shared<seismic::Gather>(in.size());
    if( res ){
        for( size_t k=0; k<in.size(); k++){
            (*res)[k]=in[k];
            agc( (*res)[k].samples(), in[k].samples(), len);
        }
    }
    return res;
};


}
