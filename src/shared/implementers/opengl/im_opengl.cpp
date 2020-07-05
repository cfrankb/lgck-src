/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2013  Francois Blanchette

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
#include "im_opengl.h"
#include "../shared/Frame.h"
#include "../shared/FrameSet.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <cstring>
#include "Font.h"
#include "implementers/opengl/glhelper.h"

CIMOpengl::CIMOpengl()
{
    m_size = 0;
    m_max = GROWBY;
    m_imageSets = new ImageSet * [ m_max ];
#ifdef LGCK_QT
    initializeOpenGLFunctions();
#endif

}

CIMOpengl::~CIMOpengl()
{
    forget();
    if (m_imageSets) {
        delete [] m_imageSets;
        m_imageSets = NULL;
    }
}

void CIMOpengl::deleteImageSet(ImageSet &set)
{
    makeCurrent();
    glDeleteTextures(set.size, set.images);
    if (set.inverse) {
        glDeleteTextures(set.size, set.inverse);
        delete [] set.inverse;
    }
    delete [] set.images;
    delete &set;
}

void CIMOpengl::forget()
{
    for (int i = 0; i < m_size; ++i) {
        deleteImageSet(* m_imageSets[i]);
    }
    m_size = 0;
}

int CIMOpengl::add(CFrameSet * filter)
{
    return insertAt(m_size, filter);
}

void CIMOpengl::cacheInverse(int i, CFrameSet *filter)
{
    ImageSet *is = m_imageSets [i];
    makeCurrent();
    if (is->inverse) {
        glDeleteTextures(is->size, is->inverse);
        delete [] is->inverse;
    }

    is->inverse = new unsigned int [ is->size ];
    memset(is->inverse, 0, sizeof(unsigned int)*is->size);
    glGenTextures(is->size, is->inverse);
    for (int i=0; i < filter->getSize(); ++i) {
        CFrame frame;
        frame.copy((*filter)[i]);
        uint32_t *t = frame.getRGB();
        for (int j=0; j < frame.m_nLen * frame.m_nHei; ++j) {
            t[j] = (t[j] & 0x00ffffff) + (t[j] & 0xff000000) / 2;
        }
        // TODO: quickfix for quirky textures
        int ix = pow2roundup(frame.m_nLen);
        int iy = pow2roundup(frame.m_nHei);
        if (ix != frame.m_nLen || iy != frame.m_nHei) {
            frame.resize(ix,iy);
        }
        frame.flipV();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glBindTexture(GL_TEXTURE_2D, is->inverse[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.m_nLen, frame.m_nHei, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.getRGB() );
    }
}

void CIMOpengl::removeAt(int i)
{
    deleteImageSet(* m_imageSets [i]);
    for (int j=i; j < m_size - 1; ++j) {
        m_imageSets[j] = m_imageSets[j + 1];
    }
    -- m_size;
}

unsigned int CIMOpengl::getImage(int imageSet, int imageNo, bool wantInverse)
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

bool CIMOpengl::hasInverse( int imageSet )
{
    if ( (imageSet < 0 || imageSet >= m_size ) ) {
        imageSet = 0;
    }

    return m_imageSets [ imageSet ]->inverse != NULL;
}

void CIMOpengl::replace(int i, CFrameSet *filter)
{
    removeAt(i);
    insertAt(i, filter);
}

int CIMOpengl::insertAt(int i, CFrameSet *filter)
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
    glGenTextures(is->size, is->images);

    for (int j=0; j < filter->getSize(); ++j) {
        CFrame frame;
        frame.copy((*filter)[j]);
        // TODO: quickfix for quirky textures
        int ix = pow2roundup(frame.m_nLen);
        int iy = pow2roundup(frame.m_nHei);
        if (ix != frame.m_nLen || iy != frame.m_nHei) {
            frame.resize(ix,iy);
        }
        frame.flipV();
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glBindTexture(GL_TEXTURE_2D, is->images[j]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.m_nLen, frame.m_nHei, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.getRGB() );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    m_imageSets [ i ] = is;
    ++ m_size;
    return i;
}

int CIMOpengl::getSize()
{
    return m_size;
}

const char * CIMOpengl::signature()
{
    return "lgck-qtOpenGL";
}

void CIMOpengl::makeCurrent()
{
  //  qDebug("m_glWidget->makeCurrent()");
}

int CIMOpengl::add(const char *uuid, CFont *font)
{
    makeCurrent();
    GLuint textureId = -1;
    GLint maxSize;
    glEnable(GL_TEXTURE_2D); GLDEBUG();
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize); GLDEBUG();
    qDebug("MAXSIZE: %u", maxSize);

    glGenTextures(1, &textureId); GLDEBUG();
    qDebug("texture: %.8x", textureId);
    CFrame frame(font->width(), font->height());
    memcpy(frame.getRGB(), font->pixels(), font->width() * font->height() * sizeof(unsigned int));
    int ix = pow2roundup(frame.m_nLen);
    int iy = pow2roundup(frame.m_nHei);
    if (ix != frame.m_nLen || iy != frame.m_nHei) {
        frame.resize(ix,iy);
    }
    frame.flipV();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); GLDEBUG();
    glBindTexture(GL_TEXTURE_2D, textureId); GLDEBUG();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.m_nLen, frame.m_nHei, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.getRGB() ); GLDEBUG();
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); GLDEBUG();
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); GLDEBUG();
    //glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE); GLDEBUG();
    //glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE); GLDEBUG();
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); GLDEBUG();
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); GLDEBUG();
    font->setTextureId(textureId);
    m_fonts[uuid] = textureId;
    return textureId;
}
