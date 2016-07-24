/*
    LGCK Builder GUI
    Copyright (C) 1999, 2012  Francois Blanchette

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

#ifndef __CWSpriteList_H
#define __CWSpriteList_H
 
#include <QtGui>
#include <QTreeWidget>

typedef struct {
    int protoId;
} ITEM_DATA;

Q_DECLARE_METATYPE( ITEM_DATA* )

class CWSpriteList : public QTreeWidget
{
public:

    Q_OBJECT
 
public:
    CWSpriteList(QWidget *parent = 0);
    void enableDrag(bool b);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);    
    void contextMenuEvent(QContextMenuEvent* event);
    QPoint m_startPos;
    bool m_drag;
    
};
 
 
#endif 
