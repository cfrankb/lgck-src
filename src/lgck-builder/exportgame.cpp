#include "exportgame.h"

#include <QTemporaryDir>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QProcess>
#include <QFileDialog>
#include <QApplication>
#include "stdafx.h"
#include "helper.h"
#include "DlgDistributeGame.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/GameFile.h"
#include "FileWrap.h"
#include "Level.h"

CExportGame::CExportGame()
{
    //m_tmpDir.setAutoRemove(false);
    m_tmpPath = m_tmpDir.path();
    qDebug() << QString("tmpPath:") << m_tmpPath;
}

bool CExportGame::fileExists(QString & path)
{
    return QFileInfo::exists(path) && QFileInfo(path).isFile();
}

bool CExportGame::replace(QStringList & list, QString & in, QString & out) {
    int i = list.indexOf(in);
    if (i != -1) {
        list.replace(i, out);
        return true;
    } else {
        return false;
    }
}

bool CExportGame::replace(QStringList & list, const char* in, QString & out) {
    QString inq = QString(in);
    return replace(list, inq, out);
}

bool CExportGame::copyLicense(QString & outMsg)
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString licensePath = m_tmpPath + "/licenses";
    QDir dirTmpTxt(licensePath);
    if (!dirTmpTxt.exists()){
        dirTmpTxt.mkpath(licensePath);
    }

    QDir txtDir(appDir + "/licenses");
    QStringList nameFilter;
    nameFilter << "*.txt";
    QFileInfoList txtList = txtDir.entryInfoList(nameFilter, QDir::Files);
    foreach (QFileInfo file, txtList){
        QString src = file.absoluteFilePath();
        QString dst = licensePath + "/" + file.fileName();
        std::string msg;
        if (!copyFile(q2c(src), q2c(dst), msg)) {
            outMsg = msg.c_str();
            return false;
        }
    }
    return true;
}

bool CExportGame::copyRuntime(QString & runtimeSource, QStringList & depList, QString & outMsg)
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString runtimeTmp = m_tmpPath + "/" + m_stub;
    std::string src = q2c(runtimeSource);
    std::string dst = q2c(runtimeTmp);
    std::string msg;
    if (!copyFile(src, dst, msg)) {
        outMsg = msg.c_str();
        return false;
    }

    if (runtimeSource.right(4).toLower() == ".exe") {
        QString base = runtimeSource.mid(0, runtimeSource.size() - 4);
        qDebug() << QString("base:") << base;
        QString deps = base + ".depends";
        if (fileExists(deps)) {
            CFileWrap file;
            if (file.open(q2c(deps), "r")) {
                long size = file.getSize() ;
                char *buf = new char[size+1];
                buf[size] = 0;
                file.read(buf, size);
                file.close();
                QString data = buf;
                data.replace("\r\n", "\n");
                delete [] buf;
                depList = data.split("\n", Qt::SkipEmptyParts);
                return copyList(depList, appDir, outMsg);
            } else {
                qWarning() << QString("cannot read: %1").arg(deps);
            }
        } else {
            qWarning() << QString("cannot find: %1").arg(deps);
        }
    }
    return true;
}

bool CExportGame::copyList(QStringList & list, const QString srcDir, QString & outMsg)
{
    std::string src;
    std::string dst;
    std::string msg;
    foreach (QString file, list) {
        QString td = m_tmpPath + "/" + file;
        QString ts = srcDir + "/" + file;
        qDebug() << "source: " << ts;
        qDebug() << "destination: " << td;
        dst = q2c(td);
        src = q2c(ts);
        if (!copyFile(src, dst, msg)) {
            outMsg = msg.c_str();
            return false;
        }
    }
    return true;
}

bool CExportGame::exportCore(CDlgDistributeGame & dlg, CGameFile & gf, QString & outMsg)
{
    QWidget *parent = dynamic_cast<QWidget*>(dlg.parent());
    const char *appName = "LGCK builder";
    QString appDir = QCoreApplication::applicationDirPath();
    if (m_tmpDir.isValid()) {
        qDebug() << m_tmpPath;
    } else {
        outMsg = dlg.tr("There was a problem creating tmpFolder: %1").arg(m_tmpPath);
        return false;
    }

    QString args;
    QString fileFilter;
    QString outName;
    int outType = dlg.outType();
    if (outType == CDlgDistributeGame::OUT_ZIP) {
        fileFilter = dlg.tr("Zip archives (*.zip)");
        args = QString("a %out% %stub% %game% licenses/*.txt");
        outName = "game.zip";
    } else {
        fileFilter = dlg.tr("7z archives (*.7z)");
        args = QString("a -m0=BCJ2 -m1=LZMA:d25 -m2=LZMA:d19 -m3=LZMA:d19 -mb0:1 -mb0s1:2 -mb0s2:3 %out% %stub% %game% licenses/*.txt");
        outName = "game.7z";
    }

#ifdef Q_OS_WIN32
    m_stub = "game.exe";
    QString path = appDir + "/7z.exe";
    if (!fileExists(path)) {
        outMsg = dlg.tr("Cannot find %1").arg(path);
        return false;
    }
    m_cmd7z = "\"" + appDir + "/7z.exe\"";
#else
    m_stub = "game.exe";
    m_cmd7z = "7z";
#endif

    if (!copyLicense(outMsg)) {
        return false;
    }

    QStringList depList;
    QString runtimeSource;
    dlg.getRuntime(runtimeSource);
    if (!copyRuntime(runtimeSource, depList, outMsg)) {
        return false;
    }

    QStringList listMusic;
    for (int i=0; i < gf.getSize(); ++i) {
        CLevel & level = *(gf[i]);
        const char *music = level.getSetting("music");
        if (music && *music) {
            if (!strstr(music, "/") && !strstr(music, "\\")) {
                listMusic.append(QString(music));
            } else {
                QString msgText = dlg.tr("File path not supported %1.").arg(music);
                QMessageBox::warning(parent, appName, msgText);
            }

        }
    }
    listMusic.removeDuplicates();
    qDebug() << "music" << listMusic;
    if (!copyList(listMusic, QString(gf.m_path.c_str()), outMsg)) {
        return false;
    }

    std::string msg;
    QString gameFile = "game.lgckdb";
    QString gameFilePath = m_tmpPath + "/" + gameFile;
    gf.write(q2c(gameFilePath));

    QString gameOut = m_tmpPath + "/" + outName;
    QString finalOut = gameOut;
    QStringList list = args.split(" ", Qt::SkipEmptyParts);
    replace(list, "%game%", gameFile);
    replace(list, "%stub%", m_stub);
    replace(list, "%out%", gameOut);
    list = list + depList + listMusic;
    qDebug() << m_cmd7z;
    qDebug() << list;
    QProcess proc;
    proc.setWorkingDirectory(m_tmpPath);
    proc.start(m_cmd7z, list);
    proc.waitForFinished(-1);
    int result = proc.exitCode();
    if (result) {
        outMsg = QString(dlg.tr("An error occured: %1\nCode: %2"))
                       .arg(m_cmd7z + " " + list.join(" ")).arg(result);
        return false;
    }
    proc.close();

    if (outType == CDlgDistributeGame::OUT_SFX) {
        QString sfx;
        dlg.getSFX(sfx);
        QString setupOut = m_tmpPath + "/package.exe";
        std::list<std::string> files;
        files.push_back(q2c(sfx));
        files.push_back(q2c(gameOut));
        if (!concat(files, q2c(setupOut), msg)) {
            outMsg = msg.c_str();
            return false;
        }
        fileFilter = dlg.tr("Applications (*.exe)");
        outName = "package.exe";
        finalOut = setupOut;
    }

    QApplication::restoreOverrideCursor();
    QString fileName = QFileDialog::getSaveFileName(parent, dlg.tr("Export Distribution..."), outName, dlg.tr(q2c(fileFilter)));
    if (!fileName.isEmpty()) {
        std::string in = q2c(finalOut);
        std::string out = q2c(fileName);
        if (!copyFile(in, out, msg)){
            outMsg = msg.c_str();
            return false;
        }
        QString msgText = dlg.tr("Export successful.");
        QMessageBox::information(parent, appName, msgText);
    }
    return true;
}
