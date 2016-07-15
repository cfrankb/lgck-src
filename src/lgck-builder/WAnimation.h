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

#ifndef WANIMATION_H
#define WANIMATION_H

#include <QtGui>
#include <QTreeWidget>

class CWAnimation : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CWAnimation(QWidget *parent = 0);
    void connectSlots(QWidget *parent);

signals:
    void animationClear(int id);
    void animationEdit(int id);

public slots:
    void editAnimation();
    void clearAnimation();

protected:
    void contextMenuEvent(QContextMenuEvent* event);
};

#endif // WANIMATION_H
