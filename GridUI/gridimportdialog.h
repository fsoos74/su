#ifndef GRIDIMPORTDIALOG_H
#define GRIDIMPORTDIALOG_H


#include <grid2d.h>

#include<memory>

#include <QDialog>

namespace Ui {
class GridImportDialog;
}



class GridImportDialog : public QDialog
{
    Q_OBJECT

public:

    const int PreviewLines=100;
    const qint64 MaxLineLength=1024;

    explicit GridImportDialog(QWidget *parent = 0);
    ~GridImportDialog();

    std::shared_ptr<Grid2D<float> > horizon(){
        return m_horizon;
    }

    int skipLines()const{
        return m_skipLines;
    }

    int inlineColumn()const{
        return m_inlineColumn;
    }

    int crosslineColumn()const{
        return m_crosslineColumn;
    }

    int valueColumn()const{
        return m_valueColumn;
    }

    bool validBounds()const{
        return m_validBounds;
    }

    int minInline()const{
        return m_minInline;
    }

    int maxInline()const{
        return m_maxInline;
    }

    int minCrossline()const{
        return m_minCrossline;
    }

    int maxCrossline()const{
        return m_maxCrossline;
    }

    double minValue()const{
        return m_minValue;
    }

    double maxValue()const{
        return m_maxValue;
    }

public slots:

    void setSkipLines( int );

    void setInlineColumn( int );

    void setCrosslineColumn( int );

    void setValueColumn( int );

    void setNullValue( const QString& );

    void setFilename( const QString& );

    void setInlineRange( int, int );

    void setCrosslineRange( int, int );

    void setValueRange( double, double );

    void setValidBounds( bool );

    void setHorizon( std::shared_ptr<Grid2D<float> > );

signals:

    void skipLinesChanged(int);

    void inlineColumnChanged( int );

    void crosslineColumnChanged( int );

    void valueColumnChanged( int );

    void nullValueChanged( const QString& );

    void filenameChanged( const QString& );

    void inlineRangeChanged( int, int);

    void crosslineRangeChanged( int, int);

    void valueRangeChanged( double, double);

    void validBoundsChanged( bool );

    void horizonChanged( std::shared_ptr<Grid2D<float> > );

private slots:

    void on_pbBrowse_clicked();
    void updatePreview( const QString&);
    void scan();
    void loadHorizon();
    void on_pbScan_clicked();

protected:
    void accept();

private:

    bool splitLine( const QString& line, int& iline, int& xline, double& value);
    void updateLastUsedColumn();
    void connectToUI();

    Ui::GridImportDialog *ui;

    int m_skipLines=0;
    int m_inlineColumn=0;
    int m_crosslineColumn=1;
    int m_valueColumn=2;
    int m_lastUsedColumn=2;
    QString m_nullValue="-999.99";
    QString m_filename;

    int m_minInline=1;
    int m_maxInline=-1;
    int m_minCrossline=1;
    int m_maxCrossline=-1;
    int m_minValue=1;
    int m_maxValue=-1;

    bool m_validBounds=false;

    std::shared_ptr<Grid2D<float> > m_horizon;
};

#endif // GridImportDialog_H
