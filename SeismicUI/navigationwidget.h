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

    size_t min()const{
        return m_min;
    }

    size_t max()const{
        return m_max;
    }

    size_t current()const{
        return m_current;
    }

signals:
    void rangeChanged(size_t, size_t);
    void currentChanged(size_t);

public slots:
    void setRange( size_t, size_t);
    void setCurrent( size_t );

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

    size_t m_min=0;
    size_t m_max=0;
    size_t m_current=0;
};

#endif // NAVIGATIONWIDGET_H
