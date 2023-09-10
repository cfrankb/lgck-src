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

#ifndef SECTIONS_H
#define SECTIONS_H

#include <QString>
class QFileWrap;

typedef struct
{
    QString name;
    QString content;
} Section;

class CSections
{
public:
    CSections();
    ~CSections();

    Section & operator [] (int i);
    int getSize() {
        return m_sCount;
    }

    int add (Section & cl);
    bool read(QFileWrap & file, int version);
    bool write(QFileWrap & file);
    void removeAt(int i);
    void removeAll();

protected:

    enum {
        GROWBY = 20
    };

    Section * m_sections;

    int m_sCount;
    int m_sMax;
};

#endif // SECTIONS_H
