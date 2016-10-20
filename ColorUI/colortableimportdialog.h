#ifndef COLORTABLEIMPORTDIALOG_H
#define COLORTABLEIMPORTDIALOG_H


#include <colortable.h>

#include<memory>

#include <QDialog>

namespace Ui {
class ColortableImportDialog;
}



class ColortableImportDialog : public QDialog
{
    Q_OBJECT

public:

    const int PreviewLines=300;
    const qint64 MaxLineLength=1024;

    explicit ColortableImportDialog(QWidget *parent = 0);
    ~ColortableImportDialog();

    QVector<ColorTable::color_type> colors()const{
        return m_colors;
    }

    int skipLines()const{
        return m_skipLines;
    }

    int redColumn();
    int greenColumn();
    int blueColumn();
    int alphaColumn();

    int minRed();
    int maxRed();
    int minGreen();
    int maxGreen();
    int minBlue();
    int maxBlue();
    int minAlpha();
    int maxAlpha();

public slots:

    void setSkipLines( int );

    void setRedColumn( int );
    void setGreenColumn( int );
    void setBlueColumn( int );
    void setAlphaColumn( int );

    void setFilename( const QString& );

    void setRedRange( int, int );
    void setGreenRange( int, int );
    void setBlueRange( int, int );
    void setAlphaRange( int, int );

    void setColors(  QVector<ColorTable::color_type>  );

signals:

    void filenameChanged( const QString& );

    void skipLinesChanged(int);

    void redColumnChanged( int );
    void greenColumnChanged( int );
    void blueColumnChanged( int );
    void alphaColumnChanged( int );

    void redRangeChanged( int, int );
    void greenRangeChanged( int, int );
    void blueRangeChanged( int, int );
    void alphaRangeChanged( int, int );

    void colorsChanged( QVector<ColorTable::color_type> );

private slots:

    void on_pbBrowse_clicked();
    void updatePreview( const QString&);
    void scan();
    void loadColors();
    void on_pbScan_clicked();

protected:
    void accept();

private:

    bool channelFromString(QString str, int& c);
    bool splitLine( const QString& line, int& red, int& green, int& blue, int& alpha);
    void updateLastUsedColumn();
    void connectToUI();

    Ui::ColortableImportDialog *ui;

    QString m_filename;
    int m_skipLines=0;
    int m_lastUsedColumn=3;

    QVector<ColorTable::color_type> m_colors;
};

#endif // ColortableImportDialog_H
