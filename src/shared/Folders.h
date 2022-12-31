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

#include <string>

class ISerial;
class IFile;

class CFolder
{
public:
    CFolder();
    ~CFolder();

    class CFileEntry
    {
    public:
        CFileEntry()
        {
            m_offset = -1;
            m_size = 0;
        }
        const char *getName() { return m_name.c_str(); }
        int getOffset() { return m_offset; }
        int getSize() { return m_size; }

    protected:
        bool writeFileIndex(IFile &file);
        bool readFileIndex(IFile &file);
        CFileEntry &operator=(CFileEntry &s);

    protected:
        std::string m_name;
        int m_offset;
        int m_size;

        friend class CFolder;
    };

    enum
    {
        MAX_NAME = 12,
        MAX_FILENAME = 32,
        MAX_FILES = 10,
        FILELIST_HEADER_SIZE = 8,
        FILELIST_ENTRY_SIZE = MAX_FILENAME + 8,
        GROW_BY = 5
    };

    const char *getName() { return m_name.c_str(); }
    void setName(const char *name) { m_name = name; }
    const CFolder &operator=(CFolder &s);
    CFolder::CFileEntry *operator[](const char *fileName);
    CFolder::CFileEntry *operator[](int i);
    int getSize() { return m_size; }

    CFileEntry &addFile(const char *name, int offset, int size = -1);

protected:
    std::string m_name;
    int m_offset;
    int m_size;
    int m_max;
    CFileEntry *m_files;

    bool writeFolderIndex(IFile &file);
    bool readFolderIndex(IFile &file);
    bool writeFileIndex(IFile &file);
    bool readFileIndex(IFile &file);
    int getOffset() { return m_offset; }
    void setOffset(int offset) { m_offset = offset; }

    friend class CFolders;
};

class CFolders
{

public:
    CFolders();
    ~CFolders();

    IFile &getFile() { return *m_file; }
    int getSize() { return m_fileSize; }

    CFolder *operator[](const char *name);
    int operator+=(const int size)
    {
        m_fileSize += size;
        return m_fileSize;
    }
    bool open(IFile *file, bool create = false);

    CFolder &addFolder(const char *name, int offset = -1);
    void addFile(CFolder &folder, const char *srcfileName, const char *fileName);

    bool writeHeader();
    bool readHeader();
    bool writeFolderIndex();
    bool readFolderIndex();
    bool writeFileIndex();
    bool readFileIndex();
    bool writeFile(IFile &file, ISerial &serial, CFolder &folder, const char *name);

    CFolder::CFileEntry *checkOut(const char *filePath);

protected:
    IFile *m_file;

    int m_version;
    int m_fileSize;
    int m_offset;
    int m_size;
    int m_max;

    enum
    {
        HEADER_SIZE = 16,
        INDEX_HEADER_SIZE = 8,
        FOLDER_ENTRY_SIZE = CFolder::MAX_FILENAME + 4, // 16,
        GROWBY = 5
    };

    CFolder *m_folders;

    friend class CFolder;
};

#endif // FOLDERS_H
