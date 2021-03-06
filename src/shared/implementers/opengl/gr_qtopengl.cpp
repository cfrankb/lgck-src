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
#include "../shared/Frame.h"
#include "../shared/FrameSet.h"

#include "im_qtopengl.h"
#include "gr_qtopengl.h"
#include "DisplayManager.h"
#include "Game.h"
#include "Proto.h"
#include "Scene.h"
#include "Layer.h"
#include "Inventory.h"
#include "Level.h"
#include "Font.h"

#include "implementers/opengl/glhelper.h"

// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
// http://www.opengl.org/archives/resources/features/fontsurvey/
// http://students.cs.byu.edu/~bfish/glfontdl.php
// http://stackoverflow.com/questions/8847899/opengl-how-to-draw-text-using-only-opengl-methods

CGROpenGL::CGROpenGL(CGame *game)
{
    m_imageManager = new CIMOpengl();
    m_game = game;
    m_displayManager = new CDisplayManager(game, m_imageManager, this);
    initializeOpenGLFunctions();
}

CGROpenGL::~CGROpenGL()
{
    delete m_displayManager;
}

const char* CGROpenGL::signature()
{
    return "graphik-qtopengl";
}

void CGROpenGL::getScreenSize(int & len, int & hei)
{
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    len = viewport[2];
    hei = viewport[3];
}

void CGROpenGL::drawLayer(CLayer * layer, int mx, int my)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    int offsetX = 0;
    int offsetY = 0;
    int maxX  = m_game->BUFFERLEN;
    if (maxX < screenLen)  {
        offsetX = (screenLen - maxX) / 2;
    }
    int maxY  = m_game->BUFFERHEI;
    if (maxY < screenHei)  {
        offsetY = (screenHei - maxY) / 2;
    }
    CFrame *pFrame;
    CLayer & l = (*layer);
    int entries = l.getSize();
    for (int i=0; i< entries; ++i) {
        CLevelEntry & entry = l[i] ;
        if ((entry.m_nProto & m_game->PROTO_POINTS) == m_game->PROTO_POINTS ) {
            pFrame = (* m_game->m_points ) [entry.m_nFrameNo];
        } else {
            pFrame = (* m_game->m_arrFrames[entry.m_nFrameSet])[entry.m_nFrameNo];
        }
        int x = entry.m_nX - mx;
        int y = entry.m_nY - my;
        if (!((entry.m_nTriggerKey & m_game->TRIGGER_HIDDEN) ||
            (x + pFrame->m_nLen <= 0) ||
            (x >= screenLen) ||
            (y + pFrame->m_nHei <= 0) ||
            (y >= screenHei) ||
            (entry.m_nFrameNo & 0x8000))) {
            x += offsetX;
            y += offsetY;
            paintImage(x, screenHei - y, entry.m_nFrameSet, entry.m_nFrameNo);
        }
    }
}

void CGROpenGL::drawScene(CScene * layer)
{
    int mx = m_game->m_mx;
    int my = m_game->m_my;
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);

    int offsetX = 0;
    int offsetY = 0;
    int maxX  = m_game->BUFFERLEN;
    if (maxX < screenLen)  {
        offsetX = (screenLen - maxX) / 2;
    }

    int maxY  = m_game->BUFFERHEI;
    if (maxY < screenHei)  {
        offsetY = (screenHei - maxY) / 2;
    }

    CScene & scene = *layer;
    int entries = scene.getSize();
    CFrame *pFrame;
    int frameSet;
    for (int i=0; i< entries; ++i) {
        CActor & entry = scene[i] ;
        if ((entry.m_nProto & m_game->PROTO_POINTS) == m_game->PROTO_POINTS ) {
            pFrame = (* m_game->m_points ) [entry.m_nFrameNo];
            frameSet = m_game->var("pointsOBL");
        } else {
            pFrame = (* m_game->m_arrFrames[entry.m_nFrameSet])[entry.m_nFrameNo];
            frameSet = entry.m_nFrameSet;
        }
        int x = entry.m_nX - mx;
        int y = entry.m_nY - my;
        if (!((entry.m_nTriggerKey & m_game->TRIGGER_HIDDEN) ||
            (x + pFrame->m_nLen <= 0) ||
            (x >= screenLen) ||
            (y + pFrame->m_nHei <= 0) ||
            (y >= screenHei) ||
            (entry.m_nFrameNo & 0x8000))) {
            x += offsetX;
            y += offsetY;
            paintImage(x, screenHei - y, pFrame, frameSet, entry.m_nFrameNo);
        }
    }
}

void CGROpenGL::drawHP()
{
#ifdef QT_OPENGL_ES_2
#else
    glDisable(GL_TEXTURE_2D);
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    CActor & player = m_game->getPlayer();
    int x = 1;
    int y = screenHei - 24;
    UINT8 a = m_game->getDisplayAlpha();
    if (a) {
        paint(x,screenHei - y, x + player.getHP() * 2, screenHei - y - 16, 0x40ff00 + (a << 24));
        paint(x,screenHei - y, x + player.getHP() * 2, screenHei - y - 16, 0xffffff + (a << 24), false);
    }
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
#endif
}

void CGROpenGL::getOffset(int & offsetX, int & offsetY)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    offsetX = 0;
    offsetY = 0;
    int maxX  = m_game->BUFFERLEN;
    if (maxX < screenLen)  {
        offsetX = (screenLen - maxX) / 2;
    }

    int maxY  = m_game->BUFFERHEI;
    if (maxY < screenHei)  {
        offsetY = (screenHei - maxY) / 2;
    }
}

void CGROpenGL::drawScreen()
{
#ifdef QT_OPENGL_ES_2
#else

    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    int offsetX;
    int offsetY;
    getOffset(offsetX, offsetY);    
//    glDisable(GL_TEXTURE_2D);
    clear(m_game->var("borderColor"));
    paint(offsetX,
          screenHei - offsetY,
          screenLen - offsetX,
          offsetY,
          m_game->var("bkColor") | ALL_ALPHA);
    glEnable(GL_TEXTURE_2D);
    //glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int colorMod = m_game->var("colorMod") | ALL_ALPHA;
    float blue = (colorMod & 0xff);// << 16;
    float green = (colorMod & 0xff00) >> 8;
    float red = (colorMod >> 16) & 0xff;
    glColor4f(red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);

    for (int i = 0; i < m_game->m_layers->getSize(); ++i) {
        int speeds[] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
        CLayer & layer = (*(m_game->m_layers))[i];
        if (layer.getType() != CLayer::LAYER_MAIN) {
            // draw background and foreground layers
            int mx = 0, my = 0;
            int h, v;                      
            layer.getSpeed(h, v);
            if (h == CLayer::SPEED_CUSTOM) {
                layer.getOffset(mx, my);
            } else {
                if (h) {
                    mx = m_game->m_mx / speeds[h];
                }
                if (v) {
                    my = m_game->m_my / speeds[v];
                }
            }
            drawLayer(&layer, mx, my);
        } else {
            // draw main layer
            drawScene(m_game->m_sBK);
            drawScene(m_game->m_sFW);
        }
    }
//    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
//    glDisable(GL_TEXTURE_2D);
    drawHP();
//    glEnable(GL_TEXTURE_2D);
    drawInventory();
    m_displayManager->draw();
    glDisable(GL_TEXTURE_2D);
#endif
}

void CGROpenGL::drawInventory()
{
#ifdef QT_OPENGL_ES_2
#else

    glEnable(GL_TEXTURE_2D);
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    const CInventory *inventory = m_game->getInventory();
    int i = 0;
    for (int j=0; inventory && (j < inventory->getSize()); ++j) {
        if ((*inventory)[j] != 0) {
            CProto proto = m_game->m_arrProto[(*inventory)[j]];
            if (!proto.getOption(CProto::OPTION_INVENTORY_HIDDEN)) {
                int imageSet = proto.m_nFrameSet;
                int imageNo = proto.m_nFrameNo;
                CFrame *frame = (*( m_game->m_arrFrames[imageSet]))[imageNo];
                int x = screenLen - frame->m_nLen - 4;
                int y = 32 * (i + 1) + 4;
                paintImage(x, screenHei - y, imageSet, imageNo);
                ++i;
            }
        }
    }
#endif
}

void CGROpenGL::clear(unsigned int red, unsigned int green, unsigned int blue)
{
#ifdef QT_OPENGL_ES_2
#else

    glDisable(GL_TEXTURE_2D);
    glClearColor(red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
}

void CGROpenGL::clear(unsigned int rgb)
{
    clear(rgb & 0xff, (rgb >> 8) & 0xff, (rgb >> 16) & 0xff);
}

void CGROpenGL::paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill)
{
#ifdef QT_OPENGL_ES_2
#else
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
#endif
}

void CGROpenGL::paintImage(int x1, int y1, int frameSet, int frameNo)
{
#ifdef QT_OPENGL_ES_2
#else

    unsigned int texture = m_imageManager->getImage(frameSet, frameNo);
    CFrame *frame = (*( m_game->m_arrFrames[frameSet]))[frameNo];
    glBindTexture(GL_TEXTURE_2D, texture);
    int ix = pow2roundup(frame->m_nLen);
    int iy = pow2roundup(frame->m_nHei);
    int x2 = x1 + ix;
    int y2 = y1 - iy;
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(x1, y2, 0.0);
        glTexCoord2f(0.0, 1.0f); glVertex3f(x1, y1, 0.0);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x2, y1, 0.0);
        glTexCoord2f(1.0f, 0.0); glVertex3f(x2, y2, 0.0);
    glEnd();
#endif
}

void CGROpenGL::paintImage(int x1, int y1, CFrame *frame, int frameSet, int frameNo)
{
#ifdef QT_OPENGL_ES_2
#else

    unsigned int texture = m_imageManager->getImage(frameSet, frameNo);
    glBindTexture(GL_TEXTURE_2D, texture);
    int ix = pow2roundup(frame->m_nLen);
    int iy = pow2roundup(frame->m_nHei);
    int x2 = x1 + ix;
    int y2 = y1 - iy;
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(x1, y2, 0.0);
        glTexCoord2f(0.0, 1.0f); glVertex3f(x1, y1, 0.0);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x2, y1, 0.0);
        glTexCoord2f(1.0f, 0.0); glVertex3f(x2, y2, 0.0);
    glEnd();
#endif
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

    //GLDEBUG();
    glEnable(GL_TEXTURE_2D); GLDEBUG();
    glEnable (GL_BLEND); GLDEBUG();
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
