#ifndef LOGWRITER_H
#define LOGWRITER_H


#include<log.h>
#include<iostream>
#include<fstream>
#include<QString>


class LogWriter
{
public:

    LogWriter(const Log&);

    bool hasError()const{
        return m_error;
    }

    const QString& lastError()const{
        return m_errorString;
    }

    bool writeToStream(std::ofstream&);

private:

   bool writeMagic(std::ofstream&);
   bool writeHeader(std::ofstream&);
   bool writeData(std::ofstream&);
   void setError(const QString& msg);

   const  Log& m_log;
   bool m_error=false;
   QString m_errorString;
};

#endif // LOGWRITER_H
