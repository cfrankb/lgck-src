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
    setAcceptHoverEvents(true);
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

/*    if (painter->paintEngine()->type() != QPaintEngine::OpenGL
            && painter->paintEngine()->type() != QPaintEngine::OpenGL2){
        qWarning("OpenGLScene: drawBackground needs a "
               "QGLWidget to be set as viewport on the "
               "graphics view");
      return;
    }


    if (m_rgb) {

       // qDebug("visible: %d", isVisible());

        QRectF rcFrame = m_view->frameRect();

        glMatrixMode (GL_MODELVIEW);
        glTranslatef(0, 0, 0);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity ();
        glOrtho(0,rcFrame.right(),0,rcFrame.bottom(),-1,1);
        glViewport(
                0,          // lower left x position
                0,			// lower left y position
                rcFrame.right(),	// viewport width
                rcFrame.bottom()	// viewport height
            );

        glMatrixMode (GL_MODELVIEW);
        glTranslatef(0, 0, 0);
        glLoadIdentity();

        QRectF r = mapRectToScene(boundingRect());
        //qDebug("%f %f %f %f", r.left(), r.top(), r.right(), r.bottom());

        int x1 = r.left();
        int x2 = r.right();
        int y1 = rcFrame.bottom() - r.top();
        int y2 = rcFrame.bottom() - r.bottom();
        unsigned int rgb = *m_rgb;
        float red = (rgb & 0xff) / 255.0f;
        float green = ((rgb >> 8) & 0xff ) / 255.0f;
        float blue = ((rgb >> 16) & 0xff) / 255.0f;

        glBegin(GL_LINE_LOOP);
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
            glVertex2f(x1, y1);
            glVertex2f(x2, y1);
            glVertex2f(x2, y2);
            glVertex2f(x1, y2);
        glEnd();

      //  y2 += 1;
        x2 -= 1;

        glBegin(GL_QUADS);
            glColor4f(red, green, blue, 1.0f);
            glVertex2f(x1, y1);  //Draw the four corners of the rectangle
            glVertex2f(x2, y1);
            glVertex2f(x2, y2);
            glVertex2f(x1, y2);
        glEnd();
    }
        */

/*
    if (m_rgb) {
        unsigned int rgb = *m_rgb;
        QBrush b = painter->brush();
        painter->setBrush(QColor(0,0,0,0xff));
        painter->drawRect(0, 0, 8, 8);
        if (rgb & 0xff000000) {
            QColor color(rgb & 0xff, (rgb >> 8) & 0xff, (rgb >> 16) & 0xff, (rgb >> 24) & 0xff);
            painter->fillRect(QRectF(0, 0, 7, 7), color);
        }
        painter->setBrush(b);
    }
    //color.darker()
*/
}
