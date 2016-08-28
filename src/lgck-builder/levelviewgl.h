/*
    LGCK Builder GUI
    Copyright (C) 1999, 2016  Francois Blanchette

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

#ifndef LEVELVIEWGL_H_160814
#define LEVELVIEWGL_H_160814

#include <QtCore/qglobal.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_2_0>
#include <QTimer>

class CGame;

class CLevelViewGL :
        public QOpenGLWidget,
        protected QOpenGLFunctions_2_0
{
    Q_OBJECT

public:
    CLevelViewGL(QWidget* parent, CGame *game);
    virtual ~CLevelViewGL();
    bool isGameMode();

    enum {
        MAX_PIXEL = 0x7ff8,
        MARGIN = 32,
        FAST_SCROLL = 32,
        OBJ_SCROLL = 16,
        PATH_SCROLL = 8,
        MOUSE_POS_MASK = 0xfffffff8,
        TICK_SCALE   = 1000 / 35,
        TICK_MAX_RATE       = 200
    };

protected:

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

    bool m_showGrid;
    int m_gridSize;
    unsigned int m_gridColor;
    CGame *m_game;
    bool m_hasFocus;
    bool m_refresh;
    QTimer m_timer;

    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

    void drawGrid();
    void drawScreen();
    void drawSelection();
    void drawCheckers();
    void drawBackground();
    void drawItemRect();

Q_SIGNALS:
    void focused(bool);
    void whereIsMouse(int &, int &, int &, int &);
    void scrollStatusResync(int &, int &);
    void gameModeStateResync(bool &);

protected slots:
    void needRefresh();
    void showGrid(bool show);
    void setGridSize(int size);
    void setGridColor(const QString & gridColor);
    void sceneUpdated();
    friend class CLevelScroll;
};

#endif // LEVELVIEWGL_H_160814
