#ifndef CDPDATABASE_H
#define CDPDATABASE_H

#include <QtSql/QtSql>

#include <stdexcept>

class CDPDatabase
{
public:

    class Exception : public std::runtime_error{
    public:
        Exception( const QString& msg):std::runtime_error(msg.toStdString()){

        }
    };

    struct CDPData{

        CDPData( int il, int xl, double x, double y):iline(il), xline(xl), x(x), y(y){

        }

        int iline=-1;
        int xline=-1;
        double x=std::numeric_limits<double>::max();
        double y=std::numeric_limits<double>::max();
    };

    CDPDatabase(const QString& fileName);
    ~CDPDatabase();

    void put( int cdp, const CDPData& );
    CDPData get( int cdp );

private:

    void connectToDatabase(const QString& fileName);

    QSqlDatabase m_db;
};

#endif // CDPDATABASE_H
