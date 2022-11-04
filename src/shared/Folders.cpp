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

#include "stdafx.h"
#include <string.h>
#include "../shared/FileWrap.h"
#include "Folders.h"
#include "ISerial.h"

CFolders::CFolders()
{
    m_version = 0;
    m_fileSize = 0;
    m_offset = -1;
    m_size = 0;
    m_max = GROWBY;
    m_folders = new CFolder[m_max];
    m_file = nullptr;
}

CFolders::~CFolders()
{
    if (m_folders) {
        delete [] m_folders;
    }
}

CFolder * CFolders::operator [] (const char * name)
{
    for (int i=0; i < m_size; ++i) {
        if (strcmp(m_folders[i].getName(),  name)==0) {
            return &m_folders[i];
        }
    }

    return nullptr;
}

bool CFolders::open(IFile *file, bool create)
{
    m_file = file;
    if (create) {
        m_offset = -1;
        m_fileSize = HEADER_SIZE;
        return writeHeader();
    } else {
        return readHeader() && readFolderIndex() && readFileIndex();
    }

    return false;
}

bool CFolders::writeHeader()
{
    // main header
    IFile & file = *m_file;
    file.seek(0);

    file.write("VFF1", 4);
    file << m_version;
    file << m_fileSize;
    file << m_offset;

    return true;
}

bool CFolders::readHeader()
{
    // main header
    IFile & file = *m_file;

    file.seek(0);
    char signature[5];

    file.read(signature, 4);
    file >> m_version;
    file >> m_fileSize;
    file >> m_offset;
    return true;
}

bool CFolders::writeFolderIndex()
{
    // folderIndexHeader
    if (m_offset == -1) {
        m_offset = m_fileSize;
        m_fileSize += INDEX_HEADER_SIZE + m_max * FOLDER_ENTRY_SIZE;
    }

    IFile & file = *m_file;
    file.seek(m_offset);
    file << m_size;
    file << m_max;

    // folderIndexEntry
    for (int i=0; i < m_max; ++i) {
        m_folders[i].writeFolderIndex(*m_file);
    }

    return true;
}

bool CFolders::readFolderIndex()
{
    // folderIndexHeader
    if (m_offset == -1) {
        // no index has been defined yet
        return true;
    }

    IFile & file = *m_file;
    file.seek(m_offset);
    file >> m_size;
    int max;
    file >> max;

    // allocate more memory if needed
    if (max > m_max) {
        CFolder *t = new CFolder[max];
        delete [] m_folders;
        m_folders = t;
    }

    m_max = max;

    // folderIndexEntry
    for (int i=0; i < m_max; ++i){
        m_folders[i].readFolderIndex(*m_file);
    }

    return true;
}

bool CFolders::writeFileIndex()
{
    // for each folder
    for (int i=0; i < m_max; ++i) {
        // if indexOffset is undefined
        // or otherwise invalidated
        if ( m_folders[i].m_offset == -1) {
            // create a new index
            m_folders[i].m_offset = m_fileSize;
            m_fileSize += CFolder::FILELIST_HEADER_SIZE + m_folders[i].m_max * CFolder::FILELIST_ENTRY_SIZE;
        }

        m_folders[i].writeFileIndex(*m_file);
    }

    return true;
}

bool CFolders::readFileIndex()
{
    // for each folder
    for (int i=0; i < m_max; ++i) {
        // if indexOffset is undefined
        // or otherwise invalidated
        if ( m_folders[i].m_offset != -1) {
            // create a new index
            m_folders[i].readFileIndex(*m_file);
        }
    }

    return true;
}


CFolder & CFolders::addFolder(const char *name, int offset)
{
    // add folder to folderIndex
    int i = m_size;
    if (m_size == m_max) {
        CFolder *t = new CFolder[m_max];
        for (i=0; i<m_size; ++i) {
            t[i] = m_folders[i];
        }
        delete [] m_folders;
        m_folders = t;
    }

    m_folders[i].setName(name);
    m_folders[i].setOffset(offset);
    m_size++;
    return m_folders[i];
}

void CFolders::addFile(CFolder & folder, const char* srcfileName, const char* fileName)
{
    long size = 0;
    CFileWrap file;
    if (srcfileName[0]) {
        if (file.open(srcfileName, "rb")) {
            size = file.getSize();
            char *buffer = new char[size];
            file.read(buffer, size);
            file.close();
            m_file->seek(m_fileSize);
            m_file->write(buffer, size);
            delete [] buffer;
        }
    }

    folder.addFile(fileName, m_fileSize, size);

    m_fileSize += size;
}


CFolder::CFileEntry * CFolders::checkOut(const char * filePath)
{
    char *t = new char[strlen(filePath) + 1];
    strcpy(t, filePath);

    std::string folderName;
    std::string fileName;

    char *p = strstr(t, "/");
    if (p) {
        *p = 0;
        folderName = t;
        fileName = p + 1;
    }
    else  {
        fileName = t;
    }
    delete [] t;

    CFolder * folder = (*this)[folderName.c_str()];
    if (folder) {
        return (*folder)[fileName.c_str()] ;
    } else {
        return nullptr;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CFolder

CFolder::CFolder()
{
    m_size =0;
    m_max = MAX_FILES;
    m_offset  = -1;
    m_files = new CFileEntry[m_max];
}

CFolder::~CFolder()
{
    if (m_files) {
        delete [] m_files;
        m_files = nullptr;
    }
}

bool CFolder::writeFolderIndex(IFile & file)
{
    char t [MAX_NAME + 1];
    memset( t, 0, MAX_NAME + 1);
    memccpy(t, m_name.c_str(), 0, MAX_NAME);

    file.write(t, MAX_NAME);
    file << m_offset;
    return true;
}

bool CFolder::readFolderIndex(IFile & file)
{
    char t [MAX_NAME + 1];
    memset( t, 0, MAX_NAME + 1);

    file.read(t, MAX_NAME);
    m_name = t;
    file >> m_offset;
    return true;
}

const CFolder &CFolder::operator =(CFolder & s)
{
    m_name = s.getName();
    m_offset = s.getOffset();
    return *this;
}

bool CFolder::writeFileIndex(IFile &file)
{
    // fileIndexHeader
    file.seek(m_offset);
    file << m_size;
    file << m_max;

    // fileIndexEntry
    for (int i=0; i < m_max; ++i) {
        m_files[i].writeFileIndex(file);
    }

    return true;
}

bool CFolder::readFileIndex(IFile &file)
{
    // fileIndexHeader
    file.seek(m_offset);
    file >> m_size;
    int max;
    file >> max;

    // allocate more memory if needed
    if (max > m_max) {
        CFileEntry *p = new CFileEntry [max];
        delete [] m_files;
        m_files=p ;
    }

    m_max = max;

    // fileIndexEntry
    for (int i=0; i < m_max; ++i) {
        m_files[i].readFileIndex(file);
    }

    return true;
}

CFolder::CFileEntry & CFolder::addFile(const char* name, int offset, int size)
{
    int i = m_size;
    if (m_size == m_max) {
        m_offset = -1;
        m_max += GROW_BY;
        CFileEntry *p = new CFileEntry [m_max];
        for (i=0; i<m_size; i++){
            p[i] = m_files[i];
        }

        delete [] m_files;
        m_files=p ;
    }

    m_files[i].m_name = name;
    m_files[i].m_size = size;
    m_files[i].m_offset = offset;

    m_size++;
    return m_files[i];
}

CFolder::CFileEntry * CFolder::operator [] (const char* fileName)
{
    for (int i=0; i < m_size; ++i) {
        if (!m_files[i].m_name.compare(fileName)) {
            return &m_files[i];
        }
    }

    return nullptr;
}

CFolder::CFileEntry * CFolder::operator [] (int i)
{
    if ( i <0 || i >= m_size) return nullptr;
    return & m_files[i];
}

bool CFolders::writeFile(IFile & file, ISerial & serial, CFolder & folder, const char *name)
{
    int bSize = getSize();
    file.seek(bSize);
    serial.write(file);
    int aSize = file.getSize();
    folder.addFile(name, bSize, aSize - bSize);
    m_fileSize += (aSize - bSize);
    return true;
}

///////////////////////////////////////////////////////////////////////////
// CFileEntry

bool CFolder::CFileEntry::writeFileIndex (IFile &file)
{
    char t [MAX_FILENAME + 1];
    memset(t, 0, MAX_FILENAME + 1);
    memccpy(t, m_name.c_str(), 0, MAX_FILENAME);

    file.write(t, MAX_FILENAME);
    file << m_offset;
    file << m_size;

    return true;
}

bool CFolder::CFileEntry::readFileIndex (IFile &file)
{
    char t [MAX_FILENAME + 1];
    memset(t, 0, MAX_FILENAME + 1);

    file.read(t, MAX_FILENAME);
    m_name = t;
    file >> m_offset;
    file >> m_size;

    return true;
}

CFolder::CFileEntry & CFolder::CFileEntry::operator = (CFileEntry & s) {
    m_name = s.getName();
    m_offset = s.getOffset();
    m_size = s.getSize();
    return *this;
}
