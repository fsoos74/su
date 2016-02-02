#ifndef AMPLITUDEOFFSETCURVE_H
#define AMPLITUDEOFFSETCURVE_H

#include <QVector>
#include <QPointF>
#include <memory>
#include <gather.h>
#include "windowreductionfunction.h"



QVector<QPointF> buildAmplitudeOffsetCurve( const seismic::Gather& gather,
                                              const double& windowCenterTime,
                                              ReductionFunction* rf, int windowSize );

QVector<QPointF> buildAmplitudeOffsetCurve( const seismic::Gather& gather, size_t sampleNumber);


void convertOffsetCurveToAngle( QVector<QPointF>& curve, const double& depth);



#endif // AMPLITUDEOFFSETCURVE_H

