/*
    LGCK Builder GUI
    Copyright (C) 2011, 2013  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WPREVIEW_H
#define WPREVIEW_H

#include <QGraphicsView>
class CFrame;
class QMouseEvent;
class QTimer;

class CWPreview : public QGraphicsView
{
    Q_OBJECT
public:
    explicit CWPreview(QWidget *parent = 0);
    ~CWPreview();
    void assignStatic(CFrame *frame);    
    void animate(int speed);
    void setSpeed(int);
    int zoom();
    int speed();

protected:
    void mousePressEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
    void drawBackground(QPainter *painter, const QRectF & rect);
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
    QGraphicsScene *m_scene;
    CFrame *m_frame;
    QTimer *m_timer;
    unsigned int m_bkColor;
    int m_zoom;
    int m_state;
    int m_frameID;
    int m_speed;
    enum {
        STATE_LIVEVIEW,
        STATE_STATIC,
        STATE_ANIMATED
    };
    void updateScrollbars();

signals:
    void zoomChanged(int zoom);
    void nextFrame(int currFrame);

public slots:
    void setZoom(int zoom);

private slots:
    void changeFrame(CFrame * frame);
    void imageChanged();
    void setBkColor(unsigned int bkColor);
    void update();
    void setNextFrame(int currFrame, CFrame *);
};

#endif // WPREVIEW_H
