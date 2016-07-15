/*
    LGCK Builder GUI
    Copyright (C) 1999, 2013  Francois Blanchette

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

#include <QResizeEvent>
#include <QSize>
#include <QPoint>
#include <QGraphicsScene>
#include <QGLWidget>
#include <QtOpenGL>
#include "../shared/qtgui/cheat.h"
#include "../shared/stdafx.h"
#include "../shared/FileWrap.h"
#include "../shared/Frame.h"
#include "../shared/Game.h"
#include "../shared/Level.h"
#include "../shared/Layer.h"
#include "../shared/PathBlock.h"
#include "../shared/Path.h"
#include "../shared/Const.h"
#include "../shared/implementers/opengl/im_opengl.h"
#include "../shared/implementers/opengl/gr_opengl.h"
//#include "../shared/implementers/opengl/dm_opengl.h"
#include "LevelView.h"
#include "Pixel.h"
#include "../shared/inputs/qt/kt_qt.h"

// http://qt-project.org/doc/qt-5.0/qtopengl/qglwidget.html#context

CLevelView::CLevelView(QWidget* parent): QGraphicsView(parent)
{            
    m_scene = new QGraphicsScene(this);
    setAcceptDrops(true);
    m_game = NULL;
    m_gameMode = false;
    m_editPath = false;
//    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_MouseTracking);
    setMouseTracking(true);
//    setAttribute(Qt::WA_PaintOnScreen);
    //setAttribute(Qt::WA_NoSystemBackground);
    // Set strong focus to enable keyboard events to be received
    setFocusPolicy(Qt::StrongFocus);
    m_ticks = 0;
    m_gridSize = 32;
    m_showGrid = true;
    m_gridColor = 0x60c0b0a0;
    m_hasFocus = false;
    m_zoom = 1;
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

    m_glWidget = new QGLWidget(QGLFormat(QGL::NoSampleBuffers));
    setViewport(m_glWidget);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setDragMode(QGraphicsView::NoDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    //m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // If cache is set, then OpenGL won't work
    //m_view->setCacheMode(QGraphicsView::CacheBackground);
    setContentsMargins(QMargins(0, 0, 0, 0));

    // force refresh
    changeLevel();
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(mxChanged(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(myChanged(int)));
    connect(this, SIGNAL(setMX(int)), horizontalScrollBar(), SLOT(setValue(int)));
    connect(this, SIGNAL(setMY(int)), verticalScrollBar(), SLOT(setValue(int)));
}

CLevelView::~CLevelView()
{
    delete m_game->graphics();
    m_game->attach((IGraphics*) NULL);
    delete m_game->cache();
    m_game->attach((IImageManager*)NULL);
}

void CLevelView::keyReflector(const QKeyEvent *event, bool pressed)
{   
    emit(keyChanged(CKeyTranslator::translate(event->key()), (int) pressed));
}

void CLevelView::mxChanged(int mx)
{
    if (m_game && m_game->m_nLevels) {
        CLevel * level = (*m_game)[m_game->m_nCurrLevel];
        level->m_mx = mx;
    }
}

void CLevelView::myChanged(int my)
{
    if (m_game && m_game->m_nLevels) {
        CLevel * level = (*m_game)[m_game->m_nCurrLevel];
        level->m_my = my;
    }
}

void CLevelView::initMouse()
{
    m_mouse.drag = m_mouse.mButton = m_mouse.lButton = m_mouse.rButton = false;
    m_mouse.orgX = m_mouse.orgY = m_mouse.oldX = m_mouse.oldY = m_mouse.x = m_mouse.y = -1;
}

void CLevelView::mousePressEvent ( QMouseEvent * event )
{
    switch (event->button())
    {
        case Qt::LeftButton:
            m_mouse.lButton = true;
        break;

        case Qt::RightButton:
            m_mouse.rButton = true;
        break;

        case Qt::MidButton:
            m_mouse.rButton = true;
        break;

        default:
        break;
    }

    if (this->isGameMode()) {
        if (m_game) {
            CGame & gf = *m_game;
            gf.triggerMouseEvent(m_mouse.x, m_mouse.y, event->button());
        }
    } else {
        if (m_game && m_game->m_nLevels && m_mouse.lButton
                && !m_editPath) {
            CGameFile & gf = *m_game;
            CLevel & level = *(gf[gf.m_nCurrLevel]);
            CLayer & layer = * level.getCurrentLayer();
            if (m_mouse.lButton || m_mouse.rButton) {
                int oldCurrEntry = layer.getSelectionIndex(0);
                int selCount = layer.getSelectionSize();                
                m_mouse.x = event->x() & MOUSE_POS_MASK;
                m_mouse.y = event->y() & MOUSE_POS_MASK;
                int index = gf.whoIs( level, m_mouse.x + level.m_mx, m_mouse.y + level.m_my);
                if (index != -1) {
                    if (layer.isInSelection(index)) {
                        if (m_game->testKey(lgck::Key::LControl)) {
                            layer.removeFromSelection( index );
                        }
                    } else {
                        if (m_game->testKey(lgck::Key::LControl)) {
                            layer.select( index );
                        } else {
                            layer.selectSingle( index );
                        }
                    }

                } else {
                    if (!m_game->testKey(lgck::Key::LControl)) {
                        layer.clearSelection();
                    }
                }

                if (oldCurrEntry != layer.getSelectionIndex(0)
                        || selCount != layer.getSelectionSize()) {
                    qDebug("menu changed");
                    emit menuChanged();
                }

                // if no selection
                if (!layer.getSelectionSize()) {
                    m_mouse.destX = m_mouse.orgX = event->x() ;
                    m_mouse.destY = m_mouse.orgY = event->y() ;
                    return;
                }
            }
        }
    }
    m_mouse.orgX = m_mouse.orgY = -1;
    setFocus();
}

void CLevelView::mouseReleaseEvent ( QMouseEvent * event )
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
    if (!m_mouse.lButton) {
        m_mouse.drag = false;
    }
    if (!this->isGameMode() && m_game && m_game->m_nLevels
            && !m_editPath) {
        CGameFile & gf = *m_game;
        CLevel & level = *(gf[gf.m_nCurrLevel]);
        CLayer & layer = * level.getCurrentLayer();
        if (!layer.getSelectionSize()
                && !m_mouse.lButton
                && m_mouse.orgX != -1
                && m_mouse.orgY != -1) {
            QPoint pointS (m_mouse.orgX, m_mouse.orgY);
            QPoint pointE (m_mouse.destX, m_mouse.destY);
            int x1 = std::min(pointS.x(), pointE.x());
            int x2 = std::max(pointS.x(), pointE.x());
            int y1 = std::min(pointS.y(), pointE.y());
            int y2 = std::max(pointS.y(), pointE.y());
            select(x1,y1,x2,y2);
            m_mouse.orgX = -1;
            m_mouse.orgY = -1;
        }
    }
}

void CLevelView::mouseMoveEvent(QMouseEvent *event)
{
    QSize sz = getSize();
    m_mouse.oldX = m_mouse.x;
    m_mouse.oldY = m_mouse.y;
    m_mouse.x = event->x() & 0xfffffff8;
    m_mouse.y = event->y() & 0xfffffff8;
    if (!this->isGameMode() && !m_editPath) {
        QString s = "";
        if (m_mouse.x >= 0 && m_mouse.y >= 0) {
           s = tr("x=%1 y=%2").arg(m_mouse.x).arg(m_mouse.y);
        }
        emit statusChanged(0, s);
        if (m_game && m_game->m_nLevels) {
            CGame & gf = *m_game;
            CLevel & level = *(gf[gf.m_nCurrLevel]);
            CLayer & layer = * level.getCurrentLayer();
            if (layer.getSelectionSize()) {
                for (int i=0; i < layer.getSelectionSize(); ++i) {
                    CLevelEntry & entry = layer.getSelection(i);
                    if (m_mouse.drag && (m_mouse.x < 0 || m_mouse.y < 0
                       || m_mouse.x >= sz.width() || m_mouse.y >= sz.height())) {
                        m_mouse.drag = false;                        
                        layer.clearSelection();
                        break;
                    }
                    if (m_mouse.drag) {
                        int tX = m_mouse.x - m_mouse.oldX;
                        int tY = m_mouse.y - m_mouse.oldY;
                        if ( (tX | tY) ){
                            gf.setDirty( true );
                        }
                        entry.m_nX += tX;
                        entry.m_nY += tY;
                    }
                    if (m_mouse.lButton) {
                        m_mouse.drag = true;
                    }

                    if (entry.m_nX < 0) {
                        entry.m_nX = 0;
                        m_mouse.drag = false;
                    }

                    if (entry.m_nY < 0) {
                        entry.m_nY = 0;
                        m_mouse.drag = false;
                    }
                }
            } else {
                if (!layer.getSelectionSize()
                        && m_mouse.lButton
                        && m_mouse.orgX != -1
                        && m_mouse.orgY != -1) {
                    m_mouse.destX = event->x();
                    m_mouse.destY = event->y();
                }
            }
        }
    }
}

void CLevelView::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
}

void CLevelView::drawSelection()
{
    QSize sz = getSize();
    if (m_mouse.orgX != -1 && m_mouse.orgY != -1) {
        int x1 = std::min(m_mouse.orgX, m_mouse.destX);
        int x2 = std::max(m_mouse.orgX, m_mouse.destX);
        int y1 = sz.height() - std::min(m_mouse.orgY, m_mouse.destY);
        int y2 = sz.height() - std::max(m_mouse.orgY, m_mouse.destY);
        float red = (rand() & 255) / 255.0f;
        float green = (rand() & 255) / 255.0f;
        float blue = (rand() & 255) / 255.0f;
        glColor3f(red, green, blue);
        glBegin(GL_LINE_LOOP);
            glVertex3f(x1, y2, 0.0);
            glVertex3f(x1, y1, 0.0);
            glVertex3f(x2, y1, 0.0);
            glVertex3f(x2, y2, 0.0);
        glEnd();
    }
}

void CLevelView::drawItemRect()
{
    QSize sz = getSize();
    if (m_game && m_game->m_nLevels) {
        CGame & gf = *m_game;
        CLevel & level = *(gf[gf.m_nCurrLevel]);
        CLayer & layer = * level.getCurrentLayer();
        float red = (rand() & 255) / 255.0f;
        float green = (rand() & 255) / 255.0f;
        float blue = (rand() & 255) / 255.0f;
        glLineWidth(0.5f);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (int i=0; i < layer.getSelectionSize(); ++i) {
            CLevelEntry & entry = layer.getSelection(i);
            CFrameSet & filter = * gf.m_arrFrames [entry.m_nFrameSet];
            CFrame * pFrame = filter [entry.m_nFrameNo];
            int x = entry.m_nX - level.m_mx;
            int y = entry.m_nY - level.m_my;
            if ((x >= 0) &&
                (y >= 0) &&
                (x < sz.width()) &&
                (y < sz.height())) {
                int x1 = x;
                int x2 = x + pFrame->m_nLen;
                int y1 = sz.height() - y;
                int y2 = sz.height() - y -pFrame->m_nHei;
                glBegin(GL_LINE_LOOP);
                    glColor3f(red, green, blue);
                    glVertex3f(x1, y2, 0.0);
                    glVertex3f(x1, y1, 0.0);
                    glVertex3f(x2, y1, 0.0);
                    glVertex3f(x2, y2, 0.0);
                glEnd();
            }
        }
    }
    glColor4f(1, 1, 1, 1);
}

void CLevelView::drawGrid()
{
    QSize sz = getSize();
    int mx = horizontalScrollBar()->value();
    int my = verticalScrollBar()->value();
    int w = std::min(sz.width(), (MAX_PIXEL - mx));
    int h = std::min(sz.height(), (MAX_PIXEL - my));

    glLineWidth(0.5f);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float gr_red = (m_gridColor & 0xff) / 255.0f;
    float gr_green = ((m_gridColor >> 8) & 0xff ) / 255.0f;
    float gr_blue = ((m_gridColor >> 16) & 0xff) / 255.0f;
    float gr_alpha = (m_gridColor >> 24) ;
    glColor4f(gr_red, gr_green, gr_blue, gr_alpha / 255.0f);

    for (int x = m_gridSize - mx ; x < w; x+= m_gridSize) {
        glBegin(GL_LINES);
        glVertex2f(x, 0.0);
        glVertex2f(x, h);
        glEnd();
    }

    for (int y = m_gridSize - my; y < h; y+= m_gridSize) {
        glBegin(GL_LINES);
        glVertex2f(0.0, h - y);
        glVertex2f(w, h - y);
        glEnd();
    }
}

const QSize CLevelView::getSize()
{
    return m_glWidget->frameSize();
}

void CLevelView::drawScreen()
{
    CLevel & level = * (*m_game)[m_game->m_nCurrLevel];
    int mx = horizontalScrollBar()->value();
    int my = verticalScrollBar()->value();

    QSize sz = getSize();
    int w = std::min(sz.width(), (MAX_PIXEL - mx));
    int h = std::min(sz.height(), (MAX_PIXEL - my));

    // draw background
    if (w > 0 && h > 0) {
        int x1 = 0;
        int x2 = w;
        int y1 = sz.height() - h;
        int y2 = sz.height();
        // Color: #RRGGBB
        int bkColor = strtol(level.getSetting("bkcolor"), NULL, 16);
        float blue = (bkColor & 0xff) << 16;
        float green = (bkColor & 0xff00) >> 8;
        float red = (bkColor >> 16) & 0xff;
        glColor4f(red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);
        glRectf(x1, y1, x2, y2);
    }

    CIMOpengl * im = (CIMOpengl *)m_game->cache();
    if (!im) {
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CLayer & layer = * level.getCurrentLayer();
    int colorMod = strtol(level.getSetting("colorMod"), NULL, 16);
    float blue = (colorMod & 0xff);// << 16;
    float green = (colorMod & 0xff00) >> 8;
    float red = (colorMod >> 16) & 0xff;
    glColor4f(red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);

    for (int i=0; i < layer.getSize(); ++i) {
        CLevelEntry & entry = layer[i] ;
        CFrameSet & filter = * m_game->m_arrFrames [entry.m_nFrameSet];
        CFrame *pFrame = filter[entry.m_nFrameNo];
        int x = entry.m_nX - mx;
        int y = entry.m_nY - my;
        if (!( (x + pFrame->m_nLen <= 0) ||
            (x >= w) ||
            (y + pFrame->m_nHei <= 0) ||
            (y >= h)) ) {
            unsigned int texture = im->getImage(entry.m_nFrameSet, entry.m_nFrameNo);
            if (entry.m_nTriggerKey & CGame::TRIGGER_HIDDEN){
                if (!im->hasInverse( entry.m_nFrameSet )) {
                    im->cacheInverse( entry.m_nFrameSet, & filter);
                }
                texture = im->getImage(entry.m_nFrameSet, entry.m_nFrameNo, true);
            }
            glBindTexture(GL_TEXTURE_2D, texture);
            int ix = pow2roundup(pFrame->m_nLen);
            int iy = pow2roundup(pFrame->m_nHei);
            int x1 = x;
            int x2 = x + ix;
            int y1 = sz.height() - y;
            int y2 = sz.height() - y - iy;
            glBegin(GL_QUADS);
                glTexCoord2f(0.0, 0.0); glVertex3f(x1, y2, 0.0);
                glTexCoord2f(0.0, 1.0f); glVertex3f(x1, y1, 0.0);
                glTexCoord2f(1.0f, 1.0f); glVertex3f(x2, y1, 0.0);
                glTexCoord2f(1.0f, 0.0); glVertex3f(x2, y2, 0.0);
            glEnd();
        }
    }
    glDisable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    if (m_showGrid) {
        drawGrid();
    }
}

void CLevelView::drawBackground(QPainter *painter, const QRectF & rect)
{
    Q_UNUSED(rect);
    Q_UNUSED(painter);

    // Checker rgb: 4080ff, 5090ff
    if (m_game && m_game->m_nLevels) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        glClearColor(0x50/255.0f, 0x90/255.0f, 1.0f, 1.0f);
    }

    QSize sz = getSize();

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
        if (m_game && m_game->m_nLevels) {
            if (!isGameMode()) {
                drawScreen();
                drawItemRect();
                drawSelection();
            } else {
                m_game->updateGeometry(sz.width(), sz.height());
                CGROpenGL * gr = (CGROpenGL *)m_game->graphics();
                if (gr) {
                    //gr->drawScreen();
                    //m_game->callLvEvent(CLevel::EL_DRAW);
                    m_game->updateScreen();
                }
            }
        } else {
            drawChecker();
        }
    glPopMatrix();
    glFlush();
}

void CLevelView::drawChecker()
{
    QSize sz = getSize();
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

void CLevelView::keyPressEvent ( QKeyEvent * event )
{
    keyReflector(event, true);
}

void CLevelView::keyReleaseEvent ( QKeyEvent * event )
{
    keyReflector(event, false);
}

void CLevelView::editPath()
{
    QSize sz = getSize();
    emit statusChanged(0, tr("Editing Path : SPACE to Save / ESC to Cancel"));
    ++m_ticks;
    setFocus();
    if (m_hasFocus && m_game && m_game->m_nLevels) {
        CGame & gf = *m_game;
        CLevel & level = *(gf[gf.m_nCurrLevel]);
        CLayer & layer = * level.getCurrentLayer();
        bool endPath = false;
        // if single selection
        if (layer.getSelectionSize() == 1) {
            CLevelEntry & entry = layer.getSelection(0);
            CFrameSet & filter = * gf.m_arrFrames [entry.m_nFrameSet];
            CFrame & frame = * filter [entry.m_nFrameNo];
            CPathBlock & paths = *(gf.getPaths());
            CPath & path = paths[entry.m_path];
            int scroll = PATH_SCROLL;
            // move up
            if (m_game->testKey(lgck::Key::Up) && entry.m_nY) {
                if (entry.m_nY > scroll) {
                    entry.m_nY -= scroll;
                    path.add(CGame::UP);
                } else {
                    entry.m_nY = 0;
                }
                if (entry.m_nY < level.m_my) {
                    level.m_my = entry.m_nY;
                }
                gf.setDirty(true);
            }
            // move down
            if (m_game->testKey(lgck::Key::Down)) {
                entry.m_nY += scroll;
                gf.setDirty(true);
                path.add(CGame::DOWN);
            }
            if (entry.m_nY + frame.m_nHei > level.m_my + sz.height() - scroll) {
                level.m_my += scroll;
            }

            // move left
            if (m_game->testKey(lgck::Key::Left) && entry.m_nX) {
                if (entry.m_nX > scroll) {
                    entry.m_nX -= scroll;
                    path.add(CGame::LEFT);
                } else {
                    entry.m_nX = 0;
                }
                if (entry.m_nX < level.m_mx) {
                    level.m_mx = entry.m_nX;
                }
                gf.setDirty(true);
            }

            // move right
            if (m_game->testKey(lgck::Key::Right)) {
                entry.m_nX += scroll;
                path.add(CGame::RIGHT);
                gf.setDirty(true);
            }

            if (m_game->testKey(lgck::Key::Back)
                    && path.getSize()) {
                switch (path[path.getSize()-1]) {
                case CGame::UP:
                    entry.m_nY += scroll;
                    break;
                case CGame::DOWN:
                    entry.m_nY -= scroll;
                    break;
                case CGame::LEFT:
                    entry.m_nX += scroll;
                    break;
                case CGame::RIGHT:
                    entry.m_nX -= scroll;
                }
                path.removeAt(path.getSize()-1);
                gf.setDirty(true);
            }

            if (entry.m_nX + frame.m_nLen > level.m_mx + sz.width()) {
                level.m_mx += scroll;
            }

            // de-select object
            if (m_game->testKey(lgck::Key::Space)
                    || m_game->testKey(lgck::Key::Escape)) {
                layer.clearSelection();
                emit pathEnded();
                emit menuChanged();
                endPath = true;
                emit statusChanged(0, "");
            }
            if (!endPath) {
                QString s;
                if (!layer.isMultiSelection()) {
                    s = QString(tr("Object: x=%1 y=%2")).arg(entry.m_nX).arg(entry.m_nY);
                } else {
                    s = tr("multiple selection");
                }
                emit statusChanged(1, s);
                s = QString(tr("moves: %1").arg(path.getSize()));
                emit statusChanged(2, s);
            }
        }
    }
}

void CLevelView::eventHandler()
{   
    QSize sz = getSize();
    int mx = horizontalScrollBar()->value();
    int my = verticalScrollBar()->value();
    int oldMX = mx;
    int oldMY = my;
    ++m_ticks;
    setFocus();
    if (m_hasFocus && m_game && m_game->m_nLevels) {
        CGame & gf = *m_game;
        CLevel & level = *(gf[gf.m_nCurrLevel]);
        CLayer & layer = * level.getCurrentLayer();
        // scroll the screen when the mouse is
        // within the page margin
        // return home
        if (m_game->testKey(lgck::Key::Home)) {
            layer.clearSelection();
            my = mx = 0;
        }

        if (!m_mouse.noScroll
            && m_mouse.lButton) {
            // this is conditional to the mouse
            // being inside the view
            if (m_mouse.x < MARGIN && m_mouse.x >= 0) {
                if (mx > FAST_SCROLL) {
                    mx -= FAST_SCROLL;
                } else {
                    mx = 0;
                }
            }

            if (m_mouse.x >= sz.width() - MARGIN
                && m_mouse.x < sz.width()) {
                mx += FAST_SCROLL;
            }

            if (m_mouse.y < MARGIN && m_mouse.y >= 0) {
                if (my > FAST_SCROLL) {
                    my -= FAST_SCROLL;
                } else {
                    my = 0;
                }
            }

            if (m_mouse.y >= sz.height() - MARGIN
                && m_mouse.y < sz.height()) {
                my += FAST_SCROLL;
            }
        }

        // apply screen translations to the selected
        // object

        int tX = mx - oldMX;
        int tY = my - oldMY;

        // if no object is selected
        if (layer.getSelectionSize() == 0) {
            if (m_game->testKey(lgck::Key::Up) && my) {
                if (my > FAST_SCROLL) {
                    my -= FAST_SCROLL;
                } else {
                    my = 0;
                }
            }

            if (m_game->testKey(lgck::Key::Down)) {
                my += FAST_SCROLL;
            }

            if (m_game->testKey(lgck::Key::Left) && mx) {
                if (mx > FAST_SCROLL) {
                    mx -= FAST_SCROLL;
                } else {
                    mx = 0;
                }
            }

            if (m_game->testKey(lgck::Key::Right)) {
                mx += FAST_SCROLL;
            }

            ///////////////////////////////

            QString s = QString(tr("topX=%1 topY=%2")).arg(mx).arg(my);
            emit statusChanged(1, s);

        } else {
            // manage this one object
            CLayer & layer = * level.getCurrentLayer();
            for (int z=0; z < layer.getSelectionSize(); ++z) {
                CLevelEntry & entry = layer.getSelection(z);//layer[layer.m_currEntry];
                CFrameSet & filter = * gf.m_arrFrames [entry.m_nFrameSet];
                CFrame & frame = * filter [entry.m_nFrameNo];
                if (entry.m_nY < my) {
                    if (my < FAST_SCROLL) {
                        my = 0;
                    } else {
                        my -= FAST_SCROLL;
                    }
                }

                if (entry.m_nX < mx) {
                    if (mx < FAST_SCROLL) {
                        mx = 0;
                    } else {
                        mx -= FAST_SCROLL;
                    }
                }

                // align object
                if (m_game->testKey(lgck::Key::LControl)
                        && m_game->testKey(lgck::Key::A)) {
                    if (entry.m_nX & 15 ||
                            entry.m_nY & 15) {
                        gf.setDirty(true);
                    }

                    entry.m_nX &= 0xfff0;
                    entry.m_nY &= 0xfff0;
                }

                // slowdown editor for precision
                // placement
                if (m_ticks % 2 == 0) {
                    // change frame ++
                    if (m_game->testKey(lgck::Key::RBracket) ) {
                        if (entry.m_nFrameNo < filter.getSize() -1){
                            ++entry.m_nFrameNo;
                            gf.setDirty(true);
                        }
                    }

                    // change frame --
                    if (m_game->testKey(lgck::Key::LBracket)) {
                        if (entry.m_nFrameNo >0){
                            --entry.m_nFrameNo;
                            gf.setDirty(true);
                        }
                    }

                    // precision scrolling
                    int scroll = OBJ_SCROLL;
                    if (m_game->testKey(lgck::Key::LShift)) {
                        scroll /= 2;
                    }

                    // move up
                    if (m_game->testKey(lgck::Key::Up) && entry.m_nY) {
                        if (entry.m_nY > scroll)
                            entry.m_nY -= scroll;
                        else
                            entry.m_nY = 0;

                        if (entry.m_nY < my) {
                            my = entry.m_nY;
                        }
                        gf.setDirty(true);
                    }

                    // move down
                    if (m_game->testKey(lgck::Key::Down)) {
                        entry.m_nY += scroll;
                        gf.setDirty(true);
                    }

                    if (entry.m_nY + frame.m_nHei > my + sz.height() - scroll) {
                        my += scroll;
                    }

                    // move left
                    if (m_game->testKey(lgck::Key::Left) && entry.m_nX) {
                        if (entry.m_nX > scroll)
                            entry.m_nX -= scroll;
                        else
                            entry.m_nX = 0;

                        if (entry.m_nX < mx) {
                            mx = entry.m_nX;
                        }
                        gf.setDirty(true);
                    }

                    // move right
                    if (m_game->testKey(lgck::Key::Right)) {
                        entry.m_nX += scroll;
                        gf.setDirty(true);
                    }

                    if (entry.m_nX + frame.m_nLen > mx + sz.width()) {
                        mx += scroll;
                    }
                }

                // de-select object
                if (m_game->testKey(lgck::Key::Space)
                        || m_game->testKey(lgck::Key::Escape)) {
                    layer.clearSelection();
                    emit menuChanged();
                }

                QString s;
                if (!layer.isMultiSelection()) {
                    s = QString(tr("Object: x=%1 y=%2")).arg(entry.m_nX).arg(entry.m_nY);
                } else {
                    s = tr("multiple selection");
                }
                emit statusChanged(1, s);
            }
        }

        // ensures that screen is within valid
        // worldview coordonates

        int maxMX = MAX_PIXEL - sz.width();
        int maxMY = MAX_PIXEL - sz.height();

        if (mx > maxMX) {
            mx = maxMX ;
        }

        if (my > maxMY) {
            my = maxMY ;
        }

        if (layer.getSelectionSize()
            && m_mouse.drag) {
            //TODO: revisit this code
            for (int i=0; i<layer.getSelectionSize(); ++i) {
                CLevelEntry & entry = layer.getSelection(i);
                entry.m_nX = std::max ( entry.m_nX + tX, 0);
                entry.m_nY = std::max ( entry.m_nY + tY, 0);
            }
        }

        level.m_mx = mx;
        level.m_my = my;
        if (mx != oldMX) {
            emit setMX(mx);
        }
        if (my != oldMY) {
            emit setMY(my);
        }
    }
}

void CLevelView::setGameMode(bool gm)
{
    m_gameMode = gm;
}

/////////////////////////////////////////////////////////////////////
// Drag & Drop

void CLevelView::dropEvent(QDropEvent *event)
{
    CGame & gf = *m_game;
    QString t = event->mimeData()->text();

    QPoint pos = event->pos();
    if (event->source() && gf.getSize()
            && !memcmp(q2c(t), "LGCK:", 5)) {

        int offsetX = 0x10;
        int offsetY = 0x10;
        QString s = t.mid(6);
        bool ok;
        int protoId = s.toInt( &ok, 10 );

        CProto & proto = gf.m_arrProto[protoId];
        CLevel & level = * gf[ gf.m_nCurrLevel ];
        CLevelEntry entry;
        entry.m_nProto = protoId;
        entry.m_nX = level.m_mx + (pos.x() & 0xfff8) + offsetX;
        entry.m_nY = level.m_my + (pos.y() & 0xfff8) + offsetY;
        entry.m_nFrameNo = proto.m_nFrameNo;
        entry.m_nFrameSet = proto.m_nFrameSet;

        if (proto.getOption(CProto::OPTION_NO_TRIGGER)) {
            entry.m_nActionMask = 0xf;
        } else {
            entry.m_nActionMask = 0x8f;
        }

        if (proto.getOption(CProto::OPTION_AUTO_GOAL)) {
            entry.m_nTriggerKey += 0x20;
        }

        CLayer & layer = * level.getCurrentLayer();
        int size = layer.getSize();
        layer.add(entry);
        layer.selectSingle(size);
        gf.setDirty( true );        
       // event->setDropAction(Qt::CopyAction);
        setFocus();
        emit menuChanged();
    }
}

void CLevelView::dragMoveEvent(QDragMoveEvent *event)
{
    CGame & gf = *m_game;
    // the drag event comes from this application
    // and we got a level to drop it on
    if (event->source() && gf.getSize() && !isGameMode()
            && !m_editPath) {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
    }
}

void CLevelView::dragEnterEvent(QDragEnterEvent *event)
{
    CGame & gf = *m_game;
    // the drag event comes from this application
    // and we got a level to drop it on
    if (event->source() && gf.getSize() && !isGameMode()
            && !m_editPath) {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
        event->setAccepted(true);
    } else {
         event->setAccepted(false);
    }        
}

void CLevelView::dragLeaveEvent( QDragLeaveEvent * event )
{
    Q_UNUSED(event);
}

void CLevelView::select(int x1, int y1, int x2, int y2)
{
    CGameFile & gf = *m_game;
    CLevel & level = *(gf[gf.m_nCurrLevel]);
    CLayer & layer = * level.getCurrentLayer();
    x1 += level.m_mx;
    x2 += level.m_mx;
    y1 += level.m_my;
    y2 += level.m_my;
    bool hasSelected = false;
    for (int i=0; i < layer.getSize(); ++i) {
        const CLevelEntry & entry = layer[i];
        const CFrameSet & filter = * m_game->m_arrFrames [entry.m_nFrameSet];
        const CFrame *pFrame = filter[entry.m_nFrameNo];
        if ( ((entry.m_nX >= x1 && entry.m_nX <= x2)
              || (entry.m_nX <= x1 && entry.m_nX + pFrame->m_nLen >= x1)
              || (entry.m_nX <= x2 && entry.m_nX + pFrame->m_nLen >= x2))
                &&
                ((entry.m_nY >= y1 && entry.m_nY <= y2)
                 || (entry.m_nY <= y1 && entry.m_nY + pFrame->m_nHei >= y1)
                 || (entry.m_nY <= y2 && entry.m_nY + pFrame->m_nHei >= y2))
                ) {
            layer.select(i);
            hasSelected = true;
        }
    }
    if (hasSelected) {
        emit menuChanged();
    }
}

void CLevelView::showGrid(bool show)
{
    m_showGrid = show;
}

void CLevelView::setGridSize(int size)
{
    m_gridSize = size;
}

void CLevelView::setGridColor(const QString & gridColor)
{
    m_gridColor = 0x60000000 | strtol(q2c(gridColor), NULL, 16);
}

bool CLevelView::isGameMode() const
{
    return m_gameMode;
}

void CLevelView::setGameDB(CGame * p) {
    m_game = p;
    //CIMOpengl * im = new CIMOpengl(/*m_glWidget*/);
    //m_game->attach((IImageManager*) im);
    CGROpenGL * gr = new CGROpenGL(m_game/*, im*/);
    m_game->attach((IGraphics*)gr);
    m_game->attach(gr->cache());
}

void CLevelView::focusInEvent(QFocusEvent * event)
{
    QWidget::focusInEvent(event);
    m_hasFocus = true;
    emit focused(true);
}

void CLevelView::focusOutEvent( QFocusEvent * event )
{
    QWidget::focusOutEvent(event);
    m_game->clearKeys();
    m_hasFocus = false;
    emit focused(false);
}

void CLevelView::enterEditPath()
{
    m_editPath = true;
}

void CLevelView::leaveEditPath()
{
    m_editPath = false;
}

void CLevelView::updateScrollbars()
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

void CLevelView::resizeEvent(QResizeEvent * event )
{
    QSize size = event->size();
    emit viewSizeChanged(size.width(), size.height());
    updateScrollbars();
}

void CLevelView::changeLevel(int)
{
    /*
    qDebug("vendor: %s", glGetString(GL_VENDOR));
    qDebug("renderer: %s", glGetString(GL_RENDERER));
    qDebug("version: %s", glGetString(GL_VERSION));
    qDebug("extensions: %s", glGetString(GL_EXTENSIONS));
    */
    CLevel * level = NULL;
    if (m_game && m_game->m_nLevels) {
        level = (*m_game)[m_game->m_nCurrLevel];
    }
    m_scene->clear();
    m_scene->setSceneRect(0, 0, level ? MAX_PIXEL : 1, level ? MAX_PIXEL : 1);

    // add dummy item to allow screen to be refreshed
    QGraphicsItem *item = new CPixel(NULL, NULL,0,0);
    item->setPos(QPoint(0,0));
    m_scene->addItem(item);
    updateScrollbars();
    sceneUpdated();
    if (level) {
        emit setMX(level->m_mx);
        emit setMY(level->m_my);
    }
}

void CLevelView::sceneUpdated()
{
    m_scene->update();
}

void CLevelView::showScrollbars(bool show)
{
    m_scene->clear();
    m_scene->setSceneRect(0, 0, show ? MAX_PIXEL : 1, show ? MAX_PIXEL : 1);
    // add dummy item to allow screen to be refreshed
    QGraphicsItem *item = new CPixel(NULL, NULL,0,0);
    item->setPos(QPoint(0,0));
    m_scene->addItem(item);
    updateScrollbars();
    sceneUpdated();
}

void CLevelView::makeCurrent()
{
    m_glWidget->makeCurrent();
}
