#ifndef SPECTRUMDEFINITION_H
#define SPECTRUMDEFINITION_H

struct SpectrumDefinition{

    QString dataset;
    int inlineNumber;
    int crosslineNumber;
    int windowCenterMS;    // milliseconds
    int windowSamples;     // length of window in samples
};

Q_DECLARE_METATYPE(SpectrumDefinition);

#endif // SPECTRUMDEFINITION_H
