#ifndef AMPLITUDECURVEDEFINITION_H
#define AMPLITUDECURVEDEFINITION_H

struct AmplitudeCurveDefinition{

    QString dataset;
    QString horizon;
    int inlineNumber;
    int crosslineNumber;
    int inlineSize;
    int crosslineSize;
    double maximumOffset;
    double minimumAzimuth;
    double maximumAzimuth;
    double depth;
    QString reductionMethod;
    int windowSize;
};

Q_DECLARE_METATYPE(AmplitudeCurveDefinition)

#endif // AMPLITUDECURVEDEFINITION_H
