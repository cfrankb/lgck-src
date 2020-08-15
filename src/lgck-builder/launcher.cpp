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
#include <QString>
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QObject>
#include <QProcess>
#include <vector>
#include "launcher.h"

bool fileExists(const QString & path)
{
    return QFileInfo::exists(path) && QFileInfo(path).isFile();
}

bool getCmd(const QString & appName, Path & outPath)
{
    std::vector<Path> paths;
    QString appDir = QCoreApplication::applicationDirPath();
    qDebug() << appDir;
    if (QSysInfo::productType() == "windows") {
        paths.push_back(Path { appDir, appName + ".exe" });
    } else {
        paths.push_back(Path { appDir, appName + ".sh" });
        paths.push_back(Path { appDir + "/..", appName + ".sh"});
        paths.push_back(Path { appDir, appName });
    }

    for(auto path: paths){
        const QString t = path.path + "/" + path.cmd;
        qDebug() << "trying path: " + t;
        if (fileExists(t)) {
            qDebug() << "final cmd: " + t;
            outPath = path;
            return true;
        }
    }
    return false;
}

bool launchProcess(const Path & path)
{
    QProcess process;
    process.setWorkingDirectory(path.path);
    process.setProgram(path.cmd);
    process.setArguments(QStringList());
    return process.startDetached();
}
