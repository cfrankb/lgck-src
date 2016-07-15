/*
    LGCK Builder GUI
    Copyright (C) 1999, 2011  Francois Blanchette

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

#ifndef _CLEVELVIEW_H
#define _CLEVELVIEW_H

#include <QGraphicsView>
class QGraphicsScene;
class QGLWidget;
class QWidget;
class QFocusEvent;
class CGame;
class CLevel;
class QSize;
class CLevel;

class CLevelView : public QGraphicsView
{
    Q_OBJECT

public :

    CLevelView(QWidget* Parent);
    ~CLevelView();
    void setGameDB(CGame * p);
    void eventHandler();
    void editPath();
    const QSize getSize();

    enum {
        MOUSE_POS_MASK = 0xfffffff8
    };

    enum {
        MAX_PIXEL = 0x7ff8,
        MARGIN = 32,
        FAST_SCROLL = 32,
        OBJ_SCROLL = 16,
        PATH_SCROLL = 8
    };

protected :

    virtual void resizeEvent (QResizeEvent *event);
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );
    virtual void mouseDoubleClickEvent ( QMouseEvent * event );
    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void keyReleaseEvent ( QKeyEvent * event );
    virtual void dropEvent(QDropEvent *de);
    virtual void dragMoveEvent(QDragMoveEvent *de);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent * event );
    virtual void focusOutEvent(QFocusEvent * event );
    virtual void focusInEvent(QFocusEvent *);
    virtual void drawBackground(QPainter *painter, const QRectF & rect);
    void drawItemRect();
    void drawScreen();
    void drawGrid();
    void drawChecker();
    void drawSelection();
    void initMouse();
    bool isGameMode() const;
    void keyReflector(const QKeyEvent *event, bool pressed);
    void select(int x1, int y1, int x2, int y2);
    void updateScrollbars();

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
        int orgX;
        int orgY;
        int destX;
        int destY;
    } Mouse;

    QGraphicsScene *m_scene;
    QGLWidget *m_glWidget;
    Mouse m_mouse;
    bool m_showGrid;
    int m_gridSize;
    unsigned int m_gridColor;
    int m_ticks;
    CGame *m_game;
    bool m_gameMode;
    bool m_editPath;
    bool m_hasFocus;
    int m_zoom;

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
    void keyChanged(int, int);
    void menuChanged();
    void statusChanged(int, QString);
    void newLevelReq();
    void viewSizeChanged(int len, int hei);    
    void pathEnded();
    void focused(bool);
    void setMX(int);
    void setMY(int);

protected slots:
    void setGameMode(bool gm);
    void showGrid(bool show);
    void setGridSize(int size);
    void setGridColor(const QString & gridColor);
    void enterEditPath();
    void leaveEditPath();
    void changeLevel(int i=0);
    void mxChanged(int);
    void myChanged(int);
    void sceneUpdated();
    void showScrollbars(bool);
    void makeCurrent();
};

#endif
