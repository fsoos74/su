#ifndef PROCESSPARAMS_H
#define PROCESSPARAMS_H

#include<QString>
#include<QMap>
#include<stdexcept>

typedef QMap<QString,QString> ProcessParams;

inline QString getParam(const ProcessParams& params, const QString& name ){
    if( !params.contains(name) ){
        throw std::runtime_error( QString("Parameters don't contain \"%1\"").arg(name).toStdString());
    }
    return params.value(name);
}

#endif // PROCESSPARAMS_H
