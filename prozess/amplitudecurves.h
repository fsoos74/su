#ifndef AMPLITUDEOFFSETCURVE_H
#define AMPLITUDEOFFSETCURVE_H

#include <QVector>
#include <QPointF>
#include <memory>
#include <gather.h>
#include "windowreductionfunction.h"


QVector<QPointF> buildAmplitudeOffsetCurve( const seismic::Gather& gather, size_t sampleNumber, const double& maximumOffset);


QVector<QPointF> buildAmplitudeOffsetCurve( const seismic::Gather& gather,
                                              const double& windowCenterTime,
                                              const double& maximumOffset,
                                              ReductionFunction* rf, int windowSize );


void convertOffsetCurveToAngle( QVector<QPointF>& curve, const double& depth);


QVector<QPointF> buildAmplitudeAngleCurve( const seismic::Gather& gather, size_t sampleNumber,
                                           const double& maximumOffset, const double& depth);


QVector<QPointF> buildAmplitudeAngleCurve( const seismic::Gather& gather,
                                              const double& windowCenterTime,
                                              const double& maximumOffset,
                                              ReductionFunction* rf, int windowSize,
                                              const double& depth);

#endif // AMPLITUDEOFFSETCURVE_H

