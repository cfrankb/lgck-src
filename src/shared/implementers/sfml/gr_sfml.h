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

#ifndef GR_SFML_H
#define GR_SFML_H

#include "interfaces/IGraphics.h"
#include <SFML/Graphics.hpp>
#include "Color.h"

class CFrame;
class CGame;
class IDisplayManager;
class CDisplayManager;
class CIMSfml;
class CFont;

class CGRSfml : public IGraphics
{
public:
    CGRSfml();
    bool init(CGame *game, int w, int h, const char *title);
    const char* lastError();
    virtual ~CGRSfml();
    virtual const char* signature();
    virtual void getScreenSize(int & len, int & hei);
    virtual void clear(unsigned int red, unsigned int green, unsigned int blue);
    virtual void clear(unsigned int rgb);
    virtual void paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill=true);
    virtual void paintImage(int x1, int y1, int frameSet, int frameNo);
    virtual void render(CFont & font, const char *text, int x, int y, const Color & color);
    virtual IDisplayManager* displayManager();
    virtual void ss_paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill=true);
    virtual void ss_paintImage(int x1, int y1, int frameSet, int frameNo);
    virtual IImageManager *cache();
    void update();
    virtual bool isFlipped(){ return true; }
    sf::RenderWindow *window();

protected:
    enum {
        SCREEN_WIDTH = 640,
        SCREEN_HEIGHT = 480
    };
    struct {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    } m_colorMod;
    CIMSfml *m_imageManager;
    sf::RenderWindow *m_window;
    std::string m_lastError;
    int m_screenWidth;
    int m_screenHeight;
};

#endif // GR_SDL_H
