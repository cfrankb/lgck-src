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
#include "im_sdl.h"
#include "../shared/Frame.h"
#include "../shared/FrameSet.h"

#include <cstring>
#include <SDL2/SDL.h>
#include "Font.h"

CIMSdl::CIMSdl()
{
    m_size = 0;
    m_max = GROWBY;
    m_imageSets = new ImageSet * [ m_max ];
    m_nextTexture = 0;
    m_renderer = NULL;
}

CIMSdl::~CIMSdl()
{
    forget();
    if (m_imageSets) {
        delete [] m_imageSets;
        m_imageSets = NULL;
    }
}

void CIMSdl::deleteImageSet(ImageSet &set)
{
    makeCurrent();
    deleteTextures(set.size, set.images);
    if (set.inverse) {
        deleteTextures(set.size, set.inverse);
        delete [] set.inverse;
    }
    delete [] set.images;
    delete &set;
}

void CIMSdl::forget()
{
    qDebug("CIMSdl::forget()");
    for (int i = 0; i < m_size; ++i) {
     //   qDebug("CIMSdl::forget() %d",i);
        deleteImageSet(* m_imageSets[i]);
    }
    m_size = 0;
}

int CIMSdl::add(CFrameSet * filter)
{
    return insertAt(m_size, filter);
}

void CIMSdl::cacheInverse(int i, CFrameSet *filter)
{
    ImageSet *is = m_imageSets [i];
    makeCurrent();
    if (is->inverse) {
        deleteTextures(is->size, is->inverse);
        delete [] is->inverse;
    }

    is->inverse = new unsigned int [ is->size ];
    memset(is->inverse, 0, sizeof(unsigned int)*is->size);
    for (int x=0; x < filter->getSize(); ++x) {
        CFrame frame;
        frame.copy((*filter)[x]);
        uint32_t *t = frame.getRGB();
        for (int j=0; j < frame.m_nLen * frame.m_nHei; ++j) {
            t[j] = (t[j] & 0x00ffffff) + (t[j] & 0xff000000) / 2;
        }
        SDL_Surface* surface =
           SDL_CreateRGBSurfaceFrom(t,
           frame.m_nLen,
           frame.m_nHei,
           PIXEL_DEPTH,
           frame.m_nLen * sizeof(unsigned int),
           0xff, 0xff00, 0xff0000, 0xff000000);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(
                    m_renderer, surface );
        SDL_FreeSurface( surface );
        m_textures[m_nextTexture] = texture;
        is->inverse[x] = m_nextTexture++;
    }
}

void CIMSdl::removeAt(int i)
{
    deleteImageSet(* m_imageSets [i]);
    for (int j=i; j < m_size - 1; ++j) {
        m_imageSets[j] = m_imageSets[j + 1];
    }
    -- m_size;
}

unsigned int CIMSdl::getImage(int imageSet, int imageNo, bool wantInverse)
{
    if ( (imageSet < 0 || imageSet >= m_size || imageNo < 0) ||
         (imageNo >= m_imageSets[imageSet]->size) ) {
        imageSet = 0;
        imageNo = 0;
    }

    // TODO: add extra validation
    if (wantInverse) {
        return m_imageSets [ imageSet ]->inverse[ imageNo ];
    } else {
        return m_imageSets [ imageSet ]->images[ imageNo ];
    }
}

bool CIMSdl::hasInverse( int imageSet )
{
    if ( (imageSet < 0 || imageSet >= m_size ) ) {
        imageSet = 0;
    }

    return m_imageSets [ imageSet ]->inverse != NULL;
}

void CIMSdl::replace(int i, CFrameSet *filter)
{
    removeAt(i);
    insertAt(i, filter);
}

int CIMSdl::insertAt(int i, CFrameSet *filter)
{
    if (m_size == m_max) {
        m_max += GROWBY;
        ImageSet ** t = new ImageSet * [m_max];
        memcpy(t, m_imageSets, sizeof(ImageSet *) * m_size);
        delete [] m_imageSets;
        m_imageSets = t;
    }

    for (int j = m_size; j > i; --j) {
        m_imageSets [ j ] = m_imageSets [ j - 1 ];
    }

    makeCurrent();
    ImageSet * is = new ImageSet;
    is->size = filter->getSize();
    is->images = new unsigned int [ is->size ];
    memset(is->images, 0, sizeof(unsigned int)*is->size);
    is->inverse = NULL;

    for (int x=0; x < filter->getSize(); ++x) {
        CFrame frame;
        frame.copy((*filter)[x]);
        unsigned int *pixels = frame.getRGB();
        SDL_Surface* surface =
           SDL_CreateRGBSurfaceFrom(pixels,
           frame.m_nLen,
           frame.m_nHei,
           PIXEL_DEPTH,
           frame.m_nLen * sizeof(unsigned int),
           0xff, 0xff00, 0xff0000, 0xff000000);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(
                    m_renderer, surface );
        SDL_FreeSurface( surface );
        m_textures[m_nextTexture] = texture;
        is->images[x] = m_nextTexture++;
    }
    m_imageSets [ i ] = is;
    ++ m_size;
    return i;
}

int CIMSdl::getSize()
{
    return m_size;
}

const char * CIMSdl::signature()
{
    return "lgck-sdl2";
}

void CIMSdl::makeCurrent()
{
}

void CIMSdl::deleteTextures(int size, unsigned int *textures)
{
    for (int i = 0; i < size; ++i) {
        int j = textures[i];
        SDL_Texture * texture = m_textures[j];
        if (texture) {
            SDL_DestroyTexture( texture );
        }
        m_textures.erase(j);
    }
}

SDL_Texture *CIMSdl::texture(int i)
{
    return m_textures[i];
}

void CIMSdl::setRenderer(SDL_Renderer *renderer)
{
    m_renderer = renderer;
}

int CIMSdl::add(const char *uuid, CFont *font)
{
    qDebug("adding font: %s %dx%d", uuid, font->width(), font->height());
    ASSERT(font->pixels());
    makeCurrent();
    SDL_Surface* surface =
       SDL_CreateRGBSurfaceFrom(font->pixels(),
       font->width(), font->height(),
       PIXEL_DEPTH,
       font->width() * sizeof(unsigned int),
       0xff, 0xff00, 0xff0000, 0xff000000);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(
                m_renderer, surface );
    SDL_FreeSurface( surface );
    ASSERT(texture);
    //texture->setRepeated(false);
    //texture->setSmooth(true);
    m_textures[m_nextTexture] = texture;
    m_fonts[uuid] = m_nextTexture;
    return m_nextTexture++;
}
