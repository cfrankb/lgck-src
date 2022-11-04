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

#ifndef CIMSDL_H
#define CIMSDL_H

#include "interfaces/IImageManager.h"
#include <unordered_map>
#include <SDL2/SDL.h>
#include <string>

class CIMSdl: public IImageManager
{
protected:
    CIMSdl();
public:
    virtual ~CIMSdl();
    virtual int add( CFrameSet * filter );
    virtual int add(const char *uuid, CFont * font);
    virtual void cacheInverse(int i, CFrameSet *filter);
    unsigned int getImage(int imageSet, int imageNo, bool wantInverse=false);
    virtual void forget();
    virtual void removeAt(int i);
    virtual void replace(int i, CFrameSet *filter);
    virtual int insertAt(int i, CFrameSet *filter);
    virtual bool hasInverse( int imageSet ) ;
    virtual int getSize();
    virtual const char *signature();
    virtual void makeCurrent();

protected:

    inline int pow2roundup(int x)
    {
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x+1;
    }

    struct ImageSet
    {
        unsigned int *images;
        unsigned int *inverse;
        int size;
    };

    enum {
        GROWBY = 20,
        PIXEL_DEPTH = 32
    };

    std::unordered_map<unsigned int, SDL_Texture*> m_textures;
    std::unordered_map<std::string, int> m_fonts;
    unsigned int m_nextTexture;
    int m_size;
    int m_max;
    ImageSet **m_imageSets;
    SDL_Renderer *m_renderer;
    void setRenderer(SDL_Renderer *renderer);
    void deleteImageSet(ImageSet &set);
    void deleteTextures(int size, unsigned int *textures);
    SDL_Texture *texture(int i);
    friend class CGRSdl;
}; 

#endif
