#ifndef HORIZONDEF_H
#define HORIZONDEF_H

#include <grid2d.h>
#include <memory>
#include <QString>
#include <QColor>


struct HorizonParameters
{
    bool                            useColor;
    QColor                          color;
};


inline bool operator==(const HorizonParameters& def1, const HorizonParameters& def2){

    return def1.useColor == def2.useColor &&
            def1.color == def2.color ;
}

#endif // HORIZONDEF_H
