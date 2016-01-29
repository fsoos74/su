#ifndef AXXISLABELWIDGET_H
#define AXXISLABELWIDGET_H

#include<QWidget>
#include<QStringList>

class GatherView;

class AxxisLabelWidget : public QWidget
{
    Q_OBJECT

public:

    AxxisLabelWidget( GatherView* parent=nullptr);

    const QStringList& labels()const{
        return m_labels;
    }

public slots:
    void setLabels(const QStringList&);

signals:
    void labelsChanged( const QStringList&);

protected:
    void paintEvent(QPaintEvent *);

private:
    GatherView* m_view;
    QStringList m_labels;
};

#endif // AXXISLABELWIDGET_H
