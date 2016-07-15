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

// TextArray.h : header file
//

#ifndef _CTEXTARRAY
#define _CTEXTARRAY

#include <QString>
#include <QChar>

/////////////////////////////////////////////////////////////////////////////
// CTextArray

class CTextArray 
{
    // Construction
public:
    void RemoveAt(int i);
    CTextArray & operator = (CTextArray & src);
    CTextArray();

    // Attributes
public:
    int GetSize();

    // Operations
public:
    QString & operator[] (int n);
    bool Add(const QString str);
    void Forget();
    void Init();

    QString readString(FILE *sfile) const;
    void writeString(FILE *tfile, const QString str);
    void read(FILE*);
    void write(FILE*);

    // Implementation
public:
    ~CTextArray();

protected:

    class CItem
    {
    public:
        CItem (QString str="", CItem *pPrev=NULL, CItem *pNext=NULL){m_str=str; m_pPrev=pPrev; m_pNext=pNext;}
        QString m_str;
        CItem *m_pPrev;
        CItem *m_pNext;
    };

protected:
    CItem *m_pHead;
    CItem *m_pTail;
    int m_nSize;

};

/////////////////////////////////////////////////////////////////////////////
#endif
