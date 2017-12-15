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

    int skipLines()const{
        return m_skipLines;
    }

    int xColumn()const{
        return m_xColumn;
    }

    int yColumn()const{
        return m_yColumn;
    }

    int zColumn()const{
        return m_zColumn;
    }

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
    void setNullValue( const QString& );
    void setFilename( const QString& );
    void setXRange( std::pair<double,double> );
    void setYRange( std::pair<double,double> );
    void setZRange( std::pair<double,double> );

signals:

    void skipLinesChanged(int);
    void xColumnChanged( int );
    void yColumnChanged( int );
    void zColumnChanged( int );
    void xRangeChanged( std::pair<double,double>);
    void yRangeChanged( std::pair<double,double>);
    void zRangeChanged( std::pair<double,double>);
    void nullValueChanged( const QString& );
    void filenameChanged( const QString& );

private slots:

    void on_pbBrowse_clicked();
    void updatePreview( const QString&);
    void scan();
    void load();
    void on_pbScan_clicked();

protected:
    void accept();

private:

    bool splitLine( const QString& line, double& x, double& y, double& z);
    void updateLastUsedColumn();
    void connectToUI();

    Ui::XYZImportDialog *ui;

    int m_skipLines=0;
    int m_xColumn=0;
    int m_yColumn=1;
    int m_zColumn=2;
    int m_lastUsedColumn=2;
    QString m_nullValue="-999.99";
    QString m_filename;

    std::pair<double, double> m_xRange;
    std::pair<double, double> m_yRange;
    std::pair<double, double> m_zRange;

    QVector<QVector3D> m_data;
};

#endif // XYZImportDialog_H
