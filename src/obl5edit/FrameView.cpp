/*
    LGCK Builder package
    Copyright (C) 2005, 2014  Francois Blanchette

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

#include "FrameView.h"
#include "../shared/stdafx.h"
#include "../shared/Frame.h"
#include "../shared/DotArray.h"
#include "../shared/Tools.h"
#include "../shared/FileWrap.h"
#include "Pixel.h"
#include <QWheelEvent>
#include <QRect>
#include <QGraphicsScene>
#include <QtOpenGL>
#include <QMouseEvent>

//#define DEBUGME

#ifdef DEBUGME
CFileWrap debugf;
#endif

CFrameView::CFrameView(QWidget *parent) :
      QGraphicsView(parent)
{
#ifdef DEBUGME
    debugf.open("../debug/frameview.txt", "wb");
#endif
    m_shadow = NULL;
    m_scene = new QGraphicsScene(this);
    m_frame = NULL;
    m_zoom = 8;
    m_mode = MODE_NORMAL;
    m_showTransparency = false;
    m_dots = new CDotArray;
    m_dotsOrg = new CDotArray;
    m_tool = TOOL_PEN;
    m_penSize = 1;
    m_gridColor = 0x804020;
    m_borderColor = 0xffff00;
    m_bkColor = 0x408040;
    m_alpha = 128;
    setScene(m_scene);
    initMouse();

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
    setViewport(new QGLWidget(QGLFormat(QGL::NoSampleBuffers)));
    //m_view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //setRenderHint(QPainter::Antialiasing, false);
    //m_view->setDragMode(QGraphicsView::RubberBandDrag);
    setDragMode(QGraphicsView::NoDrag);
    //setOptimizationFlags(QGraphicsView::DontSavePainterState);
    //m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //m_view->setRenderHint(QPainter::Antialiasing);
    //m_view->setBackgroundBrush(QPixmap(":/images/bkground.png"));
    //m_view->setBackgroundBrush(QBrush(Qt::lightGray, Qt::CrossPattern));
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    // If cache is set, the OpenGL won't work
    //m_view->setCacheMode(QGraphicsView::CacheBackground);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setMouseTracking(true);
}

CFrameView::~CFrameView()
{
    delete m_dots;
    delete m_dotsOrg;
    if (m_shadow) {
        delete m_shadow;
    }
#ifdef DEBUGME
    debugf.close();
#endif
}

void CFrameView::debug(const char *s)
{
#ifdef DEBUGME
    debugf.write(s, strlen(s));
    debugf.write("\n",1);
#else
    Q_UNUSED(s);
#endif
}

void CFrameView::wheelEvent(QWheelEvent *e)
{
    // TODO: implement wheel event
    if (e->modifiers() & Qt::ControlModifier) {
        int zoom = m_zoom;
        if (e->delta() > 0){
            m_zoom = std::min(16, m_zoom * 2);
            if (m_zoom == 2) {
                m_zoom = 4;
            }
        }
        else {
            m_zoom = std::max(1, m_zoom / 2);
            if (m_zoom == 2) {
                m_zoom = 1;
            }
        }

        if (m_zoom != zoom ){
            emit zoomChanged(m_zoom);
            changeZoom(m_zoom);
        }
        e->accept();
    } else {
        QGraphicsView::wheelEvent(e);
    }
}

void CFrameView::changeFrame(CFrame *frame)
{
#ifdef DEBUGME
    qDebug("vendor: %s", glGetString(GL_VENDOR));
    qDebug("renderer: %s", glGetString(GL_RENDERER));
    qDebug("version: %s", glGetString(GL_VERSION));
    qDebug("extensions: %s", glGetString(GL_EXTENSIONS));
#endif
    int gridSize = m_zoom;
    m_frame = frame;
    m_scene->clear();
    if (frame && m_mode != MODE_TILED_VIEW){
        m_scene->setSceneRect(0, 0, frame->m_nLen * gridSize, frame->m_nHei * gridSize);
    } else {
        m_scene->setSceneRect(0, 0, 1, 1);
    }

    // add dummy item to allow screen to be refreshed
    QGraphicsItem *item = new CPixel(NULL, NULL,0,0);
    item->setPos(QPoint(0,0));
    m_scene->addItem(item);
    updateScrollbars();
    m_scene->update();
}

void CFrameView::setBkColor(UINT32 color)
{
    m_bkColor = color;
    QList<QRectF> list;
    list.append(m_scene->sceneRect());
    updateScene(list);
}

void CFrameView::setGridColor(UINT32 color)
{
    m_gridColor = color;
}

void CFrameView::setBorderColor(UINT32 color)
{
    m_borderColor = color;
}

void CFrameView::setPenColor(UINT32 penColor)
{
    m_penColor = penColor;
}

void CFrameView::enableGrid(bool enable)
{
    m_showGrid = enable;
}

void CFrameView::changeZoom(int px)
{
    m_zoom = px;
    changeFrame(m_frame);
}

void CFrameView::initMouse()
{
    m_mouse.mButton = m_mouse.lButton = m_mouse.rButton = false;
    m_mouse.oldX = m_mouse.oldY = m_mouse.x = m_mouse.y = -1;
}

void CFrameView::changeCursor()
{
    QPixmap pixmap;
 //  qDebug("mode %d", m_mode);
    if (m_frame && (m_mode != MODE_TILED_VIEW)) {
        switch (m_tool){

        case TOOL_PEN:
            setCursor(Qt::ArrowCursor);
            break;

        case TOOL_ERASER:
            pixmap.load(":/images/cursors/pen.png");
            pixmap.setMask( pixmap.createHeuristicMask() );
            setCursor(QCursor(pixmap, 1, 2));
            break;

        case TOOL_FLOODFILL:
            pixmap.load(":/images/cursors/paint_bucket.png");
            pixmap.setMask( pixmap.createHeuristicMask() );
            setCursor(QCursor(pixmap, 3, 16));
            break;

        case TOOL_LINE:
            setCursor(Qt::CrossCursor);
            break;

        case TOOL_RECT:
            setCursor(Qt::CrossCursor);
            break;

        case TOOL_CIRCLE:
            setCursor(Qt::CrossCursor);
            break;

        case TOOL_COLORSELECT:
            pixmap.load(":/images/cursors/eyedropper.png");
            pixmap.setMask( pixmap.createHeuristicMask() );
            setCursor(QCursor(pixmap, 2, 14));
            break;

        case TOOL_SELECT:
            setCursor(Qt::CrossCursor);
            break;

        case TOOL_MAP:
            pixmap.load(":/images/cursors/link_1.png");
            pixmap.setMask( pixmap.createHeuristicMask() );
            setCursor(QCursor(pixmap, 6, 2));
            break;

        case TOOL_ZOOM:
            pixmap.load(":/images/cursors/tinyglass.png");
            pixmap.setMask( pixmap.createHeuristicMask() );
            setCursor(QCursor(pixmap, 6, 5));
            break;
        }
    } else {
        setCursor(Qt::ForbiddenCursor);
    }
}

void CFrameView::mousePressEvent(QMouseEvent * event)
{
    QList<int> tools;
    tools.append(TOOL_PEN);
    tools.append(TOOL_ERASER);
    tools.append(TOOL_FLOODFILL);
    tools.append(TOOL_LINE);
    tools.append(TOOL_RECT);
    tools.append(TOOL_CIRCLE);

    switch (event->button())
    {
    case Qt::LeftButton:
        m_mouse.lButton = true;
        if (m_frame && (m_mode != MODE_TILED_VIEW)
                && tools.indexOf(m_tool) != -1) {
            m_frame->push();
            emit modified();                        
        }
        break;

    case Qt::RightButton:
        m_mouse.rButton = true;
        if (m_frame && (m_mode != MODE_TILED_VIEW)
                && tools.indexOf(m_tool) != -1) {
            m_frame->push();
            emit modified();
        }
        break;

    case Qt::MidButton:
        m_mouse.rButton = true;
        break;

    default:
        break;
    }
#ifdef DEBUGME
    debug("mouseDown");
#endif
    setFocus();
    if (m_mouse.lButton | m_mouse.rButton) {
        handleTool();
    }
}

void CFrameView::mouseReleaseEvent(QMouseEvent * event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
        m_mouse.lButton = false;
        break;
    case Qt::RightButton:
        m_mouse.rButton = false;
        break;
    case Qt::MidButton:
        m_mouse.mButton = false;
        break;
    default:
        break;
    }
    setFocus();
#ifdef DEBUGME
    debug("mouseUP");
#endif
    if (m_dots->getSize()) {
        m_dots->flush();
    }
    emit mouseUp();
}

void CFrameView::mouseMoveEvent(QMouseEvent * event)
{
    // MouseMove isn't called unless a button is pressed
    int gridSize = m_zoom;
    QSize sz = viewport()->size();
    int xx = -1;
    int yy = -1;
    int x = -1;
    int y = -1;
    if (event->x()>=0 && event->y()>=0) {
        xx = (event->x() + gridSize * horizontalScrollBar()->value()) ;
        yy = (event->y() + gridSize * verticalScrollBar()->value());
        if (xx < m_scene->width() && yy < m_scene->height()) {
            x = xx / gridSize;
            y = yy / gridSize;
        }
    }

#ifdef DEBUGME
    Qt::MouseButtons btn = event->buttons();
    char tmp[1024];
    sprintf(tmp, "MouseMOVE l:%d r:%d m:%d\nEvent: x=%d y=%d\nx=%d y=%d",
          btn & Qt::LeftButton,
          btn & Qt::RightButton,
          btn & Qt::MiddleButton,
          event->x(), event->y(),
          x,y
      );
    debug(tmp);
#endif

    // Test if mouse in within valid coordonates
    if (event->x() >= sz.width()
             ||event->y() >= sz.height() ) {
        setCursor(Qt::ArrowCursor);
        return;
    }
    m_mouse.oldX = m_mouse.x;
    m_mouse.oldY = m_mouse.y;
    m_mouse.x = x;
    m_mouse.y = y;

    QString s = "";
    if (x < 0 || y < 0) {
        setCursor(Qt::ForbiddenCursor);
    } else {
        if (m_frame) {
            uint rgb = m_frame->at(x,y);
            char hex[12];
            int red = rgb & 0xff;
            int green = (rgb >> 8) & 0xff;
            int blue = (rgb >> 16) & 0xff;
            int alpha = (rgb >> 24) & 0xff;
            sprintf(hex, "%.2X%.2X%.2X:%.2X", red, green, blue, alpha);
            s = QString("x=%1 y=%2 (#%3)").arg(x).arg(y).arg(hex);
        }
        changeCursor();
        if (m_mouse.lButton | m_mouse.rButton) {
            handleTool();
        }
    }
    emit statusUpdate(0,s);
}

void CFrameView::updateScrollbars()
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

void CFrameView::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)
    updateScrollbars();
}

void CFrameView::mouseDoubleClickEvent(QMouseEvent * event)
{
    event->accept();
}

void CFrameView::handleTool()
{
    // TODO: adjust for zoom
    // disallow negative coordonates
    // and other out of bound value
    if (m_mouse.x < 0 || m_mouse.y < 0
            || (m_mode == MODE_TILED_VIEW)) {
        return;
    }
    m_dots->setLimit(m_frame->m_nLen, m_frame->m_nHei);
    m_dotsOrg->setLimit(m_frame->m_nLen, m_frame->m_nHei);

#ifdef DEBUGME
    char tmp[1024];
        sprintf(tmp, "m_dots->getSize(): %d", m_dots->getSize());
        debug(tmp);
#endif

    bool changed = false;
    int x = m_mouse.x;
    int y = m_mouse.y;
    if (m_frame && x < m_frame->m_nLen && y < m_frame->m_nHei) {
        // TODO: set doc Dirty
        UINT32 oldColor = m_frame->at(x,y);
        uchar oldAlpha = (uchar) (m_frame->at(x,y) >> 24);
        Dot org;
        Dot dot;
        uchar alpha = m_alpha;
        UINT32 penColor = (m_penColor & 0xffffff)+ (m_alpha << 24);
        if (m_mouse.rButton) {
            penColor = 0;
            alpha = 0;
        }
        // PEN TOOL
        switch (m_tool) {

        case TOOL_ERASER:
            penColor = 0;
            alpha = 0;

        case TOOL_PEN:
            if (m_dots->getSize()) {
                dot.x = x;
                dot.y = y;
                org = (*m_dots)[m_dots->getSize() - 1];
                m_dots->lineTab( penColor , org, dot);
            } else {
                Dot dot;
                dot.x = x;
                dot.y = y;
                m_dots->lineTab( penColor, dot, dot);
            }
            changed = m_frame->draw(m_dots, m_penSize, m_mode);
            break; // TOOL_PEN

        case TOOL_LINE:
            if (m_dots->getSize()) {
                dot.x = x;
                dot.y = y;
                org = (*m_dots)[0];
                m_dots->lineTab( penColor, org, dot);
                // restore older dots
                m_frame->draw(m_dotsOrg, 1, m_mode);
            } else {
                Dot dot;
                dot.x = x;
                dot.y = y;
                m_dots->lineTab( penColor, dot, dot);
            }

            // save original dots
            m_dotsOrg->flush();
            m_frame->save(m_dots, m_dotsOrg, m_penSize);
            changed = m_frame->draw(m_dots, m_penSize, m_mode);

            break; // TOOL_LINE

        case TOOL_RECT:
            if (m_dots->getSize()) {
                dot.x = x;
                dot.y = y;
                org = (*m_dots)[0];

                m_dots->lineTab( penColor, org, Dot(x, org.y));
                m_dots->lineTab( penColor, org, Dot(org.x, y), false);
                m_dots->lineTab( penColor, Dot(x, org.y), Dot(x, y), false);
                m_dots->lineTab( penColor, Dot(org.x,y), Dot(x, y), false);

                // restore older dots
                m_frame->draw(m_dotsOrg, 1, m_mode);
            } else {
                Dot dot;
                dot.x = x;
                dot.y = y;
                m_dots->lineTab( penColor, dot, dot);
            }

            // save original dots
            m_dotsOrg->flush();
            m_frame->save(m_dots, m_dotsOrg, m_penSize);
            changed = m_frame->draw(m_dots, m_penSize, m_mode);

        break; // TOOL_RECT

        case TOOL_FLOODFILL:
            if (m_mode != MODE_ALPHA_ONLY) {
                if (penColor != oldColor) {
                    m_frame->floodFill(x,y, oldColor, penColor);
                    changed = true;
                }
            } else {
                // TODO: transparency
                if (alpha != oldAlpha) {
                    m_frame->floodFillAlpha(x,y, oldAlpha, alpha);
                    changed = true;
                }
            }
            break; // TOOL_FLOODFILL

        case TOOL_COLORSELECT:
            switch(m_mode) {
            case MODE_NORMAL:
                m_penColor = m_frame->at(x,y) & 0xffffff;
                emit penColorChanged(m_penColor);
                m_alpha = (uchar) (m_frame->at(x,y) >> 24);
                emit alphaChanged(m_alpha);
                break;
            case MODE_COLOR_ONLY:
                m_penColor = m_frame->at(x,y) & 0xffffff;
                emit penColorChanged(m_penColor);
                break;
            case MODE_ALPHA_ONLY:
                m_alpha = (uchar) (m_frame->at(x,y) >> 24);
                emit alphaChanged(m_alpha);
            };
            break; // TOOL_COLORSELECT

        case TOOL_ZOOM:
            if (!m_dots->getSize()) {
                if (m_mouse.lButton) {
                    m_zoom = std::min(16, m_zoom * 2);
                    if (m_zoom == 2) {
                        m_zoom = 4;
                    }
                }

                if (m_mouse.rButton) {
                    m_zoom = std::max(1, m_zoom / 2);
                    if (m_zoom == 2) {
                        m_zoom = 1;
                    }
                }

                emit zoomChanged(m_zoom);
                changeZoom(m_zoom);

                m_dots->add(0,0,0);
            }
            break; // TOOL_ZOOM

        case TOOL_SELECT:
            break; // TOOL_SELECT

        case TOOL_MAP:
            break; // TOOL_MAP

        case TOOL_CIRCLE:
            if (m_dots->getSize()) {
                dot.x = x;
                dot.y = y;
                org = (*m_dots)[0];
                m_dots->circle(penColor, org, dot);
                // restore older dots
                m_frame->draw(m_dotsOrg, 1, m_mode);
            } else {
                Dot dot;
                dot.x = x;
                dot.y = y;
                m_dots->add(dot);
                org = dot;
            }

            // save original dots
            m_dotsOrg->flush();
            m_frame->save(m_dots, m_dotsOrg, m_penSize);
            changed = m_frame->draw(m_dots, m_penSize, m_mode);

            // save origin dot
            m_dots->flush();
            m_dots->add(org);
            break; // TOOL_CIRCLE
        }
    }

    if (changed) {
        m_scene->update();
    }
}

void CFrameView::setPenSize(int penSize)
{
    m_penSize = penSize;
}

void CFrameView::setTool(int tool)
{
    m_tool = tool;
    changeCursor();
}

void CFrameView::changeImage()
{
    m_dots->flush();
    changeCursor();
}

void CFrameView::setShadow(CFrame *frame)
{
    if (m_shadow) {
        delete m_shadow;
    }

    if (frame) {
        m_shadow = new CFrame(frame);
        m_shadow->shadow(2);
    } else {
        m_shadow = NULL;
    }
}

void CFrameView::drawBackground(QPainter *painter, const QRectF & rect)
{
    Q_UNUSED(rect);
    Q_UNUSED(painter);
    QRectF r = frameRect();
    // Checker rgb: 4080ff, 5090ff
    if (m_frame) {
        // border color
        uint color = m_borderColor;
        if (m_mode == MODE_TILED_VIEW) {
            color = m_bkColor;
        }
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
            if (m_mode == MODE_TILED_VIEW) {
                drawTapestry();
            } else {
                drawImage();
            }
        } else {
            drawChecker();
        }
    glPopMatrix();
}

void CFrameView::drawChecker()
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

void CFrameView::drawTapestry()
{   
    GLuint texName;
    // Enable Alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_frame->m_nLen, m_frame->m_nHei, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0 , m_frame->m_nLen, m_frame->m_nHei, GL_RGBA, GL_UNSIGNED_BYTE, m_frame->getRGB() );

    QRectF r = frameRect();
    int x1 = 0;
    int x2 = r.right();
    int y1 = 0;
    int y2 = r.bottom();
    float xx = x2 / m_frame->m_nLen;
    float yy = y2 / m_frame->m_nHei;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(x1, y2, 0.0);
        glTexCoord2f(0.0, yy); glVertex3f(x1, y1, 0.0);
        glTexCoord2f(xx, yy); glVertex3f(x2, y1, 0.0);
        glTexCoord2f(xx, 0.0); glVertex3f(x2, y2, 0.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDeleteTextures(1, &texName);
}

void CFrameView::drawImage()
{
    int mx = horizontalScrollBar()->value();
    int my = verticalScrollBar()->value();

    QRect r = frameRect();
    int gridSize = m_zoom;
    int w = std::min(r.right(), (m_frame->m_nLen - mx) * gridSize);
    int h = std::min(r.bottom(), (m_frame->m_nHei - my) * gridSize);

    // draw background
    if (w > 0 && h > 0) {
        uint rgb = m_bkColor;
        int x1 = 0;
        int x2 = w;
        int y1 = r.bottom() - h;
        int y2 = r.bottom();

        float red = (rgb & 0xff) / 255.0f;
        float green = ((rgb >> 8) & 0xff ) / 255.0f;
        float blue = ((rgb >> 16) & 0xff) / 255.0f;
        glColor4f(red, green, blue, 1.0f);
        glRectf(x1, y1, x2, y2);
    }

    // http://www.opengl.org/archives/resources/faq/technical/transparency.htm
    if (m_zoom != 1) {        
        CFrame *frame = m_frame;
        if (m_mode == MODE_ALPHA_ONLY) {
            frame = m_frame->toAlphaGray();
        }
        // draw zoomed image
        glLineWidth(0.5f);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        float gr_red = (m_gridColor & 0xff) / 255.0f;
        float gr_green = ((m_gridColor >> 8) & 0xff ) / 255.0f;
        float gr_blue = ((m_gridColor >> 16) & 0xff) / 255.0f;
        float br_red = (m_borderColor & 0xff) / 255.0f;
        float br_green = ((m_borderColor >> 8) & 0xff ) / 255.0f;
        float br_blue = ((m_borderColor >> 16) & 0xff) / 255.0f;
        uint rgb;
        for (int y=0; y * gridSize < r.bottom() && (my + y) < m_frame->m_nHei; ++y) {
            for (int x=0; x * gridSize < r.right() && (mx + x) < m_frame->m_nLen; ++x) {
                int x1 = x * gridSize;
                int x2 = (x + 1) * gridSize;
                int y1 = r.bottom() - y * gridSize;
                int y2 = r.bottom() - (y + 1) * gridSize;
                if (m_showGrid) {
                    y2 += 1;
                    // if draw grid
                    glBegin(GL_LINE_LOOP);
                        if ( (mx + x) % 8 == 7 || (my + y) % 8 == 7) {
                            glColor4f(br_red, br_green, br_blue, 1.0f);
                        }
                        else {
                            glColor4f(gr_red, gr_green, gr_blue, 1.0f);
                        }
                        glVertex2f(x1, y1);
                        glVertex2f(x2, y1);
                        glVertex2f(x2, y2);
                        glVertex2f(x1, y2);
                    glEnd();
                    x2 -= 1;
                }

                // draw shadow if applicable
                if ((m_mode != MODE_ALPHA_ONLY) &&m_shadow
                        && (x + mx) < m_shadow->m_nLen
                        && (y + my) < m_shadow->m_nHei) {
                    rgb = m_shadow->at(mx + x, my + y);
                    if (rgb & 0xff000000) {
                        // if not transparent
                        float red = (rgb & 0xff) / 255.0f;
                        float green = ((rgb >> 8) & 0xff ) / 255.0f;
                        float blue = ((rgb >> 16) & 0xff) / 255.0f;
                        float alpha = ((rgb >> 24) & 0xff) / 255.0f;
                        glColor4f(red, green, blue, alpha);
                        glRectf(x1, y1, x2, y2);
                    }
                }

                // draw pixel
                if (m_mode != MODE_ALPHA_ONLY) {
                    rgb = m_frame->at(mx + x, my + y);
                }else {
                    rgb = frame->at(mx + x, my + y);
                }
                if (rgb & 0xff000000) {
                    // if not transparent
                    float red = (rgb & 0xff) / 255.0f;
                    float green = ((rgb >> 8) & 0xff ) / 255.0f;
                    float blue = ((rgb >> 16) & 0xff) / 255.0f;
                    float alpha = ((rgb >> 24) & 0xff) / 255.0f;
                    glColor4f(red, green, blue, alpha);
                    glRectf(x1, y1, x2, y2);
                }

                // show transparent pixels
                if ((m_mode != MODE_ALPHA_ONLY) && m_showTransparency
                        && !(m_frame->at(mx + x, my + y) & 0xff000000)) {
                    x1 += (gridSize/2 -1);
                    y1 -= (gridSize/2 -1);
                    if (m_zoom > 4) {
                        x2 = x1 + 2;
                        y2 = y1 - 2;
                    } else {
                        x2 = x1 + 1;
                        y2 = y1 - 1;
                    }
                    glColor4f(br_red, br_green, br_blue, 1.0f);
                    glRectf(x1, y1, x2, y2);
                }
            }
        }
        if (frame != m_frame) {
            delete frame;
        }
    } else {
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

        int cx = m_frame->m_nLen - mx;
        int cy = m_frame->m_nHei - my;
        CFrame *tmp;
        if (m_mode == MODE_ALPHA_ONLY) {
            tmp = m_frame->toAlphaGray(mx, my, cx, cy);
        } else {
            tmp = m_frame->clip(mx, my, cx, cy);
        }
        // TODO: quickfix for quirky textures
        int ix = pow2roundup(tmp->m_nLen);
        int iy = pow2roundup(tmp->m_nHei);
        if (ix != tmp->m_nLen|| iy != tmp->m_nHei) {
            tmp->resize(ix,iy);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ix, iy, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp->getRGB() );
        delete tmp;
        int x1 = 0;
        int x2 = ix;
        int y1 = r.bottom() - iy;
        int y2 = r.bottom();
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
    }
    glDisable(GL_BLEND);
}

void CFrameView::setTransparency(bool enabled)
{
    m_showTransparency = enabled;
    m_scene->update();
}

int CFrameView::zoom()
{
    return m_zoom;
}

bool CFrameView::showGrid()
{
    return m_showGrid;
}

bool CFrameView::showTransparency()
{
    return m_showTransparency;
}

int CFrameView::tool()
{
    return m_tool;
}

int CFrameView::penSize()
{
    return m_penSize;
}

uint CFrameView::borderColor()
{
    return m_borderColor;
}

uint CFrameView::gridColor()
{
    return m_gridColor;
}

uint CFrameView::bkColor()
{
    return m_bkColor;
}

uint CFrameView::penColor()
{
    return m_penColor;
}

void CFrameView::setAlpha(uchar alpha)
{
    m_alpha = alpha;
}

uchar CFrameView::alpha()
{
    return m_alpha;
}

void CFrameView::setViewMode(int mode)
{
    m_mode = mode;
}

int CFrameView::viewMode()
{
    return m_mode;
}
