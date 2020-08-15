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
#ifndef COPTIONGROUP_H
#define COPTIONGROUP_H

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <unordered_map>

class QSettings;

class COptionGroup
{
public:
    COptionGroup();
    ~COptionGroup();
    QVariant & operator[](const char *name);
    void unset(const char *name);
    COptionGroup & operator=(COptionGroup &s);
    void setName(const char *name);
    COptionGroup & set(const char *name, QVariant value);
    QVariant & get(const char *name);
    void read(QSettings & settings);
    void write(QSettings & settings);
    void debug();

protected:
    QString m_group;
    std::unordered_map<std::string, QVariant> m_options;

friend class COptions;
};


#endif // COPTIONGROUP_H
