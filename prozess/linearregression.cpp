#include "linearregression.h"

#include<iostream>

// result is intercept/gradient
QPointF linearRegression( const QVector<QPointF>& points){

    double intercept=0;
    double gradient=0;
    double sum_x=0;
    double sum_x_x=0;
    double sum_x_y=0;
    double sum_y=0;
    int n=0;

    for( auto point : points ){

       const double& x=point.x();
       const double& y=point.y();

        sum_x+=x;
        sum_x_x+=x*x;
        sum_x_y+=x*y;
        sum_y+=y;
        n++;

    }

    if( n>0 ){

        gradient=( sum_x_y-sum_x*sum_y/n ) / ( sum_x_x - sum_x*sum_x/n);
        intercept=sum_y/n - gradient*sum_x/n;
        // original version:
        //intercept = ( n*sum_x_y - sum_x * sum_y ) / ( n*sum_x_x - sum_x*sum_x);
        //gradient = ( sum_x_x*sum_y - sum_x*sum_x_y) / ( n*sum_x_x - sum_x*sum_x);
    }


    return QPointF(intercept, gradient);
}


