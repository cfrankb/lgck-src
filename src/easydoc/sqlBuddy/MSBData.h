/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2009, 2010  Francois Blanchette

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

// database.h : header file
//

#ifndef _SQLMOTHER_H
#define _SQLMOTHER_H

#include "Table.h"
#include "AutoArr2.h"

/////////////////////////////////////////////////////////////////////////////
// CMSBData

class CMSBData
{
// Construction
public:
        CMSBData();

// Attributes
	int GetSize();

// Operations
        bool RemoveAt(int n);
        bool Add(CTable * p);
        bool read(FILE *);
        bool write(FILE *);
        void Forget();
	void Init();
	CTable * operator[] (int n);

        enum {
            DB_MAX_NAME = 32,
            VERSION = 0x0004
        };

// Implementation
        ~CMSBData();
	char m_szName[DB_MAX_NAME];

protected:
	CTable *m_pHead;
	CTable *m_pTail;
	int m_nSize;
	int m_nSelTable;
};

/////////////////////////////////////////////////////////////////////////////
#endif
