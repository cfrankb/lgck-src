/*
    LGCK Builder GUI
    Copyright (C) 1999, 2011  Francois Blanchette

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
#include "../shared/stdafx.h"
#include "../shared/Credits.h"
#include "mainwindow.h"
#include "DlgSelect.h"
#include <QFileDialog>
#include <QGLWidget>
#include <QSettings>
#include "WizGame.h"
#include "ss_build.h"
#include <ctime>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QScreen>
#include <QSurfaceFormat>
#include <QLockFile>

constexpr char appUuid[] = "e139b7fd-2d73-4eba-84ac-339fca583e89";

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setMajorVersion( 2 );
    format.setMinorVersion( 0 );
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    srand( time( nullptr ) );
    QApplication app(argc, argv);
    QString tmpDir = QDir::tempPath();
    QLockFile lockFile(tmpDir + QString("/%1.lock").arg(appUuid));
    if(!lockFile.tryLock(100)){
        QMessageBox::warning(nullptr, "", QObject::tr("You cannot run more than one instance of this application."));
        return 1;
    }

    QFileInfo fi(app.applicationDirPath());
    QString portable = QApplication::applicationDirPath() + "/portable.txt";
    if(fi.isDir() && fi.isWritable() && QFileInfo::exists(portable)) {
        // make this app portable
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, app.applicationDirPath());
    }
    MainWindow w;
    w.createEventEditor();

    QString fileName = "";
    bool done = false;
    QSettings settings(MainWindow::m_author, MainWindow::m_appName);
    settings.beginGroup("Editor");
    bool skipSplash = settings.value("skipSplash", false).toBool();
    settings.endGroup();

    if (argc == 2) {
        fileName = argv[1];
    }

    bool newProject = false;
    if (!skipSplash && fileName.isEmpty()) {
        do {
            QString ver = SS_BUILD_STR;
            CDlgSelect * dlg = new CDlgSelect(&w);
            dlg->setWindowTitle(MainWindow::m_appTitle + " " + ver);
            dlg->raise();
            dlg->setWindowState(Qt::WindowActive);
            QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
            int x = (screenGeometry.width() - dlg->width()) / 2;
            int y = (screenGeometry.height() - dlg->height()) / 2;
            dlg->move(x, y);
            dlg->exec();
            CWizGame *wiz;
            switch(dlg->getState()) {
            case CDlgSelect::OPEN:
                fileName = QFileDialog::getOpenFileName(&w, QObject::tr("Open"), "", QObject::tr("LGCK games (*.lgckdb)"));
                if (!fileName.isEmpty()) {
                    done = true;
                }
            break;

            case CDlgSelect::NEW:
                wiz = new CWizGame();
                wiz->init(w.getGame());
                if (wiz->exec()) {
                    wiz->save();
                    w.initToolBox();
                    done = true;
                    w.getGame()->setDirty(true);
                    newProject = true;
                }
                break;

            case CDlgSelect::CLOSE:
                return 0;

            case CDlgSelect::NO_SHOW:
                skipSplash = true;
                done = true;
                break;

            case CDlgSelect::SKIP:
                done = true;
            }

            delete dlg;
        } while (!done);
    }

    settings.beginGroup("Editor");
    settings.setValue("skipSplash", skipSplash);
    settings.endGroup();

    w.show();
    if (!fileName.isEmpty()) {
        if (fileName.toLower().endsWith(".lgckdb")) {
            w.open(fileName);
        } else {
            QString errMsg = QObject::tr("Invalid file: %1").arg(fileName);
            QMessageBox::warning(&w, QString(MainWindow::m_appName), errMsg);
        }
    }

    if (newProject) {
        w.addLevel();
    }

    w.setReady(true);
    return app.exec();
}
