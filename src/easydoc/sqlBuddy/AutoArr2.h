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

#ifndef _CAUTOARR2_H
#define _CAUTOARR2_H

#include <QString>
#include <QChar>
#define _T

class CAutoArr2
{

public:

    CAutoArr2();
    ~CAutoArr2();

    enum { 
        AA_ERROR = -1,
        AA_BLOCK_SIZE = 16
    };

    class AA_ITEM
    {

    public:

        AA_ITEM(QString strName=_T(""), QString strContent=_T(""), int id=0, int fk=0)
        {
            m_strName = strName;
            m_strContent = strContent;
            m_nId = id;
            m_nFK = fk;
        }

        inline AA_ITEM & operator = (QString str)
        {
            m_strContent = str;
            return *this;
        }
  
        inline AA_ITEM & operator = (int i)
        {
            m_strContent = QString::number(i);
            return *this;        
        }

        inline AA_ITEM & operator = (AA_ITEM & item)
        {
            m_strContent = item.m_strContent;
            m_strName = item.m_strName;
            m_nId = item.m_nId;
            m_nFK = item.m_nFK;
            return *this;
        }

        inline bool operator == (AA_ITEM & item)
        {
            return m_strContent == item.m_strContent
                && m_strName == item.m_strName
                && m_nId == item.m_nId
                && m_nFK == item.m_nFK;
        }

        inline bool operator != (AA_ITEM & item)
        {
            return !(*this == item);
        }

        inline bool operator == (QString str)
        {
            return m_strContent == str;
        }

        inline bool operator != (QString str)
        {
            return m_strContent != str;
        }

        inline int GetId()
        {
            return m_nId;
        }

        inline int GetFK()
        {
            return m_nFK;
        }

        inline void SetId(int n)
        {
            m_nId = n;
        }

        inline void SetFK(int n)
        {
            m_nFK = n;
        }

        inline AA_ITEM & Rename(QString str)
        {
            m_strName = str;
            return *this;
        }

        inline AA_ITEM & SetName(QString str)
        {
            m_strName = str;
            return *this;
        }

        inline QString GetName()
        {
            return m_strName;
        }

        inline QString & GetContent()
        {
            return m_strContent;
        }

    protected:
        int m_nId;
        int m_nFK;
        QString m_strName;
        QString m_strContent;

    };

    inline int GetSize() {return m_nSize;}
    bool Exists(QString strName);
    bool Exists(int nId);
    int IndexOf(QString strName);
    int IndexOf(int nId);    
    inline bool IsWithinBounds(int i) {return i>=0 && i<m_nSize;}

    AA_ITEM & Add(AA_ITEM & item);
    AA_ITEM & Add(QString strName=_T(""), QString strContent=_T(""), int nId=0, int nFK=0);
    bool InsertAt(int i, AA_ITEM & item);
    void RemoveAt(int i);
    int Remove(QString str);
    void RemoveAll();
    int Rename(QString strOld, QString strNew);

    QString & GetFormData(QString & str);

    void EnumBlocks();

    AA_ITEM & operator[] (int i);
    AA_ITEM & operator[] (QString str);
    CAutoArr2 & operator = (CAutoArr2 & s);
    CAutoArr2 & operator += (CAutoArr2 & s);
    CAutoArr2 & operator += (AA_ITEM & item);
    QString & operator > (QString & str);
    QString & operator >> (QString & str);
    CAutoArr2 & operator < (QString str);
    CAutoArr2 & operator << (QString str);
      
    static void SetAutoCreate(bool b)
    {
        m_bAutoCreate = b != 0;
    }

private:

    class AA_BLOCK
    {
    public:

        AA_BLOCK() 
        { 
            Init(); 
        }

        inline int GetSize() 
        { 
            return m_nSize; 
        }

        inline AA_BLOCK * GetNext() 
        {
            return m_pNext;
        }

        inline void SetNext(AA_BLOCK *p)
        {
            m_pNext = p;
        }

        inline AA_ITEM & operator [] (int i)
        {
            return m_items[i];
        }

        inline AA_ITEM & Add(AA_ITEM & item)
        {
            if (GetSize() != AA_BLOCK_SIZE)
            {
                AA_ITEM & t = (*this) [GetSize()] = item;
                m_nSize++;

                return t;
            }
            else
            {
                return m_items[0];
            }
        }

        inline AA_ITEM & Add()
        {
            if (GetSize() != AA_BLOCK_SIZE)
            {
                AA_ITEM & t = (*this) [GetSize()];
                m_nSize++;

                return t;
            }
            else
            {
                return m_items[0];
            }
        }

        inline int Remove(int i=1)
        {
            m_nSize = qMax(m_nSize - i, 0);
            return m_nSize;
        }

        inline void Init()
        {
            m_nSize = 0; 
            m_pNext = NULL;
        }

    protected:
        
        int m_nSize;
        AA_ITEM m_items[AA_BLOCK_SIZE];
        AA_BLOCK *m_pNext; // next block    
    };

    friend class AA_ITEM;
    friend class AA_BLOCK;

    AA_BLOCK * FindBlock(int i, int & nBase);
    void Init();
    void Forget(); // legacy

    static int str2int(QString);
    static int CountEGBChars(QChar *pBuf, bool bTotal=FALSE);
    static QChar * EGBEncode(QChar *s, QChar *t, bool bRemove=FALSE);
    static QString & EGBEncode(QString strSrc, QString & strTarget, bool bRemove=FALSE);

    static int CountURLSpecialChars(QChar *pBuf, bool bTotal=FALSE);
    static QString & URLEncode(QString strSource, QString & strTarget);
    static QString EGBDecode(const QString str);
                 
protected:
    int m_nSize;
    AA_BLOCK m_block;
    AA_BLOCK *m_pActive;

    static bool m_bAutoCreate;
    static AA_ITEM m_aaitem;
};

#endif
