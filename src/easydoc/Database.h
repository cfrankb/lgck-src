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

#ifndef DATABASE_H
#define DATABASE_H

#include "Functions.h"
#include "Params.h"
#include "Classes.h"
#include "Sections.h"

class CDatabase
{
public:
    CDatabase();
    ~CDatabase();

    bool isDirty() {
        return m_dirty;
    }

    void setDirty(bool b){
        m_dirty = b;
    }

    bool isUntitled() {
        return m_fileName.isEmpty();
    }

    void removeAll();
    bool read();
    bool write();
    void dump(CFileWrap & file);
    void exportList(CFileWrap & file);
    void exportText(CFileWrap & file);
    void exportWiki(const QString &path);
    void importGameLua(const char *cdata);

    void setFileName(const QString fileName);
    QString getFileName();
    const QString getLastError() {
        return m_lastError;
    }

    CFunctions m_functions;
    CClasses m_classes;
    CSections m_sections;

protected:

    enum {
        VERSION = 0x0005
    };

    static char m_signature[];
    bool m_dirty;
    QString m_fileName;
    QString m_lastError;
};

#endif // DATABASE_H
