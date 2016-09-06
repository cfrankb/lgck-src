#ifndef CPREVIEWSCROLL_H
#define CPREVIEWSCROLL_H

#include <QAbstractScrollArea>
class CPreviewWidget;

class CPreviewScroll : public  QAbstractScrollArea
{
    Q_OBJECT

public:
    CPreviewScroll(QWidget *parent);

protected:
    typedef struct {
        bool noScroll;
        bool drag;
        int x;
        int y;
        int oldX;
        int oldY;
        bool lButton;
        bool rButton;
        bool mButton;
    } Mouse;
    Mouse m_mouse;
    virtual void resizeEvent(QResizeEvent * event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    CPreviewWidget * m_widget;

signals:
    void zoomChanged(int zoom);

protected slots:
    void updateScrollbars();
};

#endif // CPREVIEWSCROLL_H
