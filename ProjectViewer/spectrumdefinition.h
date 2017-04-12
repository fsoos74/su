#ifndef SPECTRUMDEFINITION_H
#define SPECTRUMDEFINITION_H

struct SpectrumDefinition{

    QString dataset;
    int inlineNumber;
    int crosslineNumber;
    int windowStartMS;     // milliseconds
    int windowLengthMS;    // milliseconds
};

Q_DECLARE_METATYPE(SpectrumDefinition);

#endif // SPECTRUMDEFINITION_H
