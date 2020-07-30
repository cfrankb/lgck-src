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
