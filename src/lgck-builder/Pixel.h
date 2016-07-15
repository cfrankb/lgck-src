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

#ifndef PIXEL_H
#define PIXEL_H

#include <QGraphicsItem>
class QGraphicsView;

class CPixel : public QGraphicsItem
{
public:
    CPixel(QGraphicsView *view, unsigned int *rgb, int x, int y);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

private:
    QGraphicsView *m_view;
    unsigned int *m_rgb;
    int m_x;
    int m_y;
};

#endif // PIXEL_H
