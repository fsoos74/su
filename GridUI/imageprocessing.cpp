#include "imageprocessing.h"

namespace imageprocessing {

Matrix<double> buildSharpenKernel( int size, double power){

    Matrix<double> m(size,size, -power);
    m(size/2, size/2)=size*size-power;
    return m;
}

QImage convolve( QImage src, Matrix<double> kernel){

    QImage dest(src);//src.width(), src.height(), src.format());

    for( int i=kernel.rows()/2; i<src.height()-kernel.rows()/2; i++){

        for( int j=kernel.columns()/2; j<src.width()-kernel.columns()/2; j++){

           qreal sr=0;
           qreal sg=0;
           qreal sb=0;
           qreal sc=0;

            for( int ii=0; ii<kernel.rows(); ii++){

                for( int jj=0; jj<kernel.columns(); jj++){

                    auto rgb=src.pixel( j+jj-kernel.columns()/2, i+ii-kernel.rows()/2);
                    auto ciijj=kernel(ii,jj);
                    sc+=ciijj;
                    sr+=ciijj*qRed(rgb);
                    sg+=ciijj*qGreen(rgb);
                    sb+=ciijj*qBlue(rgb);
                }

            }
            auto r=std::max(0,std::min(255, static_cast<int>(std::round(sr/sc))));
            auto g=std::max(0,std::min(255, static_cast<int>(std::round(sg/sc))));
            auto b=std::max(0,std::min(255, static_cast<int>(std::round(sb/sc))));
            dest.setPixel(j,i, qRgb(r,g,b));
        }
    }

    return dest;
}
}
