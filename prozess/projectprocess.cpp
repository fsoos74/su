#include "projectprocess.h"

#include<iostream>

ProjectProcess::ProjectProcess( const QString& name, std::shared_ptr<AVOProject> project, QObject *parent)
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
