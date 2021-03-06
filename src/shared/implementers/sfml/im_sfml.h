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

#ifndef IM_Sfml_H
#define IM_Sfml_H

#include "interfaces/IImageManager.h"
#include <unordered_map>
#include <SFML/Graphics.hpp>

class CFont;
class CFrameSet;

class CIMSfml: public IImageManager
{
protected:
    CIMSfml();
public:
    virtual ~CIMSfml();
    virtual int add(const char *uuid, CFont * filter);
    virtual int add(CFrameSet * filter);
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

    std::unordered_map<unsigned int, sf::Texture*> m_textures;
    std::unordered_map<std::string, int> m_fonts;
    unsigned int m_nextTexture;
    int m_size;
    int m_max;
    ImageSet **m_imageSets;
    void deleteImageSet(ImageSet &set);
    void deleteTextures(int size, unsigned int *textures);
    sf::Texture *texture(int i);
    friend class CGRSfml;
    friend class CDMSfml;
}; 

#endif
