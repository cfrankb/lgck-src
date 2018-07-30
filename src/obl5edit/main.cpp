/*
    LGCK Builder package
    Copyright (C) 2005, 2011  Francois Blanchette

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

#include <stdio.h>
#include <string.h>
#include <QApplication>
#include "mainwindow.h"
#include <QSurfaceFormat>
#include <QSettings>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setMajorVersion( 2 );
    format.setMinorVersion( 0 );
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);
    QFileInfo fi(app.applicationDirPath());
    if(fi.isDir() && fi.isWritable()) {
        // make the settings portable
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, app.applicationDirPath());
    }

    MainWindow w;
    w.show();
    if (argc == 2) {
        QString fileName = argv[1];
        w.openFile(fileName);
    }
    return app.exec();
}
