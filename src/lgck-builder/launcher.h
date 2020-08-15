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
#ifndef __LAUNCHER__H
#define __LAUNCHER__H

#include <QString>

constexpr char SPRITE_EDITOR[] = "obl5edit";

typedef struct {
    QString path;
    QString cmd;
} Path;

bool getCmd(const QString & appName, Path &outPath);
bool fileExists(const QString & path);
bool launchProcess(const Path & path);

#endif
