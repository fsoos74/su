#ifndef PROCESSPARAMS_H
#define PROCESSPARAMS_H

#include<QString>
#include<QMap>
#include<stdexcept>


typedef QMap<QString,QString> ProcessParams;

inline QString getParam(const ProcessParams& params, const QString& name, bool required=true, const QString& def="" ){
    if( !params.contains(name) ){
        if( required ){
            throw std::runtime_error( QString("Parameters don't contain \"%1\"").arg(name).toStdString());
        }
        return def;
    }
    return params.value(name);
}

inline QString packParamList( QStringList l){
    return l.join("///");
}

inline QStringList unpackParamList( QString s){
    return s.split( "///", QString::KeepEmptyParts);
}

#endif // PROCESSPARAMS_H
