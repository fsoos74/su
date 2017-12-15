#include "tracklabel.h"

#include<QMouseEvent>

TrackLabel::TrackLabel(QString text, QWidget *parent):QLabel(text, parent)
{
    setMouseTracking(true);
}


void TrackLabel::mousePressEvent(QMouseEvent * event){

    if( event->button()==Qt::LeftButton){
        m_move=true;
        m_pos=event->pos();
        //setStyleSheet("QLabel { background-color : blue; }");
        setForegroundRole(QPalette::HighlightedText);
        setCursor(Qt::DragMoveCursor);
    }
}


void TrackLabel::mouseMoveEvent(QMouseEvent * event){

    if( m_move ){
        auto ppos=mapToParent(event->pos());
        auto wunder=parentWidget()->childAt(ppos);
        auto tlunder = dynamic_cast<TrackLabel*>(wunder);
        if( !tlunder ){
            setCursor(Qt::ForbiddenCursor);
            return;
        }
        if( tlunder == this ){
            setCursor(Qt::DragMoveCursor);
            return;
        }
        // could place here
        setCursor(Qt::DragCopyCursor);
    }
}


void TrackLabel::mouseReleaseEvent(QMouseEvent * event){

    if( event->button()==Qt::LeftButton && m_move ){
        m_move=false;
        emit moved( event->pos() );
        setForegroundRole(QPalette::Text);
        //setStyleSheet("QLabel { background-color : lightgrey; }");
        setCursor(Qt::ArrowCursor);
    }
}

void TrackLabel::enterEvent(QEvent *){
    if(!m_move){
        //setStyleSheet("QLabel { background-color : blue; }");
        setForegroundRole(QPalette::HighlightedText);
        setCursor(Qt::PointingHandCursor);
    }
}

void TrackLabel::leaveEvent(QEvent *){
    //setStyleSheet("QLabel { background-color : lightgrey; }");
    setForegroundRole(QPalette::Text);
    setCursor(Qt::ArrowCursor);
}

