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

// MSBData.cpp : implementation file
//

#include "MSBData.h"
#include <QDebug>

/////////////////////////////////////////////////////////////////////////////
// CMSBData

CMSBData::CMSBData()
{
    Init();
}

CMSBData::~CMSBData()
{
    Forget();
}

/////////////////////////////////////////////////////////////////////////////
// CMSBData

CTable * CMSBData::operator[] (int n)
{
    if (n<0 || n>=m_nSize) {
        return NULL;
    }
    else
    {
        if (!m_pHead) {
            return NULL;
        } else {
            CTable *p = m_pHead;
            while (n && p) {
                p = p->m_pNext;
                n --;
            }

            return p;
        }
    }
}

void CMSBData::Init()
{
    m_pHead =NULL ;
    m_pTail = NULL;
    m_nSize=0;
    m_nSelTable = -1;
    memset(m_szName, 0, DB_MAX_NAME);
}

void CMSBData::Forget()
{
    CTable * p = m_pHead;

    while (p) {
        CTable * pp = p;
        p = p -> m_pNext;
        delete pp;
    }

    m_pHead =NULL;
    m_pTail = NULL;
    m_nSize=0;
}

bool CMSBData::read(FILE *sfile)
{
    char szTemp[]="****";
    int nVer = VERSION;

    Forget();
    int nSize=0;
    //ar.Read(szTemp,4);
    fread(szTemp, 4, 1, sfile);
    if (memcmp(szTemp, "MSBD", 4))
    {
        qDebug() << "unsupported format\n";
        return false;
    }

    //ar.Read(&nVer,4);
    fread(&nVer, 4, 1, sfile);
    if (nVer > VERSION) {
        qDebug() << "unsupported version\n";
        return false;
    }

    //ar.Read(&nSize,4);
    //ar.Read(m_szName, MAX_TB_NAME);
    fread(&nSize,4, 1, sfile);
    fread(m_szName, DB_MAX_NAME, 1, sfile);

    if (m_szName[0]==-51) {
        m_szName[0]=0;
    }

    CTable *p = NULL;
    m_pHead = NULL;

    for (int i=0; i<nSize; i++) {
        p = new CTable;
        p->Init();
        //p->Serialize(ar, nVer);
        p->read(sfile, nVer);
        Add(p);
    }

    return true;
}

bool CMSBData::write(FILE *tfile)
{
    int nVer = VERSION;

    //ar.Write("MSBD",4);
    //ar.Write(&nVer,4);
    //ar.Write(&m_nSize, 4);
    //ar.Write(m_szName, DB_MAX_NAME);

    fwrite("MSBD", 4, 1, tfile);
    fwrite(&nVer, 4, 1, tfile);
    fwrite(&m_nSize, 4, 1, tfile);
    fwrite(m_szName, DB_MAX_NAME,1, tfile);

    CMSBData & mother = (*this);

    for (int i=0; i<m_nSize; i++) {
        CTable *p = mother[i];
        if (p) {
            p->write(tfile);
        }
    }

    return true;
}

bool CMSBData::Add(CTable * p)
{
    if (m_pTail) {
        m_pTail -> m_pNext = p;
    }

    if (!m_nSize) {
        m_pHead = p;
    }

    m_pTail= p;
    p -> m_pNext = NULL;
    m_nSize++;

    return TRUE;
}

int CMSBData::GetSize()
{
    return m_nSize;
}

bool CMSBData::RemoveAt(int n)
{
    if (m_pHead && m_pTail && !(n<0) && !(n>=m_nSize)) {
        if (n==0) {
            CTable *p = m_pHead;
            m_pHead = m_pHead->m_pNext;
            m_nSize--;
            delete (p);

            if (!m_pHead) {
                m_pTail = NULL;
            }
        }
        else {
            CMSBData & mother = (*this);
            CTable *p = mother[n];
            if (p) {
                if (p == m_pTail) {
                    CTable *pp = mother[n-1];
                    m_pTail = pp;
                    pp->m_pNext = NULL;
                    delete p;
                } else {
                    CTable *pp = mother[n-1];
                    pp->m_pNext = mother[n+1];
                    delete p;
                }
                m_nSize--;
            }
        }

        return TRUE;
    } else {
        return FALSE;
    }
}
