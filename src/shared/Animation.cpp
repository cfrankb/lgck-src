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
#include "Animation.h"
#include "IFile.h"

CAnimation::CAnimation()
{
    m_size = 0;
    m_speed = DEFAULT_SPEED;
    m_options = 0;
}

CAnimation::~CAnimation()
{
}

void CAnimation::read(IFile &file)
{
    file.read(&m_size, sizeof(uint16_t));
    file.read(&m_speed, sizeof(uint16_t));
    file.read(&m_options, sizeof(uint16_t));
    file.read(m_imageIds, m_size * sizeof(uint16_t));
}

void CAnimation::write(IFile &file)
{    
    file.write(&m_size, sizeof(uint16_t));
    file.write(&m_speed, sizeof(uint16_t));
    file.write(&m_options, sizeof(uint16_t));
    file.write(m_imageIds, m_size * sizeof(uint16_t));
}

void CAnimation::removeAt(int i)
{
    for (int j=i; j< m_size-1; ++j) {
        m_imageIds[j] = m_imageIds[j + 1];
    }

    --m_size;
}

void CAnimation::insertAt (int i, int imageId)
{
    for (int j=m_size; j > i; --j) {
        m_imageIds[j] = m_imageIds[j - 1];
    }

    m_imageIds[i] = imageId;

    ++m_size;
}

void CAnimation::add (int imageId)
{
    m_imageIds[m_size] = imageId;
    ++m_size;
}

int16_t CAnimation::operator [] (int i) const
{
    return m_imageIds[i];
}

void CAnimation::set(int i, int imageId)
{
    m_imageIds[i] = imageId;
}

int CAnimation::getSize() const
{
    return m_size;
}

int CAnimation::getSpeed() const
{
    return m_speed;
}

uint16_t CAnimation::getOptions() const
{
    return m_options;
}

void CAnimation::setSpeed(int speed)
{
    m_speed = speed;
}

void CAnimation::setOptions(uint16_t options)
{
    m_options = options;
}

void CAnimation::forget()
{
    m_size = 0;
    m_speed = DEFAULT_SPEED;
    m_options = 0;
}

CAnimation & CAnimation::operator = (const CAnimation & s)
{
    m_speed = s.getSpeed();
    m_size = s.getSize();
    m_options = s.getOptions();

    for (int i=0; i < m_size; ++i) {
        m_imageIds[i] = s[i];
    }

    return * this;
}

bool CAnimation::operator == (CAnimation & s)
{
    if (m_speed != s.getSpeed()) {
        return false;
    }

    if (m_size != s.getSize()) {
        return false;
    }

    if (m_options != s.getOptions()) {
        return false;
    }

    for (int i=0; i < m_size; ++i) {
        if (m_imageIds[i] != s[i]) {
            return false;
        }
    }

    return true;
}

bool CAnimation::operator != (CAnimation & s)
{
    return ! ((*this) == s);
}
