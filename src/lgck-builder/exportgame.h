#ifndef EXPORTGAME_H
#define EXPORTGAME_H

#include <QString>
#include <QTemporaryDir>

class CDlgDistributeGame;
class CGameFile;

class CExportGame
{
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
