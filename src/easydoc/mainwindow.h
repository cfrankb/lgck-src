/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2009, 2016  Francois Blanchette

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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include "Database.h"
#include "tabwidget.h"

namespace Ui
{
    class MainWindow;
}

#ifdef MAKE_KDE_APP
#include <kmainwindow.h>
#define MainWindowParent KMainWindow
#else
#include <QMainWindow>
#define MainWindowParent QMainWindow
#endif

class MainWindow : public MainWindowParent
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void open(const QString & fileName);
    void exportWiki(const char *path);

protected:
    void initFileMenu();
    bool maybeSave();
    bool save();
    bool saveAs();
    void warningMessage(const QString message);
    bool updateTitle();
    void updateRecentFileActions();
    void reloadRecentFileActions();

    static char m_fileFilter[];
    static char m_appName[];
    static char m_author[];
    CDatabase m_doc;

    enum {
        MaxRecentFiles = 4
    };

    QAction *m_recentFileActs[MaxRecentFiles];

private:
    Ui::MainWindow *ui;
    TabWidget *m_tabs;
    void closeEvent(QCloseEvent *event);
    bool m_saveHTML;
    void restoreSettings();

private slots:
    void on_action_About_triggered();
    void on_actionHTML_triggered();
    void on_actionSave_as_triggered();
    void on_action_save_triggered();
    void on_action_open_triggered();
    void openRecentFile();
    void on_actionNew_triggered();
    void on_actionFunctionList_triggered();
    void on_actionPlain_Text_triggered();
    void on_actionWiki_triggered();
    void on_actionGameLua_triggered();
    void on_actionSave_HTML_triggered(bool checked);
};

#endif // MAINWINDOW_H
