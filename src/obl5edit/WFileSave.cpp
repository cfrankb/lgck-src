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
#include "WFileSave.h"

CWFileSave::CWFileSave(QWidget *parent ,
     const QString &caption,
     const QString &directory,
     const QString &filter):
    QFileDialog(parent, caption,directory,filter)
{

}

void CWFileSave::changeDefaultSuffix ( const QString & filter )
{
    //const char oblFilter[] = "Object Blocks (*.obl *.obl5)";
    const char pngFilter[] = "PNG Images (*.png)";
    if (filter == tr(pngFilter)) {
        setDefaultSuffix("png");
    } else {
        setDefaultSuffix("obl");
    }
}
