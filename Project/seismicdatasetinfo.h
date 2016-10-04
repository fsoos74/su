#ifndef SEISMICDATASETINFO_H
#define SEISMICDATASETINFO_H

#include<QString>

class SeismicDatasetInfo
{
public:

    enum class Domain{Time,Depth};
    enum class Mode{Prestack, Poststack};


    SeismicDatasetInfo();

    const QString& name()const{
        return m_name;
    }

    const QString& path()const{
        return m_path;
    }

    const QString& infoPath()const{
        return m_infoPath;
    }

    const QString& indexPath()const{
        return m_indexPath;
    }

    Domain domain()const{
        return m_domain;
    }

    Mode mode()const{
        return m_mode;
    }

    void setName(const QString&);
    void setPath(const QString&);
    void setInfoPath(const QString&);
    void setIndexPath(const QString&);
    void setDomain(Domain);
    void setMode(Mode);

    void makeRelative(const QString& dir);
    void makeAbsolute(const QString& dir);

private:

    QString m_name;
    bool    m_internal=false;
    QString m_path;
    QString m_infoPath;
    QString m_indexPath;
    Domain  m_domain=Domain::Time;
    Mode    m_mode=Mode::Prestack;
};

QString datasetDomainToString( SeismicDatasetInfo::Domain );
SeismicDatasetInfo::Domain stringToDatasetDomain(const QString&);

QString datasetModeToString( SeismicDatasetInfo::Mode);
SeismicDatasetInfo::Mode stringToDatasetMode(const QString&);

#endif // SEISMICDATASETINFO_H
