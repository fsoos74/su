#ifndef VOLUMESPROCESS_H
#define VOLUMESPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <QVector>
#include <volume.h>


class VolumesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    VolumesProcess( QString name, AVOProject* project, QObject* parent=nullptr);
    virtual ~VolumesProcess(){}
    ResultCode init(const QMap<QString, QString>& parameters)=0;
    ResultCode run();

protected:
    int ninputs()const{ return m_inputVolumeNames.size();}
    int noutputs()const{ return m_outputVolumeNames.size();}
    Grid3DBounds inputBounds(int i)const;
    Domain inputDomain(int i)const;
    VolumeType inputType(int i)const;
    const Grid3DBounds& bounds()const{return m_bounds;}
    void setBounds(Grid3DBounds bounds){ m_bounds=bounds;}
    ResultCode addInputVolume(QString);
    ResultCode addOutputVolume(QString, Grid3DBounds, Domain, VolumeType);


    virtual ResultCode processInline( QVector<std::shared_ptr<Volume>> outputs,
                QVector<std::shared_ptr<Volume>> inputs, int iline)=0;
    virtual ResultCode finalize(){return ResultCode::Ok;}

private:

#ifdef IO_VOLUMES
    const int CHUNK_SIZE=10;
    QVector<VolumeReader2*> m_inputReaders;
    QVector<VolumeWriter2*> m_outputWriters;
#else
    QVector<std::shared_ptr<Volume> > m_inputVolumes;
    QVector<std::shared_ptr<Volume> > m_outputVolumes;
#endif

    QStringList m_inputVolumeNames;
    QStringList m_outputVolumeNames;

    Grid3DBounds m_bounds;

};

#endif // VolumesProcess_H
