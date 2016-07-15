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

// FileList.cpp: implementation of the CFileList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileList.h"
#include <stdio.h>
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
CFileList::CFileList(int nGrow, bool bSort)
{
	m_nGrow = nGrow;// max(nGrow, 1);
        m_pArray = new char *[m_nGrow];
	m_nSize = 0;
	m_bSort = bSort;
	m_nMax = m_nGrow;
}

CFileList::~CFileList()
{
	// perform final clean up

	RemoveAll();
	delete [] m_pArray;
	m_pArray = NULL;
	m_nMax = 0;
}

int CFileList::Add(char *pFilename)
{
	int i = 0;

	if (m_bSort)
	{
		// if this array is sorted, find where the new
		// node should be placed

		int min = 0;
		int max = m_nSize - 1;

		while (max >= min)
		{
			i = min + (max - min) / 2;
			int result = strcmp(m_pArray[i] , pFilename);
			if (result)
			{
				if (result < 0)
				{
					min = i + 1;
				}
				else
				{			
					max = i - 1;
				}
			}
			else
			{
				return i;
			}
		}

		if (m_nSize && (strcmp(m_pArray[i], pFilename) <= 0))
		{
			i++;
		}
	}
	else
	{
		// if this array is unsorted simply append the new node
		// at the end

		i = m_nSize;
	}
	
	return InsertAt(i, pFilename);
}

char *CFileList::operator [](int i)
{
//	ASSERT ( (i>=0) && (i<m_nSize) );
	return m_pArray[i];
}

int CFileList::GetSize()
{
	return m_nSize;
}

bool CFileList::RemoveAt(int i)
{
//	ASSERT ( (i>=0) && (i<m_nSize) );

	// remove a string from the vector
	delete m_pArray[i];

	// move up the other string to fill the gap

	for (int x = i; x < (m_nSize - 1); x++)	{
//		printf("x=%d %x %s\n", x, m_pArray[x + 1], m_pArray[x + 1]);
		m_pArray[x] = m_pArray[x + 1];
	}

	m_nSize--;

	return true;
}

int CFileList::InsertAt(int i, char *pFilename)
{
//	ASSERT(i < m_nMax );

	// move up all the displaced entries 

	for (int x = m_nSize; x > i; x--)
	{
		m_pArray[x] = m_pArray[x - 1];
	}

	// insert the new node

        m_pArray[i] = new char[ strlen(pFilename) + 1];
	strcpy(m_pArray[i], pFilename);
	m_nSize++;

	// if the vector is full, simply allocate more
	// memory

	if (m_nSize == m_nMax)
	{
		m_nMax += m_nGrow ;
                char **p = m_pArray;
                m_pArray = new  char *[m_nMax];
                memcpy(m_pArray, p, m_nSize * sizeof(char*));
		delete p;
	}

	return i;
}

void CFileList::RemoveAll()
{
	// empty the vector

	for (int i=0; i < m_nSize; i++)
	{
		delete m_pArray[i];
		m_pArray[i] = NULL;
	}

	m_nSize =0;
}

void CFileList::GrowTo(int nSize, bool bInit)
{
	// increase vector size, if needed

	if (nSize > m_nMax)
	{
		m_nMax += nSize + 1 ;
                char **p = m_pArray;
                m_pArray = new  char *[m_nMax];
                memcpy(m_pArray, p, m_nSize * sizeof(char*));
		delete p;
	}

	// intialize extra elements to blank

	if (bInit)
	{
		for (int i=m_nSize; i < nSize; i++)
		{
                        m_pArray[i] = new char[1];
			m_pArray[i][0] = 0;
		}
	}

	m_nSize = nSize;
}

void CFileList::ReplaceAt(int i, char *p)
{
//	ASSERT( (i >= 0) && (i < m_nSize) );
	
	delete m_pArray[i];
        m_pArray[i] = new char [strlen(p) +1];
	strcpy(m_pArray[i], p);
}

CFileList & CFileList::operator =(CFileList &list)
{
//	ASSERT(this);
	RemoveAll();

	for (int i=0; i<list.GetSize(); i++)
	{
		InsertAt(i, list[i]);
	}

	return *this;
}


bool CFileList::Exists(char *pString)
{
	for (int i=0; i < m_nSize; i++)
	{
		if (strcmp(m_pArray[i], pString)==0)
		{
			return true;
		}
	}

	return false;
}


void CFileList::Debug()
{
	printf("size: %d; max= %d\n", m_nSize, m_nMax);

	for (int i=0; i < m_nSize; i++)
	{
		printf("%s\n", m_pArray[i]);
	}
}
