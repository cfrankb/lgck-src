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
#include "fontmanager.h"
#include "Actor.h"
#include "Inventory.h"

#define RGBA(rgba) rgba & 0xff, (rgba & 0xff00) >> 8, (rgba & 0xff0000) >> 16, (rgba & 0xff000000) >> 24

// https://stackoverflow.com/questions/12157646/how-to-render-offscreen-on-opengl

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
    int j = indexOf(display.name());
    if (j == NOT_FOUND) {
        if (m_size == m_max) {
            m_max += GROWBY;
            CDisplay *t = new CDisplay [ m_max ];
            for (int i=0; i < m_size; ++i) {
                t[i] = m_displays[i];
            }
            delete [] m_displays;
            m_displays = t;
        }
        ++ m_size;
        j = m_size - 1;
    }
    m_displays[j] = display;
    return m_displays[j];
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
    return get(name);
}

CDisplay & CDisplayManager::get(const char * name)
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
    int x = display.x();
    int y = display.y();
    m_graphics->getScreenSize(screenLen, screenHei);
    int flagX = display.flagX();
    int flagY = display.flagY();
    if (flagX || flagY) {
        CFrame & frame = m_game->toFrame(display.imageSet(), display.imageNo());
        switch (flagX) {
        case CDisplay::FLAG_X_ALIGN_LEFT:
            x = 0;
            break;
        case CDisplay::FLAG_X_ALIGN_RIGHT:
            x = screenLen - frame.m_nLen;
            break;
        case CDisplay::FLAG_X_ALIGN_CENTER:
            x = (screenLen - frame.m_nLen) / 2;
        }
        switch (flagY) {
        case CDisplay::FLAG_Y_ALIGN_TOP:
            y = 0;
            break;
        case CDisplay::FLAG_Y_ALIGN_BOTTOM:
            y = screenHei - frame.m_nHei;
            break;
        case CDisplay::FLAG_Y_ALIGN_CENTER:
            y = (screenHei - frame.m_nHei) / 2;
        }
    }
    m_graphics->ss_paintImage(x, y, display.imageSet(), display.imageNo());
}

int CDisplayManager::computeY(CDisplay & display, int sy)
{
    int flagY = display.flagY();
    int screenLen;
    int screenHei;
    m_graphics->getScreenSize(screenLen, screenHei);

    int y = display.y();
    if (y < 0) {
        if ( y != -1) {
            y = screenHei - display.y();
        } else {
            if (y == -1) {
                y = (screenHei - sy ) / 2;
            } else {
                y = screenHei - y;
            }
        }
    }
    switch (flagY) {
    case CDisplay::FLAG_Y_ALIGN_TOP:
        y = 0;
        break;
    case CDisplay::FLAG_Y_ALIGN_BOTTOM:
        y = screenHei - sy;
        break;
    case CDisplay::FLAG_Y_ALIGN_CENTER:
        y = (screenHei - sy) / 2;
    }
    return y;
}

int CDisplayManager::computeX(CDisplay & display, int sx)
{
    int flagX = display.flagX();
    int screenLen;
    int screenHei;
    m_graphics->getScreenSize(screenLen, screenHei);
    int x = display.x();
    if (x < 0) {
        if ( x != -1) {
            x = screenLen + display.x();
        } else {
            if (x == -1) {
                x = (screenLen - sx) / 2;
            } else {
                x = screenLen - x;
            }
        }
    }

    switch (flagX) {
    case CDisplay::FLAG_X_ALIGN_LEFT:
        x = 0;
        break;
    case CDisplay::FLAG_X_ALIGN_RIGHT:
        x = screenLen - sx;
        break;
    case CDisplay::FLAG_X_ALIGN_CENTER:
        x = (screenLen - sx) / 2;
    }
    return x;
}

void CDisplayManager::drawText(CDisplay & display)
{
    CFont *font = m_game->getFonts()->at(display.font());
    if (!font) {
        CLuaVM::debugv("warning: invalid fontID: %d", display.font());
        font = m_game->getFonts()->at(0);
    }
    font->FaceSize(display.size());

    std::list<std::string> & lines = display.lines();
    int dy = display.size();
    int sy = dy * lines.size();
    int y = computeY(display, sy);

    std::list<std::string>::iterator it;
    for (it = lines.begin(); it != lines.end(); ++it) {
        int sx = font->Advance(it->c_str());
        int x = computeX(display, sx);
        if (display.shadow()) {
            Color color = {(uint8_t)display.shadowR(), (uint8_t)display.shadowG(), (uint8_t)display.shadowB(), (uint8_t)display.shadowA()};
            m_graphics->render(*font, it->c_str(), x + display.shadowX(), y + display.shadowY(), color);
        }

        Color color = { (uint8_t)display.red(), (uint8_t)display.green(), (uint8_t)display.blue(), (uint8_t)display.alpha()};
        m_graphics->render(*font, it->c_str(), x, y, color);
        y += dy;
    }
}

void CDisplayManager::drawLives(CDisplay & display)
{
    char *tmp = nullptr;
    if (asprintf(&tmp, display.templateStr(), m_game->getLives()) == -1) {
        qDebug("asprintf alloc failure in CDisplayManager::drawLives");
    }
    display.setText(tmp, CDisplay::DISPLAY_SAME);
    free(tmp);
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
    for (int i=0; i < m_size; ++i) {
        if (m_displays[i].visible()) {
            drawDisplay(m_displays[i]);
        }
    }
    drawInventory();
}

int CDisplayManager::display_sizeText(int displayId, const char *text)
{
    CFont *font = m_game->getFonts()->find(DEFAULT_FONT);
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
    CDisplay d;
    d.setFontSize(fontSize);
    d.setColor(RGBA(rgba));
    d.setXY(x, y);
    d.setText(text);
    d.setShadowColor(RGBA(shadowColor));
    d.setShadow(shadowColor != 0, shadowOffset, shadowOffset);
    d.setFont(fontID);
    drawText(d);
}

int CDisplayManager::indexOf(const char *name)
{
    for (int i=0; i < m_size; ++i) {
        if (std::string(m_displays[i].name()) == name) {
            return i;
        }
    }
    return NOT_FOUND;
}

void CDisplayManager::drawHP()
{
    int screenLen;
    int screenHei;
    m_graphics->getScreenSize(screenLen, screenHei);
    CActor & player = m_game->getPlayer();
    int sy = 16;
    int sx = player.getHP() * 2;

    CDisplay & display = get("healthbar");
    int x = computeX(display, sx);
    int y = computeY(display, sy);

    uint8_t a = m_game->getDisplayAlpha();
    if (a) {
        m_graphics->paint(x,screenHei - y, x + sx, screenHei - y - sy, display.rgb() + (a << 24));
        m_graphics->paint(x,screenHei - y, x + sx, screenHei - y - sy, 0xffffff + (a << 24), false);
    }
}

void CDisplayManager::drawInventory()
{
    int screenLen;
    int screenHei;
    m_graphics->getScreenSize(screenLen, screenHei);
    const CInventory *inventory = m_game->getInventory();
    int i = 0;
    for (int j=0; inventory && (j < inventory->getSize()); ++j) {
        if ((*inventory)[j] != 0) {
            CProto proto = m_game->m_arrProto[(*inventory)[j]];
            if (!proto.getOption(CProto::OPTION_INVENTORY_HIDDEN)) {
                int imageSet = proto.m_nFrameSet;
                int imageNo = proto.m_nFrameNo;
                CFrame & frame = m_game->toFrame(imageSet, imageNo);
                int x = screenLen - frame.m_nLen - 4;
                int y = 32 * (i + 1) + 4;
                m_graphics->ss_paintImage(x, y, imageSet, imageNo);
                ++i;
            }
        }
    }
}

void CDisplayManager::drawDisplay(CDisplay & display)
{
    char *tmp = nullptr;
    int score;

    switch (display.type()) {
    case CDisplay::DISPLAY_TIME_LEFT:
        if (m_game->getTimeLeft()  > 0) {
            if (asprintf(&tmp, display.templateStr(), m_game->getTimeLeft())==-1) {
                qDebug("malloc failure in CDisplayManager::drawDisplay(CDisplay & display)");
            } else {
                display.setText(tmp, CDisplay::DISPLAY_SAME);
                free(tmp);
                drawText(display);
            }
        }
        break;

    case CDisplay::DISPLAY_LIVES:
        drawLives(display);
        break;

    case CDisplay::DISPLAY_DEBUGX:
        // TODO: DEPRECATED fix this function
        //display.setText(m_game->m_lua.getDebugText(), CDisplay::DISPLAY_SAME);
        drawText(display);
        break;

    case CDisplay::DISPLAY_IMAGE:
        drawImage(display);
        break;

    case CDisplay::DISPLAY_SCORE:
        score = m_game->getScore();
        if (asprintf(&tmp, display.templateStr(), score) == -1) {
            qDebug("malloc failure in CDisplayManager::drawDisplay(CDisplay & display)");
        } else {
            display.setText(tmp, CDisplay::DISPLAY_SAME);
            free(tmp);
            drawText(display);
        }
        break;

    case CDisplay::DISPLAY_HEALTH_BAR:
        drawHP();
        break;

    case CDisplay::DISPLAY_MESSAGE:
        drawText(display);
        break;

    default:
        CLuaVM::debugv("unknown DisplayType: %d", display.type());
        drawText(display);
    }
}
