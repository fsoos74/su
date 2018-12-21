#include "legendwidget.h"

#include<QMap>

LegendWidget::LegendWidget(QWidget* parent, Qt::WindowFlags flags):QLabel(parent,flags)
{

}

void LegendWidget::clear(){
    m_items.clear();
    updateText();
}

void LegendWidget::addItem(QString name, QColor color){
    m_items.insert(name,color);
    updateText();
}

void LegendWidget::removeItem(QString name){
    m_items.remove(name);
    updateText();
}

void LegendWidget::updateText(){

    auto keys=m_items.keys();
    std::sort(keys.begin(), keys.end());
    QString text;
    for(auto name : keys){
        auto color=m_items.value(name);
        auto colorstring=QString().sprintf("#%02x%02x%02x", color.red(), color.green(), color.blue());
        text+=QString("<span style=\"color:%1;\"> %2 </span><br>").arg(colorstring, name);
    }
    setText(text);
}
