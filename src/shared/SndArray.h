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

// SndArray.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSndArray 

#ifndef _SNDARRAY_H
#define _SNDARRAY_H

class CSnd;

class CSndArray 
{
public:
    CSndArray();
    ~CSndArray();
    int getSize();
    void add(CSnd * pSnd);
    void removeAt(int n);
    void forget();
    CSnd * operator [] (int n);
    CSnd * operator [] (const char*);
    void debug();
    bool hasSound(int i);
    bool hasSound(const char *s);

private:
    enum {
        GROWBY  = 16
    };
    CSnd ** m_arrSnds;
    int m_nMax;
    int m_nSize;
};

/////////////////////////////////////////////////////////////////////////////

#endif
