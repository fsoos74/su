#ifndef TABLEWRITER_H
#define TABLEWRITER_H


#include<table.h>
#include<iostream>
#include<QFile>
#include<QString>

class TableWriter
{
public:

    TableWriter(const Table&);

    bool hasError()const{
        return m_error;
    }

    const QString& lastError()const{
        return m_errorString;
    }

    bool writeToFile(const QString&);

private:

   bool writeMagic(QFile&);
   bool writeKeysAndSize(QFile&);
   bool writeData(QFile&);
   void setError(const QString& msg);

   const  Table& m_table;
   bool m_error=false;
   QString m_errorString;
};

#endif // TABLEWRITER_H
