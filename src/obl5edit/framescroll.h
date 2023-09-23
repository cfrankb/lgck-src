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
    virtual void mouseDoubleClickEvent(QMouseEvent *);

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
