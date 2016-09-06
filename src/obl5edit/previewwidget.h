#ifndef CPREVIEWWIDGET_H
#define CPREVIEWWIDGET_H

#include "stdafx.h"
#include "Frame.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions_2_0>
#include <QTimer>
class QMouseEvent;
class QPainter;
class QRectF;
class CFrame;

class CPreviewWidget :
        public QOpenGLWidget,
        protected QOpenGLFunctions_2_0
{
    Q_OBJECT

public:
    CPreviewWidget(QWidget *parent);
    virtual ~CPreviewWidget();

    void assignStatic(CFrame *frame);
    void animate(int speed);
    void setSpeed(int);
    int zoom();
    int speed();

    enum {
        STATE_LIVEVIEW,
        STATE_STATIC,
        STATE_ANIMATED
    };

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
    void mousePressEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
    void drawBackground();
    void drawImage();
    void drawChecker();
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
    CFrame *m_frame;
    QTimer *m_timer;
    unsigned int m_bkColor;
    int m_zoom;
    int m_state;
    int m_frameID;
    int m_speed;
    bool m_ready;

signals:
    void zoomChanged(int zoom);
    void nextFrame(int currFrame);
    void scrollbarAdjusted();

public slots:
    void setZoom(int zoom);

private slots:
    void changeFrame(CFrame * frame);
    void imageChanged();
    void setBkColor(unsigned int bkColor);
    void updateFrame();
    void setNextFrame(int currFrame, CFrame *);
    void setMX(int);
    void setMY(int);

protected:
    int m_mx;
    int m_my;
    friend class CPreviewScroll;
};

#endif // CPREVIEWWIDGET_H
