#ifndef DYNAMICMOUSEMODESELECTOR_H
#define DYNAMICMOUSEMODESELECTOR_H

#include<mousemode.h>
#include<QVector>
#include<QMap>
#include<QToolButton>
#include<QButtonGroup>
#include<QHBoxLayout>

class DynamicMouseModeSelector : public QWidget
{
    Q_OBJECT
public:
    explicit DynamicMouseModeSelector(QWidget *parent = 0);

    QList<MouseMode> modes()const{
        return buttons.keys();
    }

    MouseMode mode()const;

    QToolButton* button(MouseMode m)const;

signals:
    void modeChanged(MouseMode);

public slots:

    void addMode( MouseMode );
    void removeMode( MouseMode );

    void setMode(MouseMode);

private slots:
    void onButtonBox();

private:

    QHBoxLayout* layout;
    QButtonGroup* buttonGroup;
    QMap<MouseMode, QToolButton*> buttons;
};

#endif // DYNAMICMOUSEMODESELECTOR_H
