/*
    LGCK Builder GUI
    Copyright (C) 1999, 2016  Francois Blanchette

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

#include <algorithm>
#include <QKeyEvent>
#include <QMimeData>
#include "stdafx.h"
#include "levelviewgl.h"
#include "Game.h"
#include "Level.h"
#include "Layer.h"
#include "FrameSet.h"
#include "Frame.h"
#include "Const.h"
#include "Path.h"
#include "PathBlock.h"
#include "qtgui/cheat.h"
#include "implementers/opengl/gr_opengl.h"
#include "implementers/opengl/im_opengl.h"
#include "inputs/qt/kt_qt.h"
#include <QDebug>
#include "fontmanager.h"
#include "Font.h"

// doc.qt.io/qt-5/qopenglwidget.html#paintGL
// http://guillaume.belz.free.fr/doku.php?id=opengl_dans_qt5
// http://doc.qt.io/qt-5/qopenglwindow.html
// https://blog.qt.io/blog/2014/11/27/qt-weekly-21-dynamic-opengl-implementation-loading-in-qt-5-4/

CLevelViewGL::CLevelViewGL(QWidget* parent, CGame *game):
    QOpenGLWidget(parent)
{
    setUpdateBehavior(QOpenGLWidget::PartialUpdate);
    m_gridSize = 32;
    m_showGrid = true;
    m_gridColor = 0x60c0b0a0;
    m_hasFocus = false;
    m_game = game;
    m_triggerFontSize = 16;
    m_mouseEvent.x = m_mouseEvent.y = m_mouseEvent.buttons = -1;
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(this, SIGNAL(versionCheck()),
            parent->parent(), SLOT(checkVersion()));
}

CLevelViewGL::~CLevelViewGL()
{
    m_timer.stop();
    if (m_game) {
        makeCurrent();
        m_game->attach(static_cast<IGraphics*>(nullptr));
        delete m_game->graphics();
        m_game->attach(static_cast<IImageManager*>(nullptr));
        delete m_game->cache();
    }
}

void CLevelViewGL::initializeGL()
{
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();
    if (m_game) {
        CGROpenGL * gr = new CGROpenGL(m_game);
        m_game->attach(static_cast<IGraphics*>(gr));
        m_game->attach(gr->cache());
    } else {
        qFatal("Game object not set. Implementors not attached");
    }
    // set refresh rate
    m_timer.setInterval(1000 / TICK_MAX_RATE);
    m_timer.start();
    qDebug("vendor: %s", glGetString(GL_VENDOR));
    qDebug("renderer: %s", glGetString(GL_RENDERER));
    qDebug("version: %s", glGetString(GL_VERSION));
    qDebug("extensions: %s", glGetString(GL_EXTENSIONS));
    emit versionCheck();
}

void CLevelViewGL::paintGL()
{
    static bool drawing = false;
    if (!drawing) {
        drawing = true;
        QOpenGLWidget::paintGL();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        drawBackground();
        drawing = false;
    }
    if (m_game && isGameMode()) {
        m_game->triggerMouseEvent(m_mouseEvent.x, m_mouseEvent.y, m_mouseEvent.buttons);
    }
    m_mouseEvent.x = m_mouseEvent.y = m_mouseEvent.buttons = -1;
}

void CLevelViewGL::mouseClick(int x, int y, Qt::MouseButtons buttons)
{
    m_mouseEvent.x = x;
    m_mouseEvent.y = y;
    m_mouseEvent.buttons = buttons;
}

void CLevelViewGL::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void CLevelViewGL::drawSelection()
{
    QSize sz = size();
    int orgX =-1;
    int orgY =-1;
    int destX = -1;
    int destY = -1;
    emit whereIsMouse(orgX, orgY, destX, destY);

    if (orgX != -1 && orgY != -1) {
        int x1 = std::min(orgX, destX);
        int x2 = std::max(orgX, destX);
        int y1 = sz.height() - std::min(orgY, destY);
        int y2 = sz.height() - std::max(orgY, destY);
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

void CLevelViewGL::drawCheckers()
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

void CLevelViewGL::drawBackground()
{
    // Checker rgb: 4080ff, 5090ff
    if (m_game && m_game->getSize()) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        glClearColor(0x50/255.0f, 0x90/255.0f, 1.0f, 1.0f);
    }
    QSize sz = size();
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
        if (m_game && m_game->getSize()) {
            if (!isGameMode()) {
                drawScreen();
                drawItemRect();
                drawSelection();
            } else {
                m_game->updateGeometry(sz.width(), sz.height());
                CGROpenGL * gr = (CGROpenGL *)m_game->graphics();
                if (gr) {
                    m_game->updateScreen();
                }
            }
        } else {
            drawCheckers();
        }
    glPopMatrix();
    glFlush();
    glFinish();
}

bool CLevelViewGL::isGameMode()
{
    bool gameMode = false;
    emit gameModeStateResync(gameMode);
    return gameMode;
}

void CLevelViewGL::uint2color(uint32_t rgba, Color & out)
{
    uint8_t blue = rgba & 0xff;
    uint8_t green = (rgba & 0xff00) >> 8;
    uint8_t red = (rgba >> 16) & 0xff;
    uint8_t alpha = rgba >> 24;
    out = {
        red, green, blue, alpha
    };
}

void CLevelViewGL::uint2rgba(uint32_t rgba, float &red, float &green, float &blue, float &alpha)
{
    Color t;
    uint2color(rgba, t);
    blue = t.blue / 255.0f;
    green = t.green / 255.0f;
    red = t.red / 255.0f;
    alpha = t.alpha / 255.0f;
}

void CLevelViewGL::drawScreen()
{
    CLevel & level = m_game->getCurrentLevel();
    int mx = 0;
    int my = 0;
    emit scrollStatusResync(mx, my);

    QSize sz = size();
    int w = std::min(sz.width(), (int) (MAX_PIXEL - mx));
    int h = std::min(sz.height(), (int)(MAX_PIXEL - my));

    float red, green, blue, alpha;
    // draw background
    if (w > 0 && h > 0) {
        int x1 = 0;
        int x2 = w;
        int y1 = sz.height() - h;
        int y2 = sz.height();
        // Color: #RRGGBB
        uint32_t bkColor = 0xff000000 | strtoul(level.getSetting("bkcolor"), nullptr, 16);
        uint2rgba(bkColor, red, green, blue, alpha);
        glColor4f(red, green, blue, alpha);
        glRectf(x1, y1, x2, y2);
    }

    CIMOpengl * im = (CIMOpengl *)m_game->cache();
    if (!im) {
        qWarning("image cache not set.");
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CLayer & layer = * level.getCurrentLayer();
    uint32_t colorMod = 0xff000000 | strtoul(level.getSetting("colorMod"), nullptr, 16);
    uint2rgba(colorMod, red, green, blue, alpha);
    glColor4f(red, green, blue, alpha);

    CFont *font = m_game->getFonts()->at(0);
    if (!font) {
        qWarning("Font[0] is not set.");
    }
    font->FaceSize(m_triggerFontSize);
    Color fontColor;
    uint2color(m_triggerKeyColor, fontColor);
    Color shadowColor = { 0, 0, 0, 255};

    // draw sprites
    for (int i=0; i < layer.getSize(); ++i) {
        CLevelEntry & entry = layer[i] ;
        CFrameSet & filter = m_game->toFrameSet(entry.m_nFrameSet);
        CFrame & frame = m_game->toFrame(entry);
        int x = entry.m_nX - mx;
        int y = entry.m_nY - my;
        if (!( (x + frame.m_nLen <= 0) ||
            (x >= w) ||
            (y + frame.m_nHei <= 0) ||
            (y >= h)) && (entry.m_nActionMask & m_skillFilters)) {
            unsigned int texture = im->getImage(entry.m_nFrameSet, entry.m_nFrameNo);
            if (entry.m_nTriggerKey & TRIGGER_HIDDEN){
                if (!im->hasInverse( entry.m_nFrameSet )) {
                    im->cacheInverse( entry.m_nFrameSet, & filter);
                }
                texture = im->getImage(entry.m_nFrameSet, entry.m_nFrameNo, true);
            }
            glBindTexture(GL_TEXTURE_2D, texture);
            int ix = pow2roundup(frame.m_nLen);
            int iy = pow2roundup(frame.m_nHei);
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
            // draw trigger keys
            if (m_showTriggerKey && entry.m_nTriggerKey & TRIGGER_KEYS) {
                char key[3] = {0,0,0};
                sprintf(key, "%.2d", entry.m_nTriggerKey & TRIGGER_KEYS);
                m_game->graphics()->render(*font, key, x + 2, y + 2, shadowColor);
                m_game->graphics()->render(*font, key, x, y, fontColor);
                glColor4f(red, green, blue, alpha);
            }
        }
    }
    glDisable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    if (m_showGrid) {
        drawGrid();
    }
}

void CLevelViewGL::drawGrid()
{
    QSize sz = size();
    int mx = 0;
    int my = 0;
    emit scrollStatusResync(mx, my);
    int w = std::min(sz.width(), (int) (MAX_PIXEL - mx));
    int h = std::min(sz.height(), (int) (MAX_PIXEL - my));

    glLineWidth(0.5f);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float red, green, blue, alpha;
    uint2rgba(m_gridColor, red, green, blue, alpha);
    glColor4f(red, green, blue, alpha);

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

void CLevelViewGL::drawItemRect()
{
    QSize sz = size();
    if (m_game && m_game->getSize()) {
        CGame & gf = *m_game;
        CLevel & level = m_game->getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        float red = (rand() & 255) / 255.0f;
        float green = (rand() & 255) / 255.0f;
        float blue = (rand() & 255) / 255.0f;
        glLineWidth(0.5f);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (int i=0; i < layer.getSelectionSize(); ++i) {
            CLevelEntry & entry = layer.getSelection(i);
            CFrame & frame = gf.toFrame(entry);
            int x = entry.m_nX - level.m_mx;
            int y = entry.m_nY - level.m_my;
            if ((x >= 0) &&
                (y >= 0) &&
                (x < sz.width()) &&
                (y < sz.height())) {
                int x1 = x;
                int x2 = x + frame.m_nLen;
                int y1 = sz.height() - y;
                int y2 = sz.height() - y -frame.m_nHei;
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

void CLevelViewGL::showGrid(bool show)
{
    m_showGrid = show;
}

void CLevelViewGL::setGridSize(int size)
{
    m_gridSize = size;
}

void CLevelViewGL::setGridColor(const QString & gridColor)
{
    m_gridColor = 0x60000000 | strtoul(q2c(gridColor), nullptr, 16);
}

void CLevelViewGL::setTriggerKeyColor(const QString & color)
{
    m_triggerKeyColor = 0xff000000 | strtoul(q2c(color), nullptr, 16);
}

void CLevelViewGL::showTriggerKey(bool state)
{
    m_showTriggerKey = state;
}

void CLevelViewGL::setTriggerFontSize(int size)
{
    m_triggerFontSize = size;
}

void CLevelViewGL::setSkillFilters(int flags)
{
    m_skillFilters = flags;
}
