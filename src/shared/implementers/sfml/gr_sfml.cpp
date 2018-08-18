/*
    LGCK Builder Runtime
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

#include "../shared/stdafx.h"
#include "../shared/Frame.h"
#include "../shared/FrameSet.h"
#include "im_sfml.h"
#include "gr_sfml.h"
#include "DisplayManager.h"
#include "Game.h"
#include "Proto.h"
#include "Scene.h"
#include "Layer.h"
#include "Inventory.h"
#include "Level.h"
#include "Font.h"

// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
// http://www.opengl.org/archives/resources/features/fontsurvey/
// http://students.cs.byu.edu/~bfish/glfontdl.php
// http://stackoverflow.com/questions/8847899/opengl-how-to-draw-text-using-only-opengl-methods

CGRSfml::CGRSfml()
{
    m_game = NULL;
}

bool CGRSfml::init(CGame *game, int w, int h, const char *title)
{
    m_screenWidth = w;
    m_screenHeight = h;
    m_window = new sf::RenderWindow(sf::VideoMode(w, h), title);
    if (!m_window) {
        qDebug("sf::RenderWindow failed");
        return false;
    }
    m_imageManager = new CIMSfml();
    m_game = game;
    m_displayManager = new CDisplayManager(game, m_imageManager, this);
    game->updateGeometry(w, h);
    return true;
}

const char* CGRSfml::lastError()
{
    return m_lastError.c_str();
}

CGRSfml::~CGRSfml()
{
    delete m_displayManager;
}

const char* CGRSfml::signature()
{
    return "graphik-sfml2";
}

void CGRSfml::getScreenSize(int & len, int & hei)
{
    len = m_screenWidth;
    hei = m_screenHeight;
}

void CGRSfml::drawLayer(CLayer * layer, int mx, int my)
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

void CGRSfml::drawScene(CScene * layer)
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

void CGRSfml::getOffset(int & offsetX, int & offsetY)
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

void CGRSfml::drawScreen()
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    int offsetX;
    int offsetY;
    getOffset(offsetX, offsetY);    
    clear(m_game->var("borderColor"));
    paint(offsetX,
          offsetY,
          screenLen - offsetX,
          screenHei - offsetY,
          m_game->var("bkColor") | 0xff000000);
    int colorMod = m_game->var("colorMod") | 0xff000000;
    m_colorMod.blue = (colorMod & 0xff);// << 16;
    m_colorMod.green = (colorMod & 0xff00) >> 8;
    m_colorMod.red = (colorMod >> 16) & 0xff;

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
    m_displayManager->draw();
}

void CGRSfml::clear(unsigned int red, unsigned int green, unsigned int blue)
{
    // Set Color
    m_window->clear(sf::Color(red, green, blue,  0xff));
}

void CGRSfml::clear(unsigned int rgb)
{
    clear(rgb & 0xff, (rgb >> 8) & 0xff, (rgb >> 16) & 0xff);
}

void CGRSfml::paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    unsigned int red = rgba & 0xff;
    unsigned int green = (rgba >> 8) & 0xff;
    unsigned int blue = (rgba >> 16) & 0xff;
    unsigned int alpha = (rgba >> 24) & 0xff;
    int w = x2 - x1;
    int h = y2 - y1;
    sf::RectangleShape rect(sf::Vector2f(w, h));
    if (fill) {
        rect.setFillColor(sf::Color(red, green, blue, alpha));
    } else {
        rect.setOutlineThickness(1);
        rect.setOutlineColor(sf::Color(red, green, blue, alpha));
        rect.setFillColor(sf::Color(red, green, blue, 0));
    }
    rect.setOrigin(0,0);
    rect.setPosition(x1,y1);
    m_window->draw(rect);
}

void CGRSfml::paintImage(int x1, int y1, int frameSet, int frameNo)
{
    CFrame *frame = (*( m_game->m_arrFrames[frameSet]))[frameNo];
    paintImage(x1,y1, frame, frameSet, frameNo);
}

void CGRSfml::paintImage(int x1, int y1, CFrame *frame, int frameSet, int frameNo)
{
    unsigned int id_texture = m_imageManager->getImage(frameSet, frameNo);
    sf::Sprite sprite;
    sprite.setTexture(*m_imageManager->texture(id_texture));
    sprite.setOrigin(0,0);
    sprite.setColor(sf::Color(
                        m_colorMod.red,
                        m_colorMod.green,
                        m_colorMod.blue,
                        0xff));
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    sprite.setPosition(x1,screenHei-y1);
    m_window->draw(sprite);
}

IDisplayManager* CGRSfml::displayManager()
{
    return (IDisplayManager*) m_displayManager;
}

void CGRSfml::ss_paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill)
{
    paint(x1, y1, x2, y2, rgba, fill);
}

void CGRSfml::ss_paintImage(int x1, int y1, int frameSet, int frameNo)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    paintImage(x1, y1, frameSet, frameNo);
}

IImageManager *CGRSfml::CGRSfml::cache()
{
    return (IImageManager *) m_imageManager;
}

void CGRSfml::update()
{
    m_window->display();
}

sf::RenderWindow *CGRSfml::window()
{
    return m_window;
}

void CGRSfml::render(CFont & font, const char *text, int x, int y, const Color & color)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    sf::Sprite sprite;
    sf::Texture *texture = m_imageManager->texture(font.textureId());
    ASSERT(texture);
    sprite.setTexture(*texture);
    float xx = x;
    for (int i=0; text[i]; ++i) {
        char id = text[i];
        CFont::Glyph & glyph = font[id];
        if (id == ' ') {
            xx += glyph.width * font.scale();
            continue;
        }
        sprite.setTextureRect(sf::IntRect(
                                  glyph.x, glyph.y,
                                  glyph.width,
                                  font.scaleY()));
                                  //glyph.height+4));
        sprite.setOrigin(0,0);
        sprite.setColor(sf::Color(
                            color.red,
                            color.green,
                            color.blue,
                            color.alpha));
        sprite.setPosition(xx,y);
        sprite.setScale(font.scale(), font.scale());
        window()->draw(sprite);
        xx += glyph.width * font.scale();
    }
}
