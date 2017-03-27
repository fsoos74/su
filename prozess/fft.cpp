#include<fft.h>

#include <cmath>

#include<cstring>  // memcpy

namespace{

// these are from numerical recipes in C

void four1(float* data, unsigned long nn, int isign)
{
    unsigned long n,mmax,m,j,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    float tempr,tempi;

    n=nn << 1;
    j=1;
    for (i=1;i<n;i+=2) {
        if (j > i) {
            std::swap(data[j],data[i]);
            std::swap(data[j+1],data[i+1]);
        }
        m=n >> 1;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    mmax=2;
    while (n > mmax) {
        istep=mmax << 1;
        theta=isign*(6.28318530717959/mmax);
        wtemp=sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi=sin(theta);
        wr=1.0;
        wi=0.0;
        for (m=1;m<mmax;m+=2) {
            for (i=m;i<=n;i+=istep) {
                j=i+mmax;
                tempr=wr*data[j]-wi*data[j+1];
                tempi=wr*data[j+1]+wi*data[j];
                data[j]=data[i]-tempr;
                data[j+1]=data[i+1]-tempi;
                data[i] += tempr;
                data[i+1] += tempi;
            }
            wr=(wtemp=wr)*wpr-wi*wpi+wr;
            wi=wi*wpr+wtemp*wpi+wi;
        }
        mmax=istep;
    }
}


void realft(float* data, unsigned long n, int isign)
{
    void four1(float* data, unsigned long nn, int isign);
    unsigned long i,i1,i2,i3,i4,np3;
    float c1=0.5,c2,h1r,h1i,h2r,h2i;
    double wr,wi,wpr,wpi,wtemp,theta;

    theta=3.141592653589793/(double) (n>>1);
    if (isign == 1) {
        c2 = -0.5;
        four1(data,n>>1,1);
    } else {
        c2=0.5;
        theta = -theta;
    }
    wtemp=sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi=sin(theta);
    wr=1.0+wpr;
    wi=wpi;
    np3=n+3;
    for (i=2;i<=(n>>2);i++) {
        i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
        h1r=c1*(data[i1]+data[i3]);
        h1i=c1*(data[i2]-data[i4]);
        h2r = -c2*(data[i2]+data[i4]);
        h2i=c2*(data[i1]-data[i3]);
        data[i1]=h1r+wr*h2r-wi*h2i;
        data[i2]=h1i+wr*h2i+wi*h2r;
        data[i3]=h1r-wr*h2r+wi*h2i;
        data[i4] = -h1i+wr*h2i+wi*h2r;
        wr=(wtemp=wr)*wpr-wi*wpi+wr;
        wi=wi*wpr+wtemp*wpi+wi;
    }
    if (isign == 1) {
        data[1] = (h1r=data[1])+data[2];
        data[2] = h1r-data[2];
    } else {
        data[1]=c1*((h1r=data[1])+data[2]);
        data[2]=c1*(h1r-data[2]);
        four1(data,n>>1,-1);
    }
}

};


std::vector< std::complex<float> > fft( const std::vector<float>& in ){

    std::vector< std::complex<float> > out(in.size()/2);    // half size because out has real and imaginary
    memcpy( &out[0], &in[0], in.size() * sizeof(float) );   // LEGAL per standard complex is 2 consecutive floats

    realft( reinterpret_cast<float*>(&out[0])-1, in.size(), 1 );        // make 1-based by -1, routine requires 1-based

    return out;
}


std::vector<float> ifft( const std::vector< std::complex<float> >& in ){

    std::vector<float> out(2*in.size());
    auto N = out.size();
    std::memcpy( &out[0], &in[0], N * sizeof(float) );  // LEGAL see above

    realft( &out[0]-1, out.size(), -1 );       // make 1-based by -1
    for( auto& x : out ){
        x *= 2.f/N;
    }

    return out;
}

// frequencies in fft transform of real data with N samples, this has only the positive half, thus N/2 freqs
std::vector<float> fft_freqs( const float& DT, unsigned N ){

    std::vector<float> f(N/2);

    for( unsigned i = 0; i<f.size(); i++){

        f[i]=static_cast<float>(i)/(N*DT);
    }

    return f;
}
