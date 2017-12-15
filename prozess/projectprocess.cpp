#include "projectprocess.h"

#include<QDateTime>

ProjectProcess::ProjectProcess( const QString& name, AVOProject* project, QObject *parent)
    : QObject(parent), m_name(name), m_project(project)
{
    if( !project ) throw ParamException("Invalid project!");
}

void ProjectProcess::cancel(){

    m_canceled=true;
}


void ProjectProcess::setErrorString( const QString& err){
    m_errorString=err;
    emit error(m_errorString);
}

void ProjectProcess::setParams(const ProcessParams & p){

    m_params=p;

    m_params.insert(tr("__Process"), m_name );
    m_params.insert(tr("__started"), QDateTime::currentDateTime().toString() );
}
