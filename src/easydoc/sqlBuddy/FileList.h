/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2005, 2011  Francois Blanchette

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

// FileList.h: interface for the CFileList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PTRLIST_H__C691F4A4_CFFA_11DC_9A69_000E2E209BA0__INCLUDED_)
#define AFX_PTRLIST_H__C691F4A4_CFFA_11DC_9A69_000E2E209BA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileList 
{
public:
        bool Exists(char *pString);
	CFileList(int nGrow=200, bool bSort=true);
	virtual ~CFileList();
        void ReplaceAt(int i, char *p);
	void RemoveAll();
        int InsertAt(int i, char *pFilename);
	bool RemoveAt(int i);
	int GetSize();
	void Debug();
	void GrowTo(int nSize, bool bInit=true);
        int Add(char *pFilename);
        char * operator [](int i);
	CFileList & operator = (CFileList & list);

protected:

        char **m_pArray;
	int m_nGrow;
	int m_nSize;
	int m_nMax;
	int m_bSort;
};

#endif // !defined(AFX_PTRLIST_H__C691F4A4_CFFA_11DC_9A69_000E2E209BA0__INCLUDED_)
