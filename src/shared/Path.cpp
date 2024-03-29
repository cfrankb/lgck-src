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

#include "Path.h"
#include "../shared/IFile.h"
#include <cstring>

CPath::CPath()
{
    m_size = 0;
    m_max = 0;
    m_path = nullptr;
}

CPath::~CPath()
{
    if (m_path) {
        delete [] m_path;
        m_path = nullptr;
    }
}

void CPath::read(IFile & file)
{
    forget();
    m_size = 0;
    file.read(&m_size, 2);
    m_max = m_size;
    m_path = new uint8_t[m_size];
    file.read(m_path, m_size);
}

void CPath::write(IFile & file)
{
    file.write(&m_size, 2);
    if (m_path) {
        file.write(m_path, m_size);
    }
}

void CPath::add(uint8_t aim)
{
    if (m_size == m_max) {
        m_max += GROW_BY;
        uint8_t *tmp = new uint8_t[m_max];
        if (m_path) {
            memcpy(tmp, m_path, m_size);
            delete [] m_path;
        }
        m_path = tmp;
    }


    m_path [ m_size ] = aim;
    ++ m_size;
}

void CPath::insertAt(int i, uint8_t aim)
{
    if (m_size == m_max) {
        m_max += GROW_BY;
        uint8_t *tmp = new uint8_t[m_max];
        if (m_path) {
            memcpy(tmp, m_path, m_size);
            delete [] m_path;
        }
        m_path = tmp;
    }

    for (int j=m_size; j > i; --j) {
        m_path[j] = m_path[j - 1];
    }

    m_path[i] = aim;

    ++m_size;
}

void CPath::removeAt(int i)
{
    if (m_path) {
        for (int j=i; j < m_size -1; ++j ) {
            m_path[j] = m_path[j + 1];
        }
    }

    --m_size ;
}

int CPath::getSize() const
{
    return m_size;
}

void CPath::forget()
{
    if (m_path) {
        delete [] m_path;
        m_path = nullptr;
    }
    m_size = 0;
    m_max = 0;
}

uint8_t & CPath::operator[] (int i) const
{
    return m_path[i] ;
}

CPath & CPath::operator = (const CPath &src)
{
    forget();

    m_size = src.getSize();
    m_max = m_size;

    if (m_size) {
        m_path = new uint8_t[m_max];
    }

    for (int i=0; i < m_size; ++i) {
        m_path [i] = src[i];
    }

    m_options = src.getOptions();

    return *this;
}

int CPath::getOptions() const
{
    return m_options;
}

void CPath::setOptions(int options)
{
    m_options = options;
}

void CPath::debug()
{
    //qDebug("[size %d] %s", m_size, toHex().c_str());
}

bool CPath::isValidAim(uint8_t aim)
{
    return aim == 0xff || aim < 4;
}

bool CPath::operator == (const CPath & src)
{
    if (m_size != src.getSize()) {
        return false;
    }
    for (int i=0; i < m_size; ++i) {
        if (src[i] != m_path[i]) {
            return false;
        }
    }
    return true;
}

bool CPath::operator != (const CPath & src)
{
    return !((*this) == src);
}

std::string CPath::toHex() {
    constexpr char hex[] = "0123456789ABCDEF";
    std::string s(m_size * 3, ' ');
    for (int i = 0; i < m_size; ++i) {
        s[3 * i]     = hex[m_path[i] >> 4];
        s[3 * i + 1] = hex[m_path[i] & 0x0f];
    }
    return s;
}

uint8_t * CPath::raw() {
    return m_path;
}
