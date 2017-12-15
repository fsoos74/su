#ifndef AMPLITUDECURVEDEFINITION_H
#define AMPLITUDECURVEDEFINITION_H

struct AmplitudeCurveDefinition{

    QString dataset;
    QString horizon;
    int inlineNumber;
    int crosslineNumber;
    double time;
    int inlineSize;
    int crosslineSize;
    double minimumOffset;
    double maximumOffset;
    double minimumAzimuth;
    double maximumAzimuth;
    double depth;
};

Q_DECLARE_METATYPE(AmplitudeCurveDefinition)

#endif // AMPLITUDECURVEDEFINITION_H
