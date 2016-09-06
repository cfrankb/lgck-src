#ifndef CFRAMESCROLL_H
#define CFRAMESCROLL_H

#include <QAbstractScrollArea>
class CFrame;
class CFrameWidget;
class CDotArray;

class CFrameScroll : public  QAbstractScrollArea
{
    Q_OBJECT
public:

    CFrameScroll(QWidget *parent);

protected:
    virtual void resizeEvent(QResizeEvent * event);
    virtual void paintEvent(QPaintEvent *event);

    virtual void wheelEvent(QWheelEvent *);
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void mouseDoubleClickEvent(QMouseEvent * event);

    void initMouse();
//    void handleTool();

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
    CFrameWidget *m_widget;

signals:
    void modified();
    void mouseUp();
    void statusUpdate(int, const QString);
    void toolHandler(int, int, bool, bool);
    void cursorChanged();

protected slots:
    void updateScrollbars();
};

#endif // CFRAMESCROLL_H
