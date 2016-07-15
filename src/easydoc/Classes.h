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

#ifndef CLASSES_H
#define CLASSES_H

//#include "struct.h"
//#include "class.h"
#include "Class.h"
//#include "FileWrap.h"
class CFileWrap;

class CClasses
{
public:
    CClasses();
    ~CClasses();

    CClass * operator [] (int i);
    int getSize() {
        return m_cCount;
    }

    int add (CClass * cl, bool sorted);
    int insertAt(int i, CClass *c);
    CClass * removeAt(int i);
    bool read(CFileWrap & file, int version);
    bool write(CFileWrap & file);
    void dump(CFileWrap & file);
    void exportList(CFileWrap & file);
    void exportWiki(const QString & path, CFunctions *fct);
    void exportText(CFileWrap & file);
    void debug(CFileWrap & file);
    void removeAll();

protected:

    enum {
        GROWBY = 20
    };

    CClass ** m_classes;

    int m_cCount;
    int m_cMax;
};

#endif // CLASSES_H
