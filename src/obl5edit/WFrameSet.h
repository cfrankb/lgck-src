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

#ifndef __CWFrameSet_H
#define __CWFrameSet_H

//#include <QtGui>
#include <QTreeWidget>

class CFrameSet;

typedef struct {
    int id;
} ITEM_DATA_FS;

Q_DECLARE_METATYPE( ITEM_DATA_FS* )

class CWFrameSet : public QTreeWidget
{
public:

    Q_OBJECT

public:
    CWFrameSet(QWidget *parent = 0);
    void setFrameSet(CFrameSet *frameSet);
    CFrameSet * getFrameSet();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent* event);

    QPoint m_startPos;
    CFrameSet *m_frameSet;

    void dropEvent(QDropEvent *de);
    void dragMoveEvent(QDragMoveEvent *de);
    void dragEnterEvent(QDragEnterEvent *event);

protected slots:
    void addImages();
    void deleteImage();
    void setTransparency();
    void removeAll();
    void enlargeImage();
    void reload();
    void sliceImage();

signals:
    void countChanged();
    void clicked();

protected:
     void updateIcon(QTreeWidgetItem *item,  int j);
     void addFrame(int index);
     void addFile(QString name);
};

#endif
