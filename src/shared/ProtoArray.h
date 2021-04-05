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

#ifndef _PROTOARRAY_H
#define _PROTOARRAY_H

#include "LevelEntry.h"
#include "Object.h"
#include "ISerial.h"
#include <unordered_map>

// ProtoArray.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProtoArray 

class CProtoIndex;
class IFile;

class CProtoArray: public ISerial
{

// Construction
public:
        virtual bool read (IFile & file);
        virtual bool write (IFile & file);
        CProto & getProto(CLevelEntry & entry);
        int getProtoIdFromUuid (const char* uuid);
        CProtoArray();

        typedef std::unordered_map<std::string, std::string> FixUpBlock;
        typedef std::unordered_map<std::string, FixUpBlock> FixUpTable;

// Attributes
public:
        int getSize() const;
        int getProtoIndex (const char* str);
        static int getEventCount();
        static const char * getEventName(int i);

// Operations
public:
        int add(const CProto proto);
        void forget();
        void removeAt (int n);
        bool exportMeta(IFile & file, int i);
        bool importMeta(IFile & file, FixUpTable *table=nullptr);
        CObject & getObject(int i);
        CProto & operator [] (int n);
        CProtoArray & operator += (CProtoArray & src);
        CProtoArray & operator = (CProtoArray & src);
        CProto & get(int i) const;
        int countSpriteOfGivenClass(int spriteClass);
        int countAutoGoals();
        void debug();

        // remove references
        void killProto (int nProto);
        void killFrameSet (int nFrameSet);
        void killSound (int nSound);

        // index
        void indexArray();
        int findIndexPos (int protoId);
        int getAtIndex( int i );
        void removeIndex (int pos );
        void removeFromIndex (int protoId);
        int insertIndex(int protoId);
        bool isIndexed();
        void resizeIndex(int newSize);
        void debugIndex();
        int getIndexSize();
        int indexOfUUID(const char *uuid);

        CProtoIndex * createIndex(int pattern = 0, const char *search=nullptr);

// Implementation
public:
	~CProtoArray();

protected:
        void forgetIndex();
        bool readEx (IFile & file, int version);
        void fixUUIDs();
        void writeFixUpTable(IFile &file, CProtoArray & s);
        bool readFixUpTable(IFile &file, FixUpTable & table);

        int *m_index;
        int m_indexSize;

        CObject * m_objects;
        int m_nMax;
        int m_nSize;
        static std::string m_eventList[];
        static CProto m_protoTmp;
        enum {
            PROTO_VERSION = 0x4,
            GROWBY = 16,
            NOT_FOUND = -1
        };
};

#endif
