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

#include "Frame.h"
#include <QPixmap>
#include <QWidget>
#include <QTimer>
#include <QBitmap>
#include <QRectF>
#include <QMouseEvent>
#include "previewwidget.h"

CPreviewWidget::CPreviewWidget(QWidget *parent):QOpenGLWidget(parent)
{
    m_ready = false;
    m_bkColor = 0x00ffff;
    m_frame = NULL;
    m_state = STATE_LIVEVIEW;
    m_zoom = 1;
    m_frameID = -1;
    m_timer = NULL;
    m_speed = 0;

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
    setContentsMargins(QMargins(0, 0, 0, 0));

    m_mx = 0;
    m_my = 0;
}

CPreviewWidget::~CPreviewWidget()
{
    if (m_state == STATE_STATIC && m_frame) {
        delete m_frame;
    }
}

void CPreviewWidget::initializeGL()
{
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();
    m_ready = true;
}

void CPreviewWidget::paintGL()
{
    QOpenGLWidget::paintGL();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    drawBackground();
}

void CPreviewWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void CPreviewWidget::changeFrame(CFrame *frame)
{
    m_frame = frame;
    emit scrollbarAdjusted();
    if (m_ready) {
        update();
    }
}

void CPreviewWidget::drawBackground()
{
    QSize sz = size();
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
    glOrtho(0,sz.width(),0,sz.height(),-1,1);
    glViewport(
            0,          // lower left x position
            0,			// lower left y position
            sz.width(),	// viewport width
            sz.height()	// viewport height
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

void CPreviewWidget::drawChecker()
{
    QSize sz = size();
    int gridSize = 32;
    for (int y=0; y * gridSize < sz.height(); ++y) {
        for (int x=0; x * gridSize < sz.width(); ++x) {
            if ((x + y) % 2 == 0) {
                int x1 = x * gridSize;
                int x2 = (x + 1) * gridSize;
                int y1 = sz.height() - y * gridSize;
                int y2 = sz.height() - (y + 1) * gridSize;
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

void CPreviewWidget::drawImage()
{
    if (!m_frame) {
        return;
    }

    QSize sz = size();
    int mx = m_mx;//horizontalScrollBar()->value();
    int my = m_my;//verticalScrollBar()->value();

    int cx = std::min(m_frame->m_nLen - mx, sz.width()/m_zoom) ;
    int cy = std::min(m_frame->m_nHei - my, sz.height()/m_zoom);
    CFrame *tmp = m_frame->clip(mx, my, cx, cy);
    tmp->flipV();

    int x1 = 0;
    //int x2 = cx * m_zoom;
    int y1 = sz.height() - cy * m_zoom;
    //int y2 = r.bottom();

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glRasterPos2d(x1, y1);
    glPixelZoom(m_zoom, m_zoom);
    glDrawPixels(cx, cy,
             GL_RGBA, GL_UNSIGNED_BYTE,
             tmp->getRGB());

    delete tmp;
}

void CPreviewWidget::setBkColor(unsigned int bkColor)
{
    m_bkColor = bkColor;
    //m_scene->update();
    update();
}

void CPreviewWidget::imageChanged()
{
    //m_scene->update();
    update();
}

void CPreviewWidget::assignStatic(CFrame *frame)
{
    m_bkColor = 0xffffff;
    m_frame = new CFrame(frame);
    m_state = STATE_STATIC;
}

void CPreviewWidget::mouseReleaseEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    m_mouse.oldX = -1;
    m_mouse.oldY = -1;
    m_mouse.x = -1;
    m_mouse.y = -1;
}

void CPreviewWidget::mousePressEvent(QMouseEvent * event)
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
        m_mouse.rButton = true;
        break;
    default:
        break;
    }

    if (m_mouse.oldX != m_mouse.x
            || m_mouse.oldY != m_mouse.y) {
        if (m_mouse.lButton) {
            ++m_zoom;
            m_zoom = std::min(10, m_zoom);
        }
        if (m_mouse.rButton) {
            --m_zoom;
            m_zoom = std::max(1, m_zoom);
        }
        changeFrame(m_frame);
        emit zoomChanged(m_zoom);
        update();
    }
}

int CPreviewWidget::zoom()
{
    return m_zoom;
}

void CPreviewWidget::setZoom(int zoom)
{
    m_zoom = zoom;
    update();
}

void CPreviewWidget::animate(int speed)
{
    m_speed = speed;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    m_timer->start(speed);
}

void CPreviewWidget::updateFrame()
{
    emit nextFrame(m_frameID);
}

void CPreviewWidget::setNextFrame(int currFrame, CFrame *frame)
{
    m_frame = frame;
    m_frameID = currFrame;
    changeFrame(frame);
}

void CPreviewWidget::setSpeed(int speed)
{
    m_speed = speed;
    m_timer->start(speed);
}

int CPreviewWidget::speed()
{
    return m_speed;
}

void CPreviewWidget::setMX(int mx)
{
    m_mx = mx;
}

void CPreviewWidget::setMY(int my)
{
    m_my = my;
}
