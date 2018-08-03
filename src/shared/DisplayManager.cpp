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

#include "stdafx.h"
#include "DisplayManager.h"
#include "Game.h"
#include "Frame.h"
#include "interfaces/IGraphics.h"
#include "interfaces/IImageManager.h"
#include "FileWrap.h"
#include "Font.h"

// http://www.codehead.co.uk/cbfg/
// http://doc.qt.io/qt-5/qtwidgets-widgets-charactermap-example.html
// http://devcry.heiho.net/2012/01/opengl-font-rendering-with-ftgl.html
// https://www.google.ca/search?client=opera&q=opengl+text+rendering+library&sourceid=opera&ie=utf-8&oe=utf-8&channel=suggest
// https://www.google.ca/search?q=ftgl&btnG=Search&safe=off&client=opera&channel=suggest
// http://devcry.heiho.net/2012/01/ftgl-font-rendering-in-multiple-colors.html

/////////////////////////////////////////////////////////////////////
// CDisplayManager

CDisplayManager::CDisplayManager(CGame *game, IImageManager *im, IGraphics *gm)
{
    m_game = game;
    m_imageManager = im;
    m_graphics = gm;

    m_displays = new CDisplay[ GROWBY ];
    m_size = 0;
    m_max = GROWBY;
}

CDisplayManager::~CDisplayManager()
{
    if (m_displays) {
        delete [] m_displays;
    }
}

CDisplay & CDisplayManager::add(CDisplay & display)
{
    if (m_size == m_max) {
        m_max += GROWBY;
        CDisplay *t = new CDisplay [ m_max ];
        for (int i=0; i < m_size; ++i) {
            t[i] = m_displays[i];
        }

        delete [] m_displays;
        m_displays = t;
    }

    m_displays[m_size] = display;
    ++ m_size;
    return m_displays[m_size - 1];
}

CDisplay & CDisplayManager::add(const char *name, int x, int y, int type)
{
    CDisplay display (name, x, y, type);
    return add(display);
}

void CDisplayManager::removeAll()
{
    m_size = 0;
}

void CDisplayManager::removeAt(int i)
{
    for (; i < m_size - 2; ++i) {
        m_displays [i] = m_displays [i + 1];
    }
}

CDisplay & CDisplayManager::operator [] (int i)
{
    return m_displays [ i ] ;
}

CDisplay & CDisplayManager::operator [] (const char * name)
{
    for (int i=0; i < m_size; ++i) {
        if (std::string(m_displays[i].name()) == name) {
            return m_displays [ i ];
        }
    }
    return m_displays [ 0 ];
}

int CDisplayManager::findDisplay(const char *name)
{
    for (int i=0; i < m_size; ++i) {
        if (std::string(m_displays[i].name()) == name) {
            return  i ;
        }
    }
    return -1;
}

void CDisplayManager::drawImage(CDisplay & display)
{
    int screenLen;
    int screenHei;
    m_graphics->getScreenSize(screenLen, screenHei);
    m_graphics->paintImage(display.x(), screenHei - display.y(), display.imageSet(), display.imageNo());
}

void CDisplayManager::drawText(CDisplay & display)
{
    int screenLen;
    int screenHei;
    m_graphics->getScreenSize(screenLen, screenHei);

    CFont * font = m_game->m_font;
    font->FaceSize(display.size());
    int x = display.x();
    if (x < 0) {
        if ( x != -1) {
            x = screenLen + display.x();
        } else {
            if (x == -1) {
                int sx = font->Advance(display.text());
                x = (screenLen - sx) / 2;
            } else {
                x = screenLen - x;
            }
        }
    }

    int y = display.y();
    if (y < 0) {
        if ( y != -1) {
            y = screenHei - display.y();
        } else {
            if (y == -1) {
                y = (screenHei - display.size() ) / 2;
            } else {
                y = screenHei - y;
            }
        }
    }

    if (display.shadow()) {
        Color color = {(UINT8)display.shadowR(), (UINT8)display.shadowG(), (UINT8)display.shadowB(), (UINT8)display.shadowA()};
        m_graphics->render(*font, display.text(), x + display.shadowX(), y + display.shadowY(), color);
    }

    Color color = { (UINT8)display.red(), (UINT8)display.green(), (UINT8)display.blue(), (UINT8)display.alpha()};
    m_graphics->render(*font, display.text(), x, y, color);
}

void CDisplayManager::drawLives(CDisplay & display)
{
    char tmp[256];
    sprintf(tmp, display.templateStr(), m_game->getLives());
    display.setText(tmp, CDisplay::DISPLAY_SAME);
    drawText(display);
}

const char* CDisplayManager::signature()
{
    return "dm-opengl";
}

CDisplay & CDisplayManager::getAt(int i)
{
    return m_displays[i];
}

int CDisplayManager::getSize()
{
    return m_size;
}

bool CDisplayManager::isValidIndex(int i)
{
    return i>=0 && i < m_size;
}

void CDisplayManager::draw()
{
    char tmp[256];
    int score;
    for (int i=0; i < m_size; ++i) {
        if (m_displays[i].visible()) {
            switch (m_displays[i].type()) {

            case CDisplay::DISPLAY_TIME_LEFT:
                if (m_game->getTimeLeft()  > 0) {
                    sprintf(tmp, m_displays[i].templateStr(), m_game->getTimeLeft());
                    m_displays[i].setText(tmp, CDisplay::DISPLAY_SAME);
                    drawText(m_displays[i]);
                }
                break;

            case CDisplay::DISPLAY_LIVES:
                drawLives(m_displays[i]);
                break;

            case CDisplay::DISPLAY_DEBUGX:
                m_displays[i].setText(m_game->m_lua.getDebugText(), CDisplay::DISPLAY_SAME);
                drawText(m_displays[i]);
                break;

            case CDisplay::DISPLAY_IMAGE:
                drawImage(m_displays[i]);
                break;

            case CDisplay::DISPLAY_SCORE:
                score = m_game->getScore();
                sprintf(tmp, m_displays[i].templateStr(), score);
                m_displays[i].setText(tmp, CDisplay::DISPLAY_SAME);
                drawText(m_displays[i]);
                break;

            //case CDisplay::DISPLAY_MESSAGE:
            default:
                drawText(m_displays[i]);
            }
        }
    }
}

int CDisplayManager::display_sizeText(int displayId, const char *text)
{
    CFont * font = m_game->m_font;
    if (isValidIndex(displayId)) {
        CDisplay & display = m_displays[displayId];
        font->FaceSize(display.size());
        if (text){
            return font->Advance(text);
        } else {
            return font->Advance(display.text());
        }
    } else {
        return -1;
    }
}

void CDisplayManager::drawText(int x, int y, const char *text, int fontID, int fontSize, unsigned int rgba, int shadowOffset, unsigned int shadowColor)
{
    Q_UNUSED(fontID);
    int screenLen;
    int screenHei;
    m_graphics->getScreenSize(screenLen, screenHei);

    CFont * font = m_game->m_font;
    font->FaceSize(fontSize);
    if (x < 0) {
        if ( x != -1) {
            x = screenLen + x;
        } else {
            if (x == -1) {
                int sx = font->Advance(text);
                x = (screenLen - sx) / 2;
            } else {
                x = screenLen - x;
            }
        }
    }

    if (y < 0) {
        if ( y != -1) {
            y = screenHei + y;
        } else {
            if (y == -1) {
                y = (screenHei - fontSize ) / 2;
            } else {
                y = screenHei - y;
            }
        }
    }

    if (shadowOffset && shadowColor) {
        int red = shadowColor & 0xff;
        int green = (shadowColor >> 8) & 0xff;
        int blue = (shadowColor >> 16) & 0xff;
        int alpha = shadowColor >> 24;
        Color color = {(UINT8)red, (UINT8)green, (UINT8)blue, (UINT8)alpha};
        m_graphics->render(*font, text, x + shadowOffset, y + shadowOffset, color);
    }

    int red = rgba & 0xff;
    int green = (rgba >> 8) & 0xff;
    int blue = (rgba >> 16) & 0xff;
    int alpha = rgba >> 24;
    Color color = {(UINT8)red, (UINT8)green, (UINT8)blue, (UINT8)alpha};
    m_graphics->render(*font, text, x, y, color);
}
