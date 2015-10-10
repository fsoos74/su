#ifndef NAVIGATIONWIDGET_H
#define NAVIGATIONWIDGET_H

#include <QWidget>
#include <QIntValidator>

namespace Ui {
class NavigationWidget;
}

class NavigationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NavigationWidget(QWidget *parent = 0);
    ~NavigationWidget();

    int min()const{
        return m_min;
    }

    int max()const{
        return m_max;
    }

    int current()const{
        return m_current;
    }

signals:
    void rangeChanged(int,int);
    void currentChanged(int);

public slots:
    void setRange( int, int);
    void setCurrent( int );

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);

private slots:
    void on_pbFirst_clicked();

    void on_pbLast_clicked();

    void on_pbPrevious_clicked();

    void on_pbNext_clicked();

    void on_leCurrent_returnPressed();

private:
    Ui::NavigationWidget *ui;

    QIntValidator* m_validator;

    int m_min=0;
    int m_max=0;
    int m_current=-1;
};

#endif // NAVIGATIONWIDGET_H
