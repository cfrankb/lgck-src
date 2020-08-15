/*
    LGCK Builder Runtime
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

#include "../shared/stdafx.h"
#include "im_opengl.h"
#include "../shared/Frame.h"
#include "../shared/FrameSet.h"

#ifdef LGCK_QT
    #include <QGLWidget>
#endif
#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #ifdef _WIN32
      #include <windows.h>
    #endif
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include "im_opengl.h"
#include "gr_opengl.h"
#include "DisplayManager.h"
#include "Game.h"
#include "Proto.h"
#include "Scene.h"
#include "Layer.h"
#include "Inventory.h"
#include "Level.h"
#include "Font.h"
#include "Display.h"
#include "ISurface.h"
#include "implementers/opengl/glhelper.h"

CGROpenGL::CGROpenGL(CGame *game)
{
    m_imageManager = new CIMOpengl();
    m_game = game;
    m_displayManager = new CDisplayManager(game, m_imageManager, this);
#ifdef LGCK_QT
    initializeOpenGLFunctions();
#endif
}

CGROpenGL::~CGROpenGL()
{
    delete m_displayManager;
}

const char* CGROpenGL::signature()
{
    return "graphik-opengl";
}

void CGROpenGL::getScreenSize(int & len, int & hei)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    len = viewport[2];
    hei = viewport[3];
}

void CGROpenGL::clear(unsigned int red, unsigned int green, unsigned int blue)
{
    glDisable(GL_TEXTURE_2D);
    glClearColor(red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CGROpenGL::clear(unsigned int rgb)
{
    clear(rgb & 0xff, (rgb >> 8) & 0xff, (rgb >> 16) & 0xff);
}

void CGROpenGL::paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill)
{
    glDisable(GL_TEXTURE_2D);
    unsigned int red = rgba & 0xff;
    unsigned int green = (rgba >> 8) & 0xff;
    unsigned int blue = (rgba >> 16) & 0xff;
    unsigned int alpha = (rgba >> 24) & 0xff;
    if (fill) {
        glColor4f(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f);
        glRectf(x1, y1, x2, y2);
    } else {
        glBegin(GL_LINE_LOOP);
            glColor4f(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f);
            glVertex2f(x1, y1);
            glVertex2f(x2, y1);
            glVertex2f(x2, y2);
            glVertex2f(x1, y2);
        glEnd();
    }
}

void CGROpenGL::paintImage(int x1, int y1, int frameSet, int frameNo)
{
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_BLEND);
    glDisable(GL_MULTISAMPLE);
    glColor4f(m_colorMod.red / 255.0f, m_colorMod.green / 255.0f, m_colorMod.blue / 255.0f, 1.0f);
    unsigned int texture = m_imageManager->getImage(frameSet, frameNo);
    CFrame & frame = m_game->toFrame(frameSet, frameNo);
    glBindTexture(GL_TEXTURE_2D, texture);
    int ix = pow2roundup(frame.m_nLen);
    int iy = pow2roundup(frame.m_nHei);
    int x2 = x1 + ix;
    int y2 = y1 - iy;
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(x1, y2, 0.0);
        glTexCoord2f(0.0, 1.0f); glVertex3f(x1, y1, 0.0);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x2, y1, 0.0);
        glTexCoord2f(1.0f, 0.0); glVertex3f(x2, y2, 0.0);
    glEnd();
}

IDisplayManager* CGROpenGL::displayManager()
{
    return (IDisplayManager*) m_displayManager;
}

void CGROpenGL::ss_paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    paint(x1, screenHei - y1, x2, screenHei - y2, rgba, fill);
}

void CGROpenGL::ss_paintImage(int x1, int y1, int frameSet, int frameNo)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    paintImage(x1,screenHei - y1, frameSet, frameNo);
}

IImageManager *CGROpenGL::CGROpenGL::cache()
{
    return (IImageManager *) m_imageManager;
}

void CGROpenGL::render(CFont &font, const char *text, int x, int y, const Color &color)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    unsigned int texture = font.textureId();
    ASSERT(texture);

    glEnable(GL_TEXTURE_2D); GLDEBUG();
    glEnable (GL_BLEND); GLDEBUG();
    glBindTexture(GL_TEXTURE_2D, texture); GLDEBUG();
    float xx = x;
    for (int i=0; text[i]; ++i) {
        char id = text[i];
        CFont::Glyph & glyph = font[id];
        if (id == ' ' || !glyph.width) {
            xx += glyph.width * font.scale();
            continue;
        }
        int x1 = xx;
        int y1 = screenHei - y;
        int x2 = x1 + (font.scale() * (float)glyph.width);
        int y2 = y1 - (font.scale() * (float)font.scaleY());

        float w = pow2roundup(font.width());
        float h = pow2roundup(font.height());

        float a1 = ((float)glyph.x) / w;
        float b1 = 1.0-((float)glyph.y) / h;
        float a2 = (((float)glyph.x + glyph.width))/ w;
        float b2 = 1.0-((((float)glyph.y + font.scaleY())) / h);

        glBegin(GL_QUADS);
            glColor4f(color.red / 255.0f, color.green / 255.0f, color.blue / 255.0f, color.alpha / 255.0f); GLDEBUG();
            glTexCoord2f(a1, b2); glVertex3f(x1, y2, 0.0);
            glTexCoord2f(a1, b1); glVertex3f(x1, y1, 0.0);
            glTexCoord2f(a2, b1); glVertex3f(x2, y1, 0.0);
            glTexCoord2f(a2, b2); glVertex3f(x2, y2, 0.0);
        glEnd();

        xx += glyph.width * font.scale();
    }
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}
