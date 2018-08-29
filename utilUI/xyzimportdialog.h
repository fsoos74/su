#ifndef XYZIMPORTDIALOG_H
#define XYZIMPORTDIALOG_H


#include <QDialog>
#include <QVector3D>
#include <QVector>


namespace Ui {
class XYZImportDialog;
}


class XYZImportDialog : public QDialog
{
    Q_OBJECT

public:

    const int PreviewLines=100;
    const qint64 MaxLineLength=1024;

    explicit XYZImportDialog(QWidget *parent = 0);
    ~XYZImportDialog();

    QVector<QVector3D> data(){
        return m_data;
    }

    int skipLines();
    int xColumn();
    int yColumn();
    int zColumn();
    double x0();
    double y0();
    double z0();
    double sx();
    double sy();
    double sz();
    QString nullValue();
    QString filename();

    std::pair<double, double> xRange()const{
        return m_xRange;
    }
 
    std::pair<double, double> yRange()const{
        return m_yRange;
    }

    std::pair<double, double> zRange()const{
        return m_zRange;
    }


    QVector<QVector3D> loadData();

public slots:

    void setSkipLines( int );
    void setXColumn( int );
    void setYColumn( int );
    void setZColumn( int );
    void setX0(double);
    void setY0(double);
    void setZ0(double);
    void setSX(double);
    void setSY(double);
    void setSZ(double);
    void setNullValue( const QString& );
    void setFilename( const QString& );
    void setXRange( std::pair<double,double> );
    void setYRange( std::pair<double,double> );
    void setZRange( std::pair<double,double> );

private slots:

    void on_pbBrowse_clicked();
    void updatePreview( const QString&);
    void scan();
    void load();
    void on_pbScan_clicked();

protected:
    void accept();

private:

    bool processLine( const QString& line, double& x, double& y, double& z);
    void updateParamsFromControls();

    Ui::XYZImportDialog *ui;

    // keep variables to avoid getting values from controls for each line, e.g. xColumn()
    int m_skipLines=0;
    int m_xColumn=0;
    int m_yColumn=1;
    int m_zColumn=2;
    double m_x0=0;
    double m_y0=0;
    double m_z0=0;
    double m_sx=1;
    double m_sy=1;
    double m_sz=1;
    int m_lastUsedColumn=2;
    QString m_nullValue="-999.99";
    QString m_filename;

    std::pair<double, double> m_xRange;
    std::pair<double, double> m_yRange;
    std::pair<double, double> m_zRange;

    QVector<QVector3D> m_data;
};

#endif // XYZImportDialog_H
