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

#ifndef IIMAGEMANAGER_H
#define IIMAGEMANAGER_H

class CFrameSet;
class CFont;

class IImageManager
{   
public:
    virtual ~IImageManager()=0;
    virtual int add(CFrameSet * filter)=0;
    virtual int add(const char *uuid, CFont * filter)=0;
    virtual void cacheInverse(int i, CFrameSet *filter)=0;
    virtual void forget()=0;
    virtual void removeAt(int i)=0;
    virtual void replace(int i, CFrameSet *filter)=0;
    virtual int insertAt(int i, CFrameSet *filter)=0;
    virtual bool hasInverse( int imageSet )=0;
    virtual int getSize()=0;
    virtual const char *signature()=0;
    virtual void makeCurrent()=0;
};

inline IImageManager::~IImageManager(){}

#endif
