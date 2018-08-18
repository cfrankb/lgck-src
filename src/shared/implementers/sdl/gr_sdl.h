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

#ifndef GR_SDL_H
#define GR_SDL_H

#include <cstring>
#include <SDL2/SDL.h>
#include "../shared/interfaces/IGraphics.h"

class CFrame;
class CGame;
class IDisplayManager;
class CDisplayManager;
class CIMSdl;
class CFont;

class CGRSdl : public IGraphics
{
public:
    CGRSdl();
    bool init(CGame *game, int w, int h, const char *title);
    const char* lastError();
    virtual ~CGRSdl();
    virtual const char* signature();
    virtual void drawScreen();
    virtual void getOffset(int & offsetX, int & offsetY);
    virtual void getScreenSize(int & len, int & hei);
    virtual void clear(unsigned int red, unsigned int green, unsigned int blue);
    virtual void clear(unsigned int rgb);
    virtual void paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill=true);
    virtual void paintImage(int x1, int y1, int frameSet, int frameNo);
    virtual void paintImage(int x1, int y1, CFrame * frame, int frameSet, int frameNo);
    virtual void render(CFont & font, const char *text, int x, int y, const Color & color);
    virtual IDisplayManager* displayManager();
    virtual void ss_paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill=true);
    virtual void ss_paintImage(int x1, int y1, int frameSet, int frameNo);
    virtual IImageManager *cache();
    void update();
    SDL_Renderer * renderer();

protected:
    enum {
        SCREEN_WIDTH = 640,
        SCREEN_HEIGHT = 480
    };

    CGame *m_game;
    CIMSdl *m_imageManager;
    CDisplayManager *m_displayManager;
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    void queryDriver();
    virtual void drawLayer (CLayer * layer, int mx, int my);
    virtual void drawScene (CScene * layer);
    struct {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    } m_colorMod;
    std::string m_lastError;
};

#endif // GR_SDL_H
