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

#include "WAnimation.h"
#include <QMenu>
#include <QAction>

CWAnimation::CWAnimation(QWidget *parent) :
    QTreeWidget(parent)
{
}

void CWAnimation::connectSlots(QWidget *parent)
{
    connect(this, SIGNAL(animationEdit(int)),
            parent, SLOT(editAnimation(int)));
    connect(this, SIGNAL(animationClear(int)),
            parent, SLOT(clearAnimation(int)));
}

void CWAnimation::contextMenuEvent(QContextMenuEvent* event)
{
    const QPoint pos = event->pos();
    QTreeWidgetItem * item = itemAt(pos);
    if (item) {
        // create pop-up
        QMenu menu(this);

        // Make sure this item is selected
        setCurrentItem( item );

        QAction *actionEdit = new QAction(tr("Edit Animation"), &menu);
        menu.addAction(actionEdit);

        QAction *actionClearAll = new QAction(tr("Clear Animation"), &menu);
        menu.addAction(actionClearAll);

        connect(actionEdit, SIGNAL(triggered()),
                this, SLOT(editAnimation()));

        connect(actionClearAll, SIGNAL(triggered()),
                this, SLOT(clearAnimation()));

        menu.exec(mapToGlobal(pos));
    }
}

void CWAnimation::editAnimation()
{
    int index = currentIndex().row();
    emit animationEdit(index);
}

void CWAnimation::clearAnimation()
{
    int index = currentIndex().row();
    emit animationClear(index);
}
