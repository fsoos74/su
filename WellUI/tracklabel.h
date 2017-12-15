#ifndef TRACKLABEL_H
#define TRACKLABEL_H

#include<QLabel>

class TrackLabel : public QLabel
{
    Q_OBJECT
public:
    TrackLabel(QString text, QWidget*parent=nullptr);

signals:
    void moved( QPoint delta);

protected:

    void mousePressEvent(QMouseEvent*)override;
    void mouseMoveEvent(QMouseEvent*)override;
    void mouseReleaseEvent(QMouseEvent*)override;
    void enterEvent(QEvent*)override;
    void leaveEvent(QEvent*)override;

private:
    bool m_move=false;
    QPoint m_pos;
};

#endif // TRACKLABEL_H
