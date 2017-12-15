#ifndef VOLUMEITEMSCONFIGDIALOG_H
#define VOLUMEITEMSCONFIGDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVector>
#include <colortable.h>
#include <histogram.h>


class VolumeItemsConfigDialog : public QDialog
{
    Q_OBJECT

public:
        VolumeItemsConfigDialog( QWidget* parent=nullptr);

public slots:

        void addVolumeItem( QString name, qreal alpha, ColorTable* ct, const Histogram&);

signals:

        void alphaChanged(QString,double);

private slots:

        void onSBAlphaValueChanged(double);
        void onPBColorsClicked();
        void onPBRangeClicked();

private:

    QVector<QLabel*> m_labels;
    QVector<QDoubleSpinBox*> m_sbAlpha;
    QVector<ColorTable*> m_colorTable;
    QVector<Histogram> m_histograms;
    QVector<QPushButton*> m_pbColors;
    QVector<QPushButton*> m_pbRange;
};

#endif // VolumeItemsConfigDialog_H
