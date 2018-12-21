#ifndef LEGENDWIDGET_H
#define LEGENDWIDGET_H


#include<QLabel>
#include<QMap>

class LegendWidget : public QLabel
{
Q_OBJECT
public:
    LegendWidget(QWidget* parent=nullptr, Qt::WindowFlags flags=0);

public slots:
    void clear();
    void addItem(QString,QColor);
    void removeItem(QString);

private:
    void updateText();

    QMap<QString, QColor> m_items;
};

#endif // LEGENDWIDGET_H
