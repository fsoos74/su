#include "processparametersdialog.h"

ProcessParametersDialog::ProcessParametersDialog(QWidget* parent):QDialog(parent)
{

}


void ProcessParametersDialog::setReservedDatasets(const QStringList& list){
    m_reservedDatasets=list;
}

void ProcessParametersDialog::setReservedGrids(const QStringList& list){
    m_reservedGrids=list;
}

void ProcessParametersDialog::setReservedVolumes(const QStringList& list){
    m_reservedVolumes=list;
}
