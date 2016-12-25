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

// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
// http://www.opengl.org/archives/resources/features/fontsurvey/
// http://students.cs.byu.edu/~bfish/glfontdl.php
// http://stackoverflow.com/questions/8847899/opengl-how-to-draw-text-using-only-opengl-methods

CGRSdl::CGRSdl()
{
    m_game = NULL;
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
        qDebug("SDL_CreateWindow failed");
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

void CGRSdl::drawLayer(CLayer * layer, int mx, int my)
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

void CGRSdl::drawScene(CScene * layer)
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

void CGRSdl::drawHP()
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    CActor & player = m_game->getPlayer();
    int x1 = 1;
    int x2 = x1 + player.getHP() * 2;
    int y1 = screenHei - 24;
    int y2 = screenHei - 8;
    UINT8 a = m_game->getDisplayAlpha();
    if (a) {
        paint(x1, y1, x2, y2, 0x40ff00 | (a << 24));
        paint(x1, y1, x2, y2, 0xffffff | (a << 24), false);
    }
}

void CGRSdl::getOffset(int & offsetX, int & offsetY)
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

void CGRSdl::drawScreen()
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
    drawHP();
    drawInventory();
    m_displayManager->draw();
}

void CGRSdl::drawInventory()
{
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
    CFrame *frame = (*( m_game->m_arrFrames[frameSet]))[frameNo];
    paintImage(x1,y1, frame, frameSet, frameNo);
}

void CGRSdl::paintImage(int x1, int y1, CFrame *frame, int frameSet, int frameNo)
{
    int len;
    int hei;
    getScreenSize(len, hei);
    unsigned int id_texture = m_imageManager->getImage(frameSet, frameNo);
    SDL_Rect sRect;
    sRect.x = sRect.y = 0;
    sRect.w = frame->m_nLen;
    sRect.h = frame->m_nHei;
    SDL_Rect dRect;
    dRect.x = x1;
    dRect.y = hei-y1;
    dRect.w = frame->m_nLen;
    dRect.h = frame->m_nHei;
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
    qDebug("driver count: %d", count);
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
        qDebug("driver %s", info.name);
        qDebug("...flags: %s", flags.c_str());
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
