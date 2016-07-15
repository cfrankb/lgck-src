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
#include "im_sfml.h"
#include "../shared/Frame.h"
#include "../shared/FrameSet.h"
#include "Font.h"
#include <cstring>

CIMSfml::CIMSfml()
{
    m_size = 0;
    m_max = GROWBY;
    m_imageSets = new ImageSet * [ m_max ];
    m_nextTexture = 0;
}

CIMSfml::~CIMSfml()
{
    forget();
    if (m_imageSets) {
        delete [] m_imageSets;
        m_imageSets = NULL;
    }
}

void CIMSfml::deleteImageSet(ImageSet &set)
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

void CIMSfml::forget()
{
    for (int i = 0; i < m_size; ++i) {
        deleteImageSet(* m_imageSets[i]);
    }
    m_size = 0;
}

int CIMSfml::add(CFrameSet * filter)
{
    return insertAt(m_size, filter);
}

void CIMSfml::cacheInverse(int i, CFrameSet *filter)
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
        UINT32 *t = frame.getRGB();
        for (int j=0; j < frame.m_nLen * frame.m_nHei; ++j) {
            t[j] = (t[j] & 0x00ffffff) + (t[j] & 0xff000000) / 2;
        }
        sf::Texture *texture = new sf::Texture();
        if (!texture->create(frame.m_nLen, frame.m_nHei)) {
            qDebug("texture.create() failed");
        }
        texture->update(reinterpret_cast<sf::Uint8*>(frame.getRGB()));
        texture->setRepeated(false);
        texture->setSmooth(false);
        m_textures[m_nextTexture] = texture;
        is->inverse[x] = m_nextTexture++;
    }
}

void CIMSfml::removeAt(int i)
{
    deleteImageSet(* m_imageSets [i]);
    for (int j=i; j < m_size - 1; ++j) {
        m_imageSets[j] = m_imageSets[j + 1];
    }
    -- m_size;
}

unsigned int CIMSfml::getImage(int imageSet, int imageNo, bool wantInverse)
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

bool CIMSfml::hasInverse( int imageSet )
{
    if ( (imageSet < 0 || imageSet >= m_size ) ) {
        imageSet = 0;
    }

    return m_imageSets [ imageSet ]->inverse != NULL;
}

void CIMSfml::replace(int i, CFrameSet *filter)
{
    removeAt(i);
    insertAt(i, filter);
}

int CIMSfml::add(const char *name, CFont *font)
{
    qDebug("adding font: %s %dx%d", name, font->width(), font->height());
    ASSERT(font->pixels());
    makeCurrent();
    sf::Texture *texture = new sf::Texture();
    ASSERT(texture);
    if (!texture->create(font->width(), font->height())) {
        qDebug("font texture.create() failed");
    }
    texture->update(reinterpret_cast<sf::Uint8*>(font->pixels()));
    texture->setRepeated(false);
    texture->setSmooth(true);
    m_textures[m_nextTexture] = texture;
    m_fonts[name] = m_nextTexture;
    return m_nextTexture++;
}

int CIMSfml::insertAt(int i, CFrameSet *filter)
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
        sf::Texture *texture = new sf::Texture();
        if (!texture->create(frame.m_nLen, frame.m_nHei)) {
            qDebug("texture.create() failed");
        }
        texture->update(reinterpret_cast<sf::Uint8*>(frame.getRGB()));
        texture->setRepeated(false);
        texture->setSmooth(false);
        m_textures[m_nextTexture] = texture;
        is->images[x] = m_nextTexture++;
    }
    m_imageSets [ i ] = is;
    ++ m_size;
    return i;
}

int CIMSfml::getSize()
{
    return m_size;
}

const char * CIMSfml::signature()
{
    return "lgck-sfml2";
}

void CIMSfml::makeCurrent()
{
}

void CIMSfml::deleteTextures(int size, unsigned int *textures)
{
    for (int i = 0; i < size; ++i) {
        int j = textures[i];
        sf::Texture * texture = m_textures[j];
        if (texture) {
            delete texture;
        }
        m_textures.erase(j);
    }
}

sf::Texture *CIMSfml::texture(int i)
{
    return m_textures[i];
}
