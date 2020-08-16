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
#include "im_sdl.h"
#include "gr_sdl.h"
#include "DisplayManager.h"
#include "Game.h"
#include "Proto.h"
#include "Scene.h"
#include "Layer.h"
#include "Inventory.h"
#include "Level.h"
#include <SDL2/SDL.h>
#include "Font.h"

CGRSdl::CGRSdl()
{
    m_game = nullptr;
}

bool CGRSdl::init(CGame *game, int w, int h, const char*title)
{
    m_game = game;
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN );
    if (!m_window) {
        CLuaVM::debugv("SDL_CreateWindow failed");
        return false;
    }
    queryDriver();
    m_renderer = SDL_CreateRenderer( m_window, -1, SDL_RENDERER_SOFTWARE );
    m_imageManager = new CIMSdl();
    m_imageManager->setRenderer(m_renderer);
    m_displayManager = new CDisplayManager(game, m_imageManager, this);
    game->updateGeometry(w, h);
    return true;
}

const char* CGRSdl::lastError()
{
    return m_lastError.c_str();
}

CGRSdl::~CGRSdl()
{
    delete m_displayManager;

    //Destroy window
    SDL_DestroyWindow( m_window );

    //Destroy renderer
    SDL_DestroyRenderer( m_renderer );

    //Quit SDL subsystems
   // SDL_Quit();
}

const char* CGRSdl::signature()
{
    return "graphik-sdl2";
}

void CGRSdl::getScreenSize(int & len, int & hei)
{
    SDL_GetRendererOutputSize(m_renderer, &len, &hei);
    //SDL_GetWindowSize(m_window, &len, &hei);
}

void CGRSdl::clear(unsigned int red, unsigned int green, unsigned int blue)
{
    // Set Color
    SDL_SetRenderDrawColor(
                m_renderer,
                red,
                green,
                blue,
                0xff);
    // Clear screen
    SDL_RenderClear( m_renderer );
}

void CGRSdl::clear(unsigned int rgb)
{
    clear(rgb & 0xff, (rgb >> 8) & 0xff, (rgb >> 16) & 0xff);
}

void CGRSdl::paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill)
{
    unsigned int red = rgba & 0xff;
    unsigned int green = (rgba >> 8) & 0xff;
    unsigned int blue = (rgba >> 16) & 0xff;
    unsigned int alpha = (rgba >> 24) & 0xff;
    SDL_SetRenderDrawColor(
        m_renderer,
        red,
        green,
        blue,
        alpha);
    SDL_Rect rect;
    rect.x = x1;
    rect.y = y1;
    rect.w = x2-x1;
    rect.h = y2-y1;
    if (fill) {
        SDL_RenderFillRect(m_renderer, &rect);
    } else {
        SDL_RenderDrawRect(m_renderer, &rect);
    }
}

void CGRSdl::paintImage(int x1, int y1, int frameSet, int frameNo)
{
    CFrame & frame = m_game->toFrame(frameSet, frameNo);
    int len;
    int hei;
    getScreenSize(len, hei);
    unsigned int id_texture = m_imageManager->getImage(frameSet, frameNo);
    SDL_Rect sRect;
    sRect.x = sRect.y = 0;
    sRect.w = frame.m_nLen;
    sRect.h = frame.m_nHei;
    SDL_Rect dRect;
    dRect.x = x1;
    dRect.y = hei-y1;
    dRect.w = frame.m_nLen;
    dRect.h = frame.m_nHei;
    SDL_Texture * texture = m_imageManager->texture(id_texture);
    SDL_SetTextureColorMod(
                texture,
                m_colorMod.red,
                m_colorMod.green,
                m_colorMod.blue);
    SDL_RenderCopy(
        m_renderer,
        texture,
        &sRect,
        &dRect);
}

IDisplayManager* CGRSdl::displayManager()
{
    return (IDisplayManager*) m_displayManager;
}

void CGRSdl::ss_paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill)
{
    paint(x1, y1, x2, y2, rgba, fill);
}

void CGRSdl::ss_paintImage(int x1, int y1, int frameSet, int frameNo)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    paintImage(x1,screenHei - y1, frameSet, frameNo);
}

IImageManager *CGRSdl::CGRSdl::cache()
{
    return (IImageManager *) m_imageManager;
}

void CGRSdl::update()
{
    SDL_RenderPresent( m_renderer );
}

void CGRSdl::queryDriver()
{
    int count = SDL_GetNumRenderDrivers();
    CLuaVM::debugv("driver count: %d", count);
    for (int i=0; i < count ; ++i) {
        SDL_RendererInfo info;
        int result = SDL_GetRenderDriverInfo(i, &info);
        Q_UNUSED(result);
        std::string flags;
        if (info.flags & SDL_RENDERER_SOFTWARE) {
            flags += "SDL_RENDERER_SOFTWARE ";
        }
        if (info.flags & SDL_RENDERER_ACCELERATED) {
            flags += "SDL_RENDERER_ACCELERATED ";
        }
        if (info.flags & SDL_RENDERER_PRESENTVSYNC) {
            flags += "SDL_RENDERER_PRESENTVSYNC ";
        }
        if (info.flags & SDL_RENDERER_TARGETTEXTURE) {
            flags += "SDL_RENDERER_TARGETTEXTURE ";
        }
        CLuaVM::debugv("driver %s", info.name);
        CLuaVM::debugv("...flags: %s", flags.c_str());
    }
}

void CGRSdl::render(CFont &font, const char *text, int x, int y, const Color &color)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    SDL_Texture * texture = m_imageManager->texture(font.textureId());
    ASSERT(texture);
    SDL_SetTextureColorMod(
                texture,
                color.red,
                color.green,
                color.blue);
    float xx = x;
    for (int i=0; text[i]; ++i) {
        char id = text[i];
        CFont::Glyph & glyph = font[id];
        if (id == ' ' || !glyph.width) {
            xx += glyph.width * font.scale();
            continue;
        }
        SDL_Rect sRect = {
            glyph.x,
            glyph.y,
            glyph.width,
            font.scaleY()
        };
        SDL_Rect dRect = {
            (int)xx,
            y,
            (int) (font.scale() * glyph.width),
            (int) (font.scale() * font.scaleY())
        };
        SDL_RenderCopy(
            m_renderer,
            texture,
            &sRect,
            &dRect);
        xx += glyph.width * font.scale();
    }
}
