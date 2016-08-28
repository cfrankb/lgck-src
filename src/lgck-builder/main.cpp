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
#include "WizGame.h"

int main(int argc, char *argv[])
{
    //QApplication::setAttribute(Qt::AA_UseDesktopOpenGL, false);
    //QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, true);
    QApplication app(argc, argv);
    MainWindow w;
    w.createEventEditor();

    char m_fileFilter[] = "LGCK games (*.lgckdb)";
    QString fileName = "";
    bool done = false;

    do {
        CDlgSelect * dlg = new CDlgSelect(&w);
        dlg->exec();
        CWizGame *wiz;
        switch(dlg->getState()) {
        case CDlgSelect::OPEN:
            fileName = QFileDialog::getOpenFileName(&w, w.tr("Open"), "", w.tr(m_fileFilter));
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
            }
            break;

        case CDlgSelect::CLOSE:
            return 0;

        case CDlgSelect::SKIP:
            done = true;
        }

        delete dlg;
    } while (!done);

    w.show();
    if (!fileName.isEmpty()) {
        w.open(fileName);
    }
#ifdef Q_OS_WIN32
#else
    srand( time( NULL ) );
#endif
    return app.exec();
}
