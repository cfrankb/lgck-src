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

#ifndef FOLDERS_H
#define FOLDERS_H

#include "stdafx.h"
#include <string>

class CFileWrap;
class ISerial;

class CFolder
{
public:
    CFolder();
    ~CFolder();

    class CFileEntry
    {
    public:
        CFileEntry(){m_offset = -1 ; m_size = 0;}
        const char * getName() { return m_name.c_str(); }
        int getOffset() { return m_offset; }
        int getSize() { return m_size; }

    protected:
        bool writeFileIndex (CFileWrap &file)
        {
            char t [MAX_FILENAME + 1];
            memset(t, 0, MAX_FILENAME + 1);
            memccpy(t, m_name.c_str(), 0, MAX_FILENAME);

            file.write(t, MAX_FILENAME);
            file << m_offset;
            file << m_size;

            return true;
        }

        bool readFileIndex (CFileWrap &file)
        {
            char t [MAX_FILENAME + 1];
            memset(t, 0, MAX_FILENAME + 1);

            file.read(t, MAX_FILENAME);
            m_name = t;
            file >> m_offset;
            file >> m_size;

            return true;
        }

        CFileEntry & operator = (CFileEntry & s) {
            m_name = s.getName();
            m_offset = s.getOffset();
            m_size = s.getSize();
            return *this;
        }

    protected:

        std::string m_name;
        int m_offset;
        int m_size;

        friend class CFolder;
    };

    enum {
        MAX_NAME           = 12,
        MAX_FILENAME       = 32,
        MAX_FILES          = 10,
        FILELIST_HEADER_SIZE = 8,
        FILELIST_ENTRY_SIZE = MAX_FILENAME + 8,
        GROW_BY             = 5
    };

    const char * getName() { return m_name.c_str(); }
    void setName(const char *name) { m_name = name;}
    const CFolder operator = (CFolder & s);
    CFolder::CFileEntry * operator [] (const char * fileName);
    CFolder::CFileEntry * operator [] (int i);
    int getSize() { return m_size; }

    CFileEntry & addFile(const char * name, int offset, int size=-1);

protected:
    std::string m_name;
    int m_offset;
    int m_size;
    int m_max;
    CFileEntry *m_files ;

    bool writeFolderIndex(CFileWrap & file);
    bool readFolderIndex(CFileWrap & file);
    bool writeFileIndex(CFileWrap & file);
    bool readFileIndex(CFileWrap & file);
    int getOffset() { return m_offset; }
    void setOffset(int offset) { m_offset = offset;}

friend class CFolders;
};

class CFolders
{

public:
    CFolders();
    ~CFolders();

    CFileWrap & getFile() { return m_file; }
    int getSize() { return m_fileSize; }

    CFolder * operator [] (const char* name);
    int operator += (const int size) { m_fileSize += size; return m_fileSize; }
    bool open(const char * fileName, bool create=false);

    CFolder & addFolder(const char* name, int offset = -1);
    void addFile(CFolder & folder, const char * srcfileName, const char * fileName);

    bool writeHeader();
    bool readHeader();
    bool writeFolderIndex();
    bool readFolderIndex();
    bool writeFileIndex();
    bool readFileIndex();
    bool writeFile(CFileWrap & file, ISerial & serial, CFolder &folder, const char *name);

    typedef struct {
        CFileWrap file;
        int size;
        bool internal;
    } FileInfo;

    CFolder::CFileEntry * checkOut(const char *filePath);

protected:

    CFileWrap m_file;

    int m_version;
    int m_fileSize;
    int m_offset;
    int m_size;
    int m_max;

    enum {
        HEADER_SIZE         = 16,
        INDEX_HEADER_SIZE   =  8,
        FOLDER_ENTRY_SIZE   = CFolder::MAX_FILENAME + 4, //16,
        GROWBY              = 5
    };

    CFolder *m_folders;

    friend class CFolder;
};

#endif // FOLDERS_H
