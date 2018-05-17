#ifndef LINEARREGRESSION_H
#define LINEARREGRESSION_H

#include<QVector>
#include<QPointF>


// resuld x=intercept, y=gradient
QPointF linearRegression( const QVector<QPointF>&, double* quality=nullptr);
QPointF linearRegression( const float* x, const float* y, qint64 n, const float& NULL_VALUE, double* quality=nullptr);

#endif // LINEARREGRESSION_H

