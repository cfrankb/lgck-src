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
#include "qthelper.h"
#include "../Frame.h"
#include "../GameFile.h"
#include <QtDebug>
#include <QString>

QIcon frame2icon(CFrame & frame)
{
    QIcon icon;
    icon.addPixmap(frame2pixmap(frame), QIcon::Normal, QIcon::On);
    return icon;
}

QPixmap frame2pixmap(CFrame & frame)
{
    uint8_t *png;
    int size;
    frame.toPng(png, size);
    QImage img;
    if (!img.loadFromData(png, size)) {
        qWarning("failed to load png");
    }
    delete [] png;
    return QPixmap::fromImage(img);
}

QString formatVersion(bool singleDigit)
{
    int version = CGameFile::getEngineVersion();
    int vv[4]={0,0,0,0};
    for (int i=3; i >= 0; --i) {
        vv[i] = version & 0xff;
        version /= 256;
    }
    const char *fmt = singleDigit ? "%.1d.%.1d.%.1d.%.1d" : "%.2d.%.2d.%.2d.%.2d";
    return QString::asprintf(fmt, vv[0], vv[1], vv[2], vv[3]);
}
