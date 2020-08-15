/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

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
#ifndef CLEVELSCROLL_H
#define CLEVELSCROLL_H

#include <QAbstractScrollArea>
class CGame;
class CLevelEntry;
class CSelection;
class CProto;
class CLevelEntry;

class CLevelScroll : public QAbstractScrollArea
{
    Q_OBJECT

public:
    CLevelScroll(QWidget *parent, CGame *game);
    void eventHandler();
    void editPath();
    void getGLInfo(QString & vendor,
                   QString & renderer,
                   QString & version,
                   QString & extensions);

protected:
    virtual void resizeEvent(QResizeEvent * event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    void initMouse();
    void select(int x1, int y1, int x2, int y2, CSelection & selection);
    void insideTile(int x1, int y1, int x2, int y2, CSelection & selection);
    bool isGameMode() {
        return m_gameMode;
    }
    void keyReflector(const QKeyEvent *event, bool pressed);

    typedef struct {
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

    Mouse m_mouse;
    CGame *m_game;
    int m_zoom;
    bool m_editPath;
    int m_ticks;
    bool m_gameMode;
    bool m_paintSprite;
    int m_proto;
    int m_gridSize;
    bool m_bErase;

    void scrollFastMargin(int & mx, int & my);
    void scrollFastByKeys(int & mx, int & my);
    void slowPrecisionPlacement(int & mx, int & my, CLevelEntry & entry);
    void alignEntry(CLevelEntry & entry);
    void changeOriginFromEntry(int &mx, int &my, const CLevelEntry &entry);
    int gridMask();
    void entryFromProto(const int protoId, CLevelEntry &entry);
    void paintSprite();
    void setCursor();

Q_SIGNALS:
    void pathEnded();
    void statusChanged(int, QString);
    void menuChanged();
    void setMX(int);
    void setMY(int);
    void viewSizeChanged(int len, int hei);
    void newLevelReq();
    void keyChanged(int, int);

protected slots:
    void setGameMode(bool gm);
    void getGameMode(bool & gm);
    void updateScrollbars(int w, int h);
    void enterEditPath();
    void leaveEditPath();
    void changeLevel(int i=0);
    void showScrollbars(bool);
    void mousePos(int &, int &, int &, int &);
    void mxChanged(int);
    void myChanged(int);
    void scrollStatus(int &, int &);
    void setPaintState(bool state);
    void changeProto(int proto);
    void setGridSize(int size);
    void setEraserState(bool state);
};

#endif // CLEVELSCROLL_H
