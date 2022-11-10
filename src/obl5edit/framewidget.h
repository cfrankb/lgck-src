/*
    LGCK Builder package
    Copyright (C) 2005, 2016  Francois Blanchette

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

#ifndef CFRAMEWIDGET_H
#define CFRAMEWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_2_0>
#include <QTimer>
#include "DotArray.h"

class CFrame;

class CFrameWidget :
        public QOpenGLWidget,
        protected QOpenGLFunctions_2_0
{

    Q_OBJECT
public:
    CFrameWidget(QWidget *parent);
    virtual ~CFrameWidget();
    enum {
        TICK_RATE = 30
    };
    enum {
        MODE_NORMAL ,
        MODE_COLOR_ONLY,
        MODE_ALPHA_ONLY,
        MODE_TILED_VIEW
    };

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
    void getGLInfo(QString &vendor, QString &renderer, QString &version, QString &extensions);

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
    void drawBackground();
    void drawImage();
    void drawCheckers();
    void drawTapestry();

    QTimer m_timer;
    CFrame *m_frame;
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
    CFrame *m_shadow;
    int m_mx;
    int m_my;
    CDotArray *m_dots;
    CDotArray *m_dotsOrg;

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
    void penColorChanged(uint penColor);
    void zoomChanged(int zoom);
    void alphaChanged(uchar alpha);
    void scrollbarAdjusted();

private slots:
    void setMX(int mx);
    void setMY(int my);
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
    void handleTool(int x, int y, bool lButton, bool rButton);
    void clearDotArray();

    friend class CFrameScroll;
};

#endif // CFRAMEWIDGET_H
