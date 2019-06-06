#ifndef SMARTCOLORBARWIDGET_H
#define SMARTCOLORBARWIDGET_H

#include <QWidget>
#include <colortable.h>
#include <histogram.h>

class SmartColorBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SmartColorBarWidget(QWidget *parent = nullptr);
    ColorTable* colorTable()const{
        return mColorTable;
    }
    Histogram histogram()const{
        return mHistogram;
    }

signals:

public slots:
    void setColorTable(ColorTable*);
    void setHistogram(Histogram);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    ColorTable* mColorTable=nullptr;
    Histogram mHistogram;
};

#endif // SMARTCOLORBARWIDGET_H
