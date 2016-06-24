#ifndef RUNGRIDSCRIPTPROCESS_H
#define RUNGRIDSCRIPTPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>

#include<QVector>
#include<QTemporaryFile>
#include<grid2d.h>

class RunGridScriptProcess : public ProjectProcess
{

    Q_OBJECT

public:

    RunGridScriptProcess( std::shared_ptr<AVOProject> project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_gridName;
    std::shared_ptr<Grid2D<double> > m_grid;

    QVector<QString> m_inputGridName;
    QVector< std::shared_ptr<Grid2D<double> > > m_inputGrid;

    QString m_script;

};


#endif // RUNGRIDSCRIPTPROCESS_H
