#ifndef HORIZONWINDOWPOSITION_H
#define HORIZONWINDOWPOSITION_H


#include<QString>


enum class HorizonWindowPosition{
    Above, Center, Below
};


HorizonWindowPosition toHorizonWindowPosition( QString );
QString toQString( HorizonWindowPosition );

#endif // HORIZONWINDOWPOSITION_H
