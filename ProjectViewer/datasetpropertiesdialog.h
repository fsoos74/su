#ifndef DATASETPROPERTIESDIALOG_H
#define DATASETPROPERTIESDIALOG_H

#include <QDialog>
#include "seismicdatasetinfo.h"

namespace Ui {
class DatasetPropertiesDialog;
}

class DatasetPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatasetPropertiesDialog(QWidget *parent = 0);
    ~DatasetPropertiesDialog();

    QString name();
    int dimensions();
    SeismicDatasetInfo::Domain domain();
    SeismicDatasetInfo::Mode mode();

public slots:

    void setName(const QString&);
    void setDimensions(int);
    void setDomain(SeismicDatasetInfo::Domain);
    void setMode(SeismicDatasetInfo::Mode);


private:
    Ui::DatasetPropertiesDialog *ui;
};

#endif // DATASETPROPERTIESDIALOG_H
