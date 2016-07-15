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

// Table.h : header file
//

#ifndef _TABLE_H
#define _TABLE_H

#include "Field.h"

/////////////////////////////////////////////////////////////////////////////
// CTable

class CTable 
{
    // Construction
public:
    int FindFirstAlias();
    void InsertAt(CField * pField, int i);
    void FixLegacy();
    QString & GetName();
    void SetName (QString strName);
    CField * FindField(const QString);
    const QString techDoc();
    void GetWndXY(int & nX, int &nY);
    void GetWndSize(int & nLen, int &nHei);
    void SetWndXY(int nX, int nY);
    void SetWndSize(int nLen, int nHei);
    void SetWndTable(void *pWnd);
    void * GetWndTable();
    CTable();

    // Attributes
public:
    int GetSize();
    enum {
        MAX_TB_NAME = 32,
        MAX_TB_INFO = 1024
                  };

    // Operations
public:
    QString sql(const QString tableName="");
    QString readString(FILE *sfile) const;
    void writeString(FILE *tfile, const QString str);
    void read(FILE *, qint32);
    void write(FILE *);
    void Forget();
    void Init();
    CField * operator[] (int n);
    bool RemoveAt(int n);
    bool Add(CField * p);

    // Implementation
public:
    ~CTable();

    QString m_strName;

    CTable *m_pNext;
    CField *m_pHead;
    CField *m_pTail;

    char m_szInfo[MAX_TB_INFO];

protected:
    qint32 m_nSize;

    int m_nX;
    int m_nY;
    int m_nLen;
    int m_nHei;

    void *m_pWnd;
};

/////////////////////////////////////////////////////////////////////////////
#endif
