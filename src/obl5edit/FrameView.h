/*
    LGCK Builder package
    Copyright (C) 2005, 2013  Francois Blanchette

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

#ifndef FRAMEVIEW_H
#define FRAMEVIEW_H

#include <QGraphicsView>
class QGraphicsScene;
class CFrame;
class CDotArray;

class CFrameView : public QGraphicsView
{
    Q_OBJECT
public:
    CFrameView(QWidget *parent = 0);
    ~CFrameView();
    int zoom();
    bool showGrid();
    bool showTransparency();
    int tool();
    int penSize();
    uint borderColor();
    uint gridColor();
    uint bkColor();
    uint penColor();
    uchar alpha();
    int viewMode();

    enum {
        MODE_NORMAL ,
        MODE_COLOR_ONLY,
        MODE_ALPHA_ONLY,
        MODE_TILED_VIEW
    };

protected:
    void resizeEvent(QResizeEvent * event);
    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseDoubleClickEvent(QMouseEvent * event);
    void drawBackground(QPainter *painter, const QRectF & rect);
    void debug(const char *s);

private:
    QGraphicsScene *m_scene;
    CFrame *m_frame;
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
    bool m_showGrid;
    bool m_showTransparency;
    uint m_borderColor;
    uint m_gridColor;
    uint m_bkColor;
    uint m_penColor;
    int m_penSize;
    int m_tool;
    int m_zoom;
    int m_mode;
    uchar m_alpha;
    CDotArray *m_dots;
    CDotArray *m_dotsOrg;
    CFrame *m_shadow;
    void initMouse();
    void handleTool();
    void drawChecker();
    void drawTapestry();
    void drawImage();
    void updateScrollbars();
    enum {
        mask_full_alpha = 0xff000000
    };
    inline int pow2roundup (int x)
    {
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x+1;
    }

signals:
    void sceneCleared();
    void statusUpdate(int, const QString);
    void penColorChanged(uint penColor);
    void zoomChanged(int zoom);
    void modified();
    void mouseUp();
    void alphaChanged(uchar alpha);

private slots:
    void setViewMode(int mode);
    void changeZoom(int px);
    void changeCursor();
    void setBkColor(uint bkColor);
    void setBorderColor(uint bkColor);
    void setGridColor(uint bkColor);
    void setPenColor(uint penColor);
    void setPenSize(int penSize);
    void setTool(int tool);
    void changeImage();
    void enableGrid(bool enable);
    void setShadow(CFrame *frame);
    void changeFrame(CFrame *);
    void setTransparency(bool);
    void setAlpha(uchar alpha);
};

#endif // FRAMEVIEW_H
