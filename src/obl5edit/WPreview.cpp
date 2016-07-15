/*
    LGCK Builder GUI
    Copyright (C) 2011, 2013  Francois Blanchette

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

#include "WPreview.h"
#include <QtOpenGL>
#include "../shared/stdafx.h"
#include "../shared/Frame.h"
#include "Pixel.h"
#include <QPixmap>
#include <QWidget>

CWPreview::CWPreview(QWidget *parent) :
    QGraphicsView(parent)
{
    m_bkColor = 0x00ffff;
    m_frame = NULL;
    m_scene = new QGraphicsScene(this);
    m_state = STATE_LIVEVIEW;
    m_zoom = 1;
    m_frameID = -1;
    m_timer = NULL;
    m_speed = 0;

    setScene(m_scene);

    //QGL::setPreferredPaintEngine(QPaintEngine::OpenGL2);
    QGLFormat glFmt;
    glFmt.setSwapInterval(1); // 1= vsync on
    glFmt.setAlpha(true);
    glFmt.setRgba(true);
    glFmt.setDoubleBuffer(true); // default
    glFmt.setOverlay(false);
    glFmt.setSampleBuffers(false);
    QGLFormat::setDefaultFormat(glFmt);

    setAttribute(Qt::WA_OpaquePaintEvent,true);
    setAttribute(Qt::WA_PaintOnScreen,true);

    m_mouse.x = -1;
    m_mouse.y = -1;
    m_mouse.oldX=-1;
    m_mouse.oldY=-1;
    m_mouse.lButton = false;
    m_mouse.rButton = false;

    QPixmap pixmap;
    pixmap.load(":/images/cursors/tinyglass.png");
    pixmap.setMask( pixmap.createHeuristicMask() );
    setCursor(QCursor(pixmap, 6, 5));

    setViewport(new QGLWidget(QGLFormat(QGL::NoSampleBuffers)));
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //setRenderHint(QPainter::Antialiasing, false);
    setDragMode(QGraphicsView::NoDrag);
    //setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // If cache is set, the OpenGL won't work
    //m_view->setCacheMode(QGraphicsView::CacheBackground);
    setContentsMargins(QMargins(0, 0, 0, 0));
}

CWPreview::~CWPreview()
{
    if (m_state == STATE_STATIC && m_frame) {
        delete m_frame;
    }
}

void CWPreview::changeFrame(CFrame *frame)
{
    m_frame = frame;
    m_scene->clear();
    if (frame) {
        m_scene->setSceneRect(0, 0, m_zoom * frame->m_nLen, m_zoom * frame->m_nHei);
        // add dummy item to allow screen to be refreshed
        QGraphicsItem *item = new CPixel(NULL, NULL,0,0);
        item->setPos(QPoint(0,0));
        m_scene->addItem(item);
    } else {
        m_scene->setSceneRect(0, 0, 1, 1);
    }
    updateScrollbars();
}

void CWPreview::drawBackground(QPainter *painter, const QRectF & rect)
{
    Q_UNUSED(rect);
    Q_UNUSED(painter);

/*    if (painter->paintEngine()->type() != QPaintEngine::OpenGL
            && painter->paintEngine()->type() != QPaintEngine::OpenGL2){
        qDebug("engine: %d", painter->paintEngine()->type());
        qWarning("OpenGLScene: drawBackground needs a "
               "QGLWidget to be set as viewport on the "
               "graphics view");
        return;
    }*/

    QRectF r = frameRect();
    //qDebug("%f %f %f %f", r.top(), r.left(), r.right(), r.bottom());
    if (m_frame) {
        // border color
        uint color = m_bkColor;
        float red = (color & 0xff) / 255.0f;
        float green = ( (color >> 8) & 0xff) / 255.0f;
        float blue = ( (color >> 16) & 0xff) / 255.0f;
        glClearColor(red, green, blue, 1.0f);
    } else {
        glClearColor(0x50/255.0f, 0x90/255.0f, 1.0f, 1.0f);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,r.right(),0,r.bottom(),-1,1);
    glViewport(
            0,          // lower left x position
            0,			// lower left y position
            r.right(),	// viewport width
            r.bottom()	// viewport height
    );

    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glShadeModel( GL_FLAT );
    glDisable(GL_LIGHTING);
    glPushMatrix();
        glTranslatef(0,0,0);
        if (m_frame) {
            drawImage();
        } else {
            drawChecker();
        }
    glPopMatrix();
}

void CWPreview::drawChecker()
{
    QRectF r = frameRect();
    int gridSize = 32;
    for (int y=0; y * gridSize < r.bottom(); ++y) {
        for (int x=0; x * gridSize < r.right(); ++x) {
            if ((x + y) % 2 == 0) {
                int x1 = x * gridSize;
                int x2 = (x + 1) * gridSize;
                int y1 = r.bottom() - y * gridSize;
                int y2 = r.bottom() - (y + 1) * gridSize;
                glBegin(GL_QUADS);
                    glColor4f(0.25f, 0.5f, 1.0f, 1.0f);
                    glVertex2f(x1, y1);  //Draw the four corners of the rectangle
                    glVertex2f(x2, y1);
                    glVertex2f(x2, y2);
                    glVertex2f(x1, y2);
                glEnd();
            }
        }
    }
}

void CWPreview::drawImage()
{
    QRect r = frameRect();
    int mx = horizontalScrollBar()->value();
    int my = verticalScrollBar()->value();

    int cx = std::min(m_frame->m_nLen - mx, r.right()/m_zoom) ;
    int cy = std::min(m_frame->m_nHei - my, r.bottom()/m_zoom);
    CFrame *tmp = m_frame->clip(mx, my, cx, cy);
    tmp->flipV();

    int x1 = 0;
    //int x2 = cx * m_zoom;
    int y1 = r.bottom() - cy * m_zoom;
    //int y2 = r.bottom();

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glRasterPos2d(x1, y1);
    glPixelZoom(m_zoom, m_zoom);
    glDrawPixels(cx, cy,
             GL_RGBA, GL_UNSIGNED_BYTE,
             tmp->getRGB());

/*
    // needed or the colors become funky
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    //glLoadIdentity();
    GLuint texName;
    // Enable Alpha blending
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Enable textures
    glEnable(GL_TEXTURE_2D);
    // set pixel alignment (DWORD)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    // genTextureName
    glGenTextures(1, &texName);
    // Bind texture to name
    glBindTexture(GL_TEXTURE_2D, texName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cx, cy, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp->getRGB() );

    float xx = 1.0f;
    float yy = 1.0f;

    glBegin(GL_QUADS);


        glTexCoord2f(0.0, 0.0); glVertex3f(x1, y2, 0.0);
        glTexCoord2f(0.0, yy); glVertex3f(x1, y1, 0.0);
        glTexCoord2f(xx, yy); glVertex3f(x2, y1, 0.0);
        glTexCoord2f(xx, 0.0); glVertex3f(x2, y2, 0.0);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable (GL_BLEND);
    glDeleteTextures(1, &texName);
    */
    delete tmp;
}

void CWPreview::setBkColor(unsigned int bkColor)
{
    m_bkColor = bkColor;
    m_scene->update();
}

void CWPreview::imageChanged()
{
    m_scene->update();
}

void CWPreview::assignStatic(CFrame *frame)
{
    m_bkColor = 0xffffff;
    m_frame = new CFrame(frame);
    m_state = STATE_STATIC;
}

void CWPreview::mouseReleaseEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    m_mouse.oldX = -1;
    m_mouse.oldY = -1;
    m_mouse.x = -1;
    m_mouse.y = -1;
}

void CWPreview::mousePressEvent(QMouseEvent * event)
{    
    m_mouse.oldX = m_mouse.x;
    m_mouse.oldY = m_mouse.y;
    m_mouse.x = event->x();
    m_mouse.y = event->y();
    m_mouse.lButton = false;
    m_mouse.rButton = false;
    switch (event->button())
    {
    case Qt::LeftButton:
        m_mouse.lButton = true;
        break;

    case Qt::RightButton:
    case Qt::MidButton:
        m_mouse.rButton = true;
        break;
    default:
        break;
    }

    if (m_mouse.oldX != m_mouse.x
            || m_mouse.oldY != m_mouse.y) {
        if (m_mouse.lButton) {
            m_zoom = std::min(10, ++m_zoom);
        }

        if (m_mouse.rButton) {
            m_zoom = std::max(1, --m_zoom);
        }
        //qDebug("zoom:%d", m_zoom);
        changeFrame(m_frame);
        m_scene->update();
        emit zoomChanged(m_zoom);
    }
}

void CWPreview::updateScrollbars()
{
    QSize sz = viewport()->size();
    if (sz.width() >= m_scene->width()) {
        horizontalScrollBar()->setRange(0, 0);
    } else {
        horizontalScrollBar()->setRange(0, (m_scene->width() - sz.width())/ m_zoom + 1);
    }

    if (sz.height() >= m_scene->height()) {
        verticalScrollBar()->setRange(0, 0);
    } else {
        verticalScrollBar()->setRange(0, (m_scene->height() - sz.height()) / m_zoom + 1);
    }

    horizontalScrollBar()->setPageStep(m_zoom);
    verticalScrollBar()->setPageStep(m_zoom);
}

int CWPreview::zoom()
{
    return m_zoom;
}

void CWPreview::setZoom(int zoom)
{
    m_zoom = zoom;
    m_scene->update();
}

void CWPreview::animate(int speed)
{
    m_speed = speed;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(speed);
}

void CWPreview::update()
{
    emit nextFrame(m_frameID);
}

void CWPreview::setNextFrame(int currFrame, CFrame *frame)
{
    m_frame = frame;
    m_frameID = currFrame;
    changeFrame(frame);
}

void CWPreview::setSpeed(int speed)
{
    m_speed = speed;
    m_timer->start(speed);
}

int CWPreview::speed()
{
    return m_speed;
}
