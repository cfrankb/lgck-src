/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2009, 2011  Francois Blanchette

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

#include "Class.h"

CClass::CClass()
{
}

CClass::~CClass()
{
}

QString & CClass::name()
{
    return m_name;
}

QString & CClass::desc()
{
    return m_desc;
}

CFunctions & CClass::methods()
{
    return m_methods;
}

CParams & CClass::variables()
{
    return m_variables;
}
