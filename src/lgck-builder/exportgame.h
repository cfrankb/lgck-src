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
#ifndef EXPORTGAME_H
#define EXPORTGAME_H

#include <QString>
#include <QObject>
#include <QTemporaryDir>

class CDlgDistributeGame;
class CGameFile;

class CExportGame: public QObject
{
    Q_OBJECT
public:
    CExportGame();
    bool exportCore(CDlgDistributeGame & dlg, CGameFile &file, QString & outMsg);
    bool fileExists(QString & path);
    bool replace(QStringList & list, QString & in, QString & out);
    bool replace(QStringList & list, const char* in, QString & out);
    bool copyLicense(QString & outMsg);
    bool copyRuntime(QString & runtimeSource, QStringList &depList, QString & outMsg);
    bool copyList(QStringList & list, const QString srcDir, QString & outMsg);

protected:
    QTemporaryDir m_tmpDir;
    QString m_tmpPath;
    QString m_stub;
    QString m_cmd7z;
};

#endif // EXPORTGAME_H
