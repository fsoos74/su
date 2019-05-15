#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include "matrix.h"
#include <QImage>
#include<cmath>

namespace sliceviewer {

namespace imageprocessing{

Matrix<double> buildSharpenKernel( int size, double power);
QImage convolve( QImage src, Matrix<double> kernel);

}

}

#endif // IMAGEPROCESSOR_H
