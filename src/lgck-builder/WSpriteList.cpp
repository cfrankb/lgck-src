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

#include "WSpriteList.h"
#include <QApplication>

CWSpriteList::CWSpriteList(QWidget *parent)
    : QTreeWidget(parent)
{
    m_drag = true;
} 

void CWSpriteList::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPos = event->pos();
    }
    QTreeWidget::mousePressEvent(event);
}

void CWSpriteList::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && m_drag) {
          int distance = (event->pos() - m_startPos).manhattanLength();
          if (distance >= QApplication::startDragDistance()) {

              QTreeWidgetItem *item =  itemAt(event->pos());
              if (item) {
                  QMimeData *mimeData = new QMimeData;

                  QDrag *drag = new QDrag(this);
                  drag->setMimeData(mimeData);

                  ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
                  int protoId = data->protoId;

                  QString text = QString("LGCK: %1").arg(protoId);
                  mimeData->setText(text);
                  drag->setPixmap(item->icon(0).pixmap(QSize(32,32)));
                  drag->setMimeData(mimeData);

                  // start drag
                  if (drag->exec(Qt::CopyAction) == Qt::CopyAction) {
                     // delete item;
                  }
              }

            event->accept();
        }
    }
}

void CWSpriteList::contextMenuEvent(QContextMenuEvent* event)
{
    Q_UNUSED(event);
}

void CWSpriteList::enableDrag(bool b)
{
    m_drag = b;
}
