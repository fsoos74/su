#ifndef PROCESSPARAMETERSDIALOG_H
#define PROCESSPARAMETERSDIALOG_H

#include <QObject>
#include <QDialog>
#include <QStringList>


class ProcessParametersDialog : public QDialog
{
public:

    explicit ProcessParametersDialog(QWidget* parent=nullptr);

    virtual QMap<QString,QString> params()=0;

    const QStringList& reservedDatasets()const{
        return m_reservedDatasets;
    }

    const QStringList& reservedGrids()const{
        return m_reservedGrids;
    }

    const QStringList& reservedVolumes()const{
        return m_reservedVolumes;
    }

    void setReservedDatasets(const QStringList&);
    void setReservedGrids(const QStringList&);
    void setReservedVolumes(const QStringList&);

private:

    QStringList m_reservedDatasets;
    QStringList m_reservedGrids;
    QStringList m_reservedVolumes;
};

#endif // PROCESSPARAMETERSDIALOG_H
