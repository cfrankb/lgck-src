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
    uint32_t red = rgba & 0xff;
    uint32_t green = (rgba >> 8) & 0xff;
    uint32_t blue = (rgba >> 16) & 0xff;
    uint32_t alpha = (rgba >> 24) & 0xff;
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
    unsigned int id_texture = m_imageManager->getImage(frameSet, frameNo);
    sf::Sprite sprite;
    sprite.setTexture(*m_imageManager->texture(id_texture));
    sprite.setOrigin(0,0);
    sprite.setColor(sf::Color(
                        255,//m_colorMod.red,
                        255,//m_colorMod.green,
                        255,//m_colorMod.blue,
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
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    paint(x1, screenHei - y1, x2, screenHei - y2, rgba, fill);
}

void CGRSfml::ss_paintImage(int x1, int y1, int frameSet, int frameNo)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    paintImage(x1,screenHei - y1, frameSet, frameNo);
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
