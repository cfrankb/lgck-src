/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

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
#include <QDebug>
#include "optiongroup.h"

COptionGroup::COptionGroup()
{
}

COptionGroup::~COptionGroup()
{

}

void COptionGroup::setName(const char *name)
{
    Q_ASSERT(name && name[0]);
    m_group = name;
}

QVariant & COptionGroup::get(const char *name)
{
    return m_options[name];
}

QVariant & COptionGroup::operator[](const char *name)
{
    return get(name);
}

COptionGroup & COptionGroup::operator=(COptionGroup &s)
{
    for (auto it = s.m_options.begin(); it != s.m_options.end(); ++it) {
        m_options[it->first] = it->second;
    }
    return *this;
}

COptionGroup &COptionGroup::set(const char *name, QVariant value)
{
    m_options[name] = value;
    return *this;
}

void COptionGroup::read(QSettings & settings)
{
    settings.beginGroup(m_group);
    for (auto it = m_options.begin(); it != m_options.end(); ++it) {
        std::string name = it->first;
        m_options[name] = settings.value(name.c_str(), it->second);
    }
    settings.endGroup();
}

void COptionGroup::write(QSettings &settings)
{
    settings.beginGroup(m_group);
    for (auto it = m_options.begin(); it != m_options.end(); ++it) {
        std::string name = it->first;
        settings.setValue(name.c_str(), it->second);
    }
    settings.endGroup();
}

void COptionGroup::debug()
{
    for (auto it = m_options.begin(); it != m_options.end(); ++it) {
        qDebug() << QString("debug option: ") << it->first.c_str() << it->second;
    }
}

void COptionGroup::unset(const char *name)
{
    m_options.erase(name);
}
