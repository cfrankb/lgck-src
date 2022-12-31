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

#include <QApplication>
#include <QDir>
#include "mainwindow.h"
#include "../shared/qtgui/cheat.h"
#include <QSettings>

int main(int argc, char *argv[])
{
    QString src;
    QString target;

    enum
    {
        OPTION_NONE,
        OPTION_OUTPUT
    };
    int flag = OPTION_NONE;
    bool quiet = false;

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (flag)
            {
                qDebug("error: flag to a flag`%s`???", argv[i]);
                break;
            }
            if (strcmp(argv[i], "-o") == 0)
            {
                flag = OPTION_OUTPUT;
                continue;
            }
            if (strcmp(argv[i], "-q") == 0)
            {
                quiet = true;
                continue;
            }
            qDebug("error: invalid option `%s`", argv[i]);
            break;
        }
        else
        {
            if (flag)
            {
                flag = false;
                if (!QDir(argv[i]).exists())
                {
                    qDebug("target doesn't exists: %s", argv[i]);
                    continue;
                }
                if (target.isEmpty())
                {
                    target = argv[i];
                }
                else
                {
                    qDebug("cannot redefine target to `%s`", argv[i]);
                }
            }
            else
            {
                if (src.isEmpty())
                {
                    src = argv[i];
                }
                else
                {
                    qDebug("cannot redefine src to `%s`", argv[i]);
                }
            }
        }
    }

    QApplication app(argc, argv);
    QFileInfo fi(app.applicationDirPath());
    QString portable = QApplication::applicationDirPath() + "/portable.txt";
    if (fi.isDir() && fi.isWritable() && QFileInfo::exists(portable))
    {
        // make this app portable
        qDebug("app dir: %s", q2c(app.applicationDirPath()));
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, app.applicationDirPath());
    }

    MainWindow w;
    if (!src.isEmpty())
    {
        w.open(src);
    }

    if (!src.isEmpty() &&
        !target.isEmpty())
    {
        w.exportWiki(q2c(target));
    }

    if (!quiet)
    {
        w.show();
        return app.exec();
    }

    return 0;
}
