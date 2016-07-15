/*
    LGCK Builder package
    Copyright (C) 2005, 2013  Francois Blanchette

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

#include "Pixel.h"
#include <QBrush>
#include <QPainter>
#include <QPaintEngine>
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#endif
#include <QGraphicsView>

CPixel::CPixel(QGraphicsView * view, unsigned int *rgb, int x, int y)
{
    m_view = view;
    m_rgb = rgb;
    m_x = x;
    m_y = y;
    setZValue(1);
   // setFlags(ItemIsSelectable | ItemIsMovable);
    //setAcceptsHoverEvents(true);
}

QRectF CPixel::boundingRect() const
{
    return QRectF(0, 0, 8, 8);
}

QPainterPath CPixel::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, 8, 8);
    return path;
}

void CPixel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(painter);
    Q_UNUSED(option);
}
