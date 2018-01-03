#ifndef RULERAXISVIEW_H
#define RULERAXISVIEW_H

#include "axisview.h"
#include "hscaleview.h"
#include "vscaleview.h"

#include<QScrollBar>
#include<QResizeEvent>

class RulerAxisView : public AxisView{
public:
    RulerAxisView(QWidget* parent=nullptr);
    void setXAxis(Axis*)override;
    void setZAxis(Axis *)override;

    HScaleView* hscaleView()const{
        return m_hscaleView;
    }

    VScaleView* vscaleView()const{
        return m_vscaleView;
    }

public slots:
    void setHScaleAlignment(Qt::Alignment);
    void setVScaleAlignment(Qt::Alignment);

protected:
    virtual void refreshScene()=0;
    virtual void resizeEvent(QResizeEvent *)override;

private:
    void updateLayout();

    HScaleView* m_hscaleView;
    VScaleView* m_vscaleView;
};


#endif  // RULERAXISVIEW_H
