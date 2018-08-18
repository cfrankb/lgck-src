/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2011  Francois Blanchette

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

#ifndef _OBL3ARRAY_H
#define _OBL3ARRAY_H

// FrameArray.h : header file
//

class QString;

/////////////////////////////////////////////////////////////////////////////
// CFrameArray

#include "FrameSet.h"
class CLevelEntry;

class CFrameArray
{
    // Construction
public:
    CFrameArray();

    // Attributes
public:
    int getSize();
    int getNameIndex (const char *str);

    // Operations
public:
    void setAt (int n, CFrameSet * pFilter);
    CFrame * getFrame (const CLevelEntry &entry);
    CFrame * getFrame (const int imageSet, const int imageNo);
    CFrameSet * operator [] (int);
    CFrameSet * operator [] (const char *);
    int add(CFrameSet *pFilter);
    void removeAt(int n);
    void forget();

    // Implementation
public:
    ~CFrameArray();

private:
    int m_nMax;
    int m_nSize;
    CFrameSet ** m_arrFilters;
};

/////////////////////////////////////////////////////////////////////////////

#endif
