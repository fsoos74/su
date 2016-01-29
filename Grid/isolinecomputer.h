#ifndef ISOLINECOMPUTER_H
#define ISOLINECOMPUTER_H

#include<memory>
#include<QLineF>
#include<QVector>

#include "grid2d.h"


class IsoLineComputer
{
public:

    IsoLineComputer( const Grid2D<double>& g, const double& thres);

    static QVector<QLineF> compute( const Grid2D<double>& grid, const double threshold);

private:

    void dumpCell( int i,int j);
    void compute();

    void compute_1( int i, int j, int idx_gt, int idx_i, int idx_j);

    void compute_2_i( int i, int j );

    void compute_2_j( int i, int j );

    void compute_2_a( int i, int j );

    const Grid2D<double>& grid;
    double threshold;
    QVector< QLineF > segments;
};

#endif // ISOLINECOMPUTER_H
