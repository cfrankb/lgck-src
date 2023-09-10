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

#include "Functions.h"
#include "Sections.h"
#include "../shared/qtgui/qfilewrap.h"

CSections::CSections()
{
    m_sMax = GROWBY;
    m_sCount = 0;
    m_sections = new Section[m_sMax];
}

CSections::~CSections()
{
    if (m_sections)
    {
        delete[] m_sections;
    }
}

Section &CSections::operator[](int i)
{
    return m_sections[i];
}

int CSections::add(Section &section)
{
    m_sections[m_sCount] = section;
    ++m_sCount;
    return m_sCount - 1;
}

bool CSections::read(QFileWrap &file, int version)
{
    m_sCount = 0;
    if (version >= 3)
    {
        int count;
        file >> count;
        for (int i = 0; i < count; ++i)
        {
            Section &s = m_sections[i];
            std::string tmp;
            file >> tmp;
            s.name = tmp.c_str();
            file >> tmp;
            s.content = tmp.c_str();
            ++m_sCount;
        }
    }

    return true;
}

bool CSections::write(QFileWrap &file)
{
    file << m_sCount;
    for (int i = 0; i < m_sCount; ++i)
    {
        Section &s = m_sections[i];
        file << s.name.toStdString();
        file << s.content.toStdString();
    }

    return true;
}

void CSections::removeAt(int i)
{
    for (; i < m_sCount - 1; ++i)
    {
        m_sections[i] = m_sections[i + 1];
    }
    --m_sCount;
}

void CSections::removeAll()
{
    m_sCount = 0;
}
