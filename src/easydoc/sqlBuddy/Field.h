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

// Field.h : header file
//

#ifndef _FIELD_H
#define _FIELD_H

#include <QString>
#include <QChar>
#include "TextArray.h"

/////////////////////////////////////////////////////////////////////////////
// CField

class CField 
{
// Construction
public:
	void FindFriends(char *sz);
	CTextArray & GetFriends();
	CField();

// Attributes
public:
        QString GetDefault() const;
        QString GetArgs() const;
        QString GetInfo() const;
        QString GetName() const;
        qint32 GetType();

        void SetInfo(QString strInfo);
        void SetDefault(QString strDefault);
        void SetArgs(QString strArgs);
        void SetName(QString strName);
        void SetAuto(bool bAuto);
        void SetNotNull(bool bNotNull);
        void SetUnsigned(bool bUnsigned);
	void SetType(int nType);

        bool IsAlias();
        bool IsKey();
        bool IsPrimary();
        bool IsUnique();

        enum {
            FI_ALIAS_PRIMARY = 0x100,
            FI_ALIAS_UNIQUE = 0x101,
            FI_ALIAS_KEY = 0x102,
            MAX_FI_NAME = 32,
            MAX_FI_DEFAULT = 16,
            MAX_FI_SIZE =4,
            MAX_FI_ARG =32,
            MAX_FI_INFO =1024
        };

        typedef struct
        {
            // DO NOT MODIFY THIS BLOCK
            char m_szName[MAX_FI_NAME];
            char m_szDefault[MAX_FI_DEFAULT];
            char m_szArg[MAX_FI_ARG];
            qint32 m_nType;
            qint32 m_nSize;

            qint32 m_bPrimary;
            qint32 m_bNotNull;
            qint32 m_bAuto;
            qint32 m_bKey;
            qint32 m_bUnique;
            qint32 m_bUnsigned;

            qint32 m_pNext;
            // ***************************
        } FIELD_INFO;

        static QString m_arrTypes [];
        static QString m_arrTypeAlias[];

// Operations
public:
        QString techDoc() const;
        const QString formated() ;
	void Forget();
        QString readString(FILE *sfile) const;
        void writeString(FILE *tfile, const QString str);

        void read(FILE *, qint32);
        void write(FILE *);
	void Init();

protected:
        qint32 findType(const QString);

// Implementation
public:
	 ~CField();

         bool m_bNotNull;
         bool m_bAuto;
         bool m_bUnsigned;

	 // legacy vars maintained
	 // for backward compatibility
         bool m_bPrimary;
         bool m_bKey;
         bool m_bUnique;

 	 CField *m_pNext;

protected:

         QString m_strName;
         QString m_strArgs;
         QString m_strDefault;
         QString m_strInfo;

	 CTextArray m_arrFriends;
         qint32 m_nType;
};

/////////////////////////////////////////////////////////////////////////////

#endif
