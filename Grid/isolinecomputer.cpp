#include "isolinecomputer.h"


#include<iostream>
#include<iomanip>
#include<array>

#include<tree2d.h>

const std::array<int,4> CODES{ 0x01, 0x02, 0x04, 0x08 };
const std::array<int,4> DELTA_I{ 1, 1, 0, 0 };
const std::array<int,4> DELTA_J{ 0, 1, 1, 0 };




IsoLineComputer::IsoLineComputer( const Grid2D<float>& g, const double& thres ): grid(g), threshold(thres){
}


QVector<QLineF> IsoLineComputer::compute( const Grid2D<float>& grid, const double threshold){

    IsoLineComputer ilc( grid, threshold);
    ilc.compute();
    return ilc.segments;
}

void IsoLineComputer::dumpCell( int i, int j){

   std::cout<<std::setw(8)<<grid( i , j )<<" ";
   std::cout<<std::setw(8)<<grid( i , j + 1)<<std::endl;
   std::cout<<std::setw(8)<<grid( i + 1 , j )<<" ";
   std::cout<<std::setw(8)<<grid( i + 1, j + 1)<<std::endl;
   std::cout<<std::flush;
}

void IsoLineComputer::compute(  ){


    segments.clear();

    int i1=grid.bounds().i1();
    int j1=grid.bounds().j1();
    int i2=grid.bounds().i2();
    int j2=grid.bounds().j2();

    for( int i=i1; i< i2; i++){

        for( int j=j1; j<j2; j++){

            //if( segments.size()>0)break;

            //std::cout<<i<<" "<<j<<std::endl;

            //dumpCell(i,j);

            int code=0;

            for( int k=0; k<4; k++){

                //std::cout<<"grid( "<<i+DELTA_I[k]<<", "<<j+DELTA_J[k]<<std::endl;
                double value=grid(i+DELTA_I[k], j+DELTA_J[k] );
                if( value==grid.NULL_VALUE){
                    code=-1;
                    break;
                }
                if( value>threshold ){
                    code |= CODES[k];
                }
            }

            if( code<0 ) continue;      // one vertex is NULL



            switch( code ){

            // all vertices below threshold
            case 0x00:
                continue;
                break;

            // 1 vertex above threshold
            case 0x01:
                compute_1( i, j, 0, 1, 3);
                break;
            case 0x02:
                 compute_1( i, j, 1, 0, 2);
                break;
            case 0x04:
                 compute_1( i, j, 2, 3, 1);
                break;
            case 0x08:
                compute_1( i, j, 3, 2, 0 );
                break;

            // 2 adjacent vertices above threshold
            case 0x03:
            case 0x0c:
                compute_2_j( i, j);
                break;
            case 0x06:
            case 0x09:
                compute_2_i( i, j);
                break;

            // 2 opposite vertices above threshold
            case 0x05:
                compute_2_a( i, j );
                break;
            case 0x0a:
                compute_2_a( i, j );
                break;


            // 3 vertices above threshold
            case 0x07:
                compute_1( i, j, 3, 2, 0);
                break;
            case 0x0b:
                compute_1( i, j, 2, 3, 1);
                break;
            case 0x0d:
                compute_1( i, j, 1, 0, 2);
                break;
            case 0x0e:
                compute_1( i, j, 0, 1, 3);
               break;

            case 0x0f:                      // all vertices above threhold
                        continue;
                        break;

            default:
                qFatal( QString::asprintf("Illegal vertex code i=%d j=%d code=%x\n", i, j, code).toStdString().c_str() );
                break;
            }

        }

    }

}


inline double interp( double x1, double y1, double x2, double y2, double x){

    double y= y1 + ( x - x1 ) * ( y2 - y1 ) / ( x2 - x1);

    return y;
}



// 1 vertex is greater than threhold, idx_gt
// lines intesect cell between idx_gt and idx_i with same i AND between idx_gt and idx_j with same j
void IsoLineComputer::compute_1( int i, int j, int idx_gt, int idx_i, int idx_j){



    double val_gt= grid(  i + DELTA_I[idx_gt], j+ DELTA_J[ idx_gt]  );
    double val_i= grid( i + DELTA_I[idx_i], j+ DELTA_J[ idx_i] );
    double val_j= grid( i + DELTA_I[idx_j] , j+ DELTA_J[ idx_j] );

    //QPointF intersect_i( i + DELTA_I[idx_gt ], double(j) + interp( val_gt, DELTA_J[idx_gt], val_i, DELTA_J[idx_i], threshold) );
    //QPointF intersect_j( double(i) + interp( val_gt, DELTA_I[idx_gt], val_j, DELTA_I[idx_j], threshold), j + DELTA_J[idx_gt] );

    // i:vertical=y, j:horizontal=x
    QPointF intersect_i( double(j) + interp( val_gt, DELTA_J[idx_gt], val_i, DELTA_J[idx_i], threshold), i + DELTA_I[idx_gt ]  );
    QPointF intersect_j( j + DELTA_J[idx_gt], double(i) + interp( val_gt, DELTA_I[idx_gt], val_j, DELTA_I[idx_j], threshold) );

    segments.push_back( QLineF( intersect_i, intersect_j) );
/*
    std::cout<<"Compute_1"<<std::endl;
    std::cout<<"val_gt="<<val_gt<<std::endl;
    std::cout<<"val_i="<<val_i<<std::endl;
    std::cout<<"val_j="<<val_j<<std::endl;
    std::cout<<"intersect_i: i="<< intersect_i.x()<<" j="<<intersect_i.y()<<std::endl;
    std::cout<<"intersect_j: i="<< intersect_j.x()<<" j="<<intersect_j.y()<<std::endl;
*/
}


// 2 adjacent vertices greater than threshold
// line intersects horizontal cell bounds between vertices 0 and 1  AND vertices 3 and 2
void IsoLineComputer::compute_2_i( int i, int j ){


    const int idx_1_1=0;
    const int idx_1_2=1;
    const int idx_2_1=3;
    const int idx_2_2=2;

    Q_ASSERT( DELTA_I[idx_1_1] == DELTA_I[idx_1_2]);
    Q_ASSERT( DELTA_I[idx_2_1] == DELTA_I[idx_2_2]);

    double val_1_1= grid( i + DELTA_I[idx_1_1], j+ DELTA_J[ idx_1_1] );
    double val_1_2= grid(  i + DELTA_I[idx_1_2], j+ DELTA_J[ idx_1_2]);
    double val_2_1= grid( i + DELTA_I[idx_2_1], j+ DELTA_J[ idx_2_1] );
    double val_2_2= grid( i + DELTA_I[idx_2_2], j+ DELTA_J[ idx_2_2] );


    //QPointF intersect_1( i + DELTA_I[idx_1_1], double(j) + interp( val_1_1, DELTA_J[idx_1_1], val_1_2, DELTA_J[idx_1_2], threshold) );
    //QPointF intersect_2( i + DELTA_I[idx_2_1], double(j) + interp( val_2_1, DELTA_J[idx_2_1], val_2_2, DELTA_J[idx_2_2], threshold) );

    // i:vertical=y, j:horizontal=x
    QPointF intersect_1( double(j) + interp( val_1_1, DELTA_J[idx_1_1], val_1_2, DELTA_J[idx_1_2], threshold), i + DELTA_I[idx_1_1] );
    QPointF intersect_2( double(j) + interp( val_2_1, DELTA_J[idx_2_1], val_2_2, DELTA_J[idx_2_2], threshold), i + DELTA_I[idx_2_1] );


    segments.push_back( QLineF( intersect_1, intersect_2) );
}


// 2 adjacent vertices greater than threshold
// line intersects vertical cell bounds between vertices 0 and 3  AND vertices 1 and 2
void IsoLineComputer::compute_2_j( int i, int j ){

    const int idx_1_1=0;
    const int idx_1_2=3;
    const int idx_2_1=1;
    const int idx_2_2=2;

    Q_ASSERT( DELTA_J[idx_1_1] == DELTA_J[idx_1_2]);
    Q_ASSERT( DELTA_J[idx_2_1] == DELTA_J[idx_2_2]);

    double val_1_1= grid(  i + DELTA_I[idx_1_1], j+ DELTA_J[ idx_1_1]);
    double val_1_2= grid( i + DELTA_I[idx_1_2], j+ DELTA_J[ idx_1_2] );
    double val_2_1= grid(  i + DELTA_I[idx_2_1] , j+ DELTA_J[ idx_2_1] );
    double val_2_2= grid(  i + DELTA_I[idx_2_2] , j+ DELTA_J[ idx_2_2]);


    //QPointF intersect_1( double(i) + interp( val_1_1, DELTA_I[idx_1_1], val_1_2, DELTA_I[idx_1_2], threshold), j + DELTA_J[idx_1_1] );
    //QPointF intersect_2( double(i) + interp( val_2_1, DELTA_I[idx_2_1], val_2_2, DELTA_I[idx_2_2], threshold), j + DELTA_J[idx_2_1] );

    // i:vertical=y, j:horizontal=x
    QPointF intersect_1( j + DELTA_J[idx_1_1], double(i) + interp( val_1_1, DELTA_I[idx_1_1], val_1_2, DELTA_I[idx_1_2], threshold) );
    QPointF intersect_2( j + DELTA_J[idx_2_1], double(i) + interp( val_2_1, DELTA_I[idx_2_1], val_2_2, DELTA_I[idx_2_2], threshold) );


    segments.push_back( QLineF( intersect_1, intersect_2) );

}


// 2 opposite vertices greater than threshold
// ambiguos case
void IsoLineComputer::compute_2_a( int i, int j ){

    double val_0=grid( i + DELTA_I[0], j + DELTA_J[0]);
    double val_1=grid( i + DELTA_I[1], j + DELTA_J[1]);
    double val_2=grid( i + DELTA_I[2], j + DELTA_J[2]);
    double val_3=grid( i + DELTA_I[3], j + DELTA_J[3]);

    // compute central value
    double avg=(val_0 + val_1 + val_2 + val_3)/4;


    if( val_0 < threshold ){

        if( avg < threshold ){
            compute_1( i, j, 1, 0, 2);
            compute_1( i, j, 3, 2, 0);
        }
        else{
            compute_1( i, j, 2, 3, 1);
            compute_1( i, j, 0, 1, 3);
        }

    }
    else{

        if( avg < threshold ){
            compute_1( i, j, 0, 1, 3);
            compute_1( i, j, 2, 3, 1);
        }
        else{
            compute_1( i, j, 3, 2, 0);
            compute_1( i, j, 1, 0, 2);
        }


    }

}


void create_polygons( QVector< QLineF > segments ){



}
