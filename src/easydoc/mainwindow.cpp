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

#include "stdafx.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QMessageBox>
#include <QResizeEvent>
#include "stdafx.h"
#include "DlgAbout.h"
#include "tabwidget.h"
#include <QFileDialog>

char MainWindow::m_fileFilter[] = "easyDoc (*.edoc)";
char MainWindow::m_appName[] = "easyDoc";
char MainWindow::m_author[] = "cfrankb";

static QString g_fileNameHTML;

MainWindow::MainWindow(QWidget *parent)
    : MainWindowParent(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateTitle();
    initFileMenu();
    restoreSettings();
    m_tabs = new TabWidget(this);
    m_tabs->init(&m_doc);
    setCentralWidget(m_tabs);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::maybeSave()
{
    if (m_doc.isDirty()) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr(m_appName),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard
                     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::open(const QString & fileNameNew)
{
    if (maybeSave()) {
        QString fileName = fileNameNew;
        if (fileName.isEmpty()) {
            fileName = QFileDialog::getOpenFileName(this, "", fileName, tr(m_fileFilter));
        }

        if (!fileName.isEmpty()) {
            QString oldFileName = m_doc.getFileName();
            m_doc.setFileName(fileName);
            if (!m_doc.read())  {
                warningMessage(tr("cannot open file:\n") + m_doc.getLastError());
                m_doc.setFileName(oldFileName);
                // update fileList
                QSettings settings(m_author, m_appName);
                QStringList files = settings.value("recentFileList").toStringList();
                files.removeAll(fileName);
                settings.setValue("recentFileList", files);
            }

            updateTitle();
            updateRecentFileActions();
            reloadRecentFileActions();            
        }
    }

    m_tabs->init(&m_doc);
}

bool MainWindow::save()
{
    QString oldFileName = m_doc.getFileName();

    if (m_doc.isUntitled()) {
        if (!saveAs())
            return false;
    }

    if (!m_doc.write() || !updateTitle())  {
        warningMessage(tr("Can't write file"));
        m_doc.setFileName(oldFileName);
        return false;
    }

    if (m_saveHTML) {
        CFileWrap file;
        QString fileName = m_doc.getFileName();
        const char EDOC[] = ".edoc";
        if (fileName.endsWith(EDOC)) {
            fileName = fileName.mid(0, fileName.length() - strlen(EDOC));
        }
        const char HTML[] = ".html";
        if (!fileName.endsWith(HTML)) {
            fileName += HTML;
        }
        if (file.open(fileName, QIODevice::WriteOnly)) {
            m_doc.dump(file);
            file.close();
        }  else {
            // write error
            warningMessage( QString(tr("can't write to %1")).arg(fileName) );
        }
    }

    updateRecentFileActions();
    reloadRecentFileActions();

    return true;
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), m_doc.getFileName(), tr(m_fileFilter));
    if (fileName.isEmpty())
        return false;

    m_doc.setFileName(fileName);
    return true;
}

void MainWindow::warningMessage(const QString message)
{
    QMessageBox msgBox(QMessageBox::Warning, m_appName, message, 0, this);
    msgBox.exec();
}

bool MainWindow::updateTitle()
{
    QString file;

    if (m_doc.getFileName().isEmpty()) {
        file = tr("untitled");
    } else {
        file = QFileInfo(m_doc.getFileName()).fileName();
    }

    m_doc.setDirty(false);
    setWindowTitle(tr("%1[*] - %2").arg( file  )  .arg(tr(m_appName)));

    return true;
}

void MainWindow::initFileMenu()
{
    // gray out the open recent `nothin' yet`
    ui->actionNothing_yet->setEnabled(false);

    for (int i = 0; i < MaxRecentFiles; i++) {
        m_recentFileActs[i] = new QAction(this);
        m_recentFileActs[i]->setVisible(false);
        ui->menuRecent_files->addAction(m_recentFileActs[i]);
        connect(m_recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    reloadRecentFileActions();

    // connect the File->Quit to the close app event
    connect(ui->action_quit, SIGNAL(triggered()), this, SLOT(close()));
    ui->action_quit->setMenuRole(QAction::QuitRole);
}


void MainWindow::reloadRecentFileActions()
{
    QSettings settings(m_author, m_appName);
    QStringList files = settings.value("recentFileList").toStringList();
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        m_recentFileActs[i]->setText(text);
        m_recentFileActs[i]->setData(files[i]);
        m_recentFileActs[i]->setVisible(true);
        m_recentFileActs[i]->setStatusTip(files[i]);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        m_recentFileActs[j]->setVisible(false);

    ui->actionNothing_yet->setVisible(numRecentFiles == 0);
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(m_author, m_appName);
    QStringList files = settings.value("recentFileList").toStringList();
    QString fileName = m_doc.getFileName();
    files.removeAll(fileName);
    if (!fileName.isEmpty()) {
        files.prepend(fileName);
        while (files.size() > MaxRecentFiles)
            files.removeLast();
    }

    settings.setValue("recentFileList", files);
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        open(action->data().toString());
    }
}

void MainWindow::on_action_open_triggered()
{
    open("");
}

void MainWindow::on_action_save_triggered()
{
   save();
}

void MainWindow::on_actionSave_as_triggered()
{
   if (saveAs()) {
        save();
    }
}

void MainWindow::on_actionHTML_triggered()
{
    const char fileFilter[] = "html documents (*.html)";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export..."), g_fileNameHTML, tr(fileFilter));
    if (!fileName.isEmpty()) {
        CFileWrap file;
        if (!fileName.endsWith(".html")) {
            fileName += ".html";
        }
        if (file.open(fileName, QIODevice::WriteOnly)) {
            m_doc.dump(file);
            file.close();
        }  else {
            // write error
            warningMessage( QString(tr("can't write to %1")).arg(fileName) );
        }
    }
}

void MainWindow::on_action_About_triggered()
{
    CDlgAbout *dlg = new CDlgAbout(this);
    dlg->exec();
    delete dlg;
}

void MainWindow::on_actionNew_triggered()
{
    if (maybeSave()) {
        m_doc.setFileName("");
        m_doc.setDirty(false);
        m_doc.removeAll();
        m_tabs->init(&m_doc);
        updateTitle();
    }
}

void MainWindow::on_actionFunctionList_triggered()
{
    const char fileFilter[] = "text files (*.txt)";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export..."), "", tr(fileFilter));
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".txt")) {
            fileName += ".txt";
        }
        CFileWrap file;
        if (file.open(fileName, QIODevice::WriteOnly)) {
            m_doc.exportList(file);
            file.close();
        }  else {
            // write error
            warningMessage( QString(tr("can't write to %1")).arg(fileName) );
        }
        g_fileNameHTML = fileName;
    }
}

void MainWindow::on_actionPlain_Text_triggered()
{
    const char fileFilter[] = "text files (*.edocx)";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export..."), "", tr(fileFilter));
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".edocx")) {
            fileName += ".edocx";
        }
        CFileWrap file;
        if (file.open(fileName, QIODevice::WriteOnly)) {
            m_doc.exportText(file);
            file.close();
        }  else {
            // write error
            warningMessage( QString(tr("can't write to %1")).arg(fileName) );
        }
    }
}

void MainWindow::on_actionWiki_triggered()
{
    QSettings settings(m_author, m_appName);
    QString folder = settings.value("wikiFolder", "").toString();
    folder = QFileDialog::getExistingDirectory(this, "Export folder...", folder);
    if (!folder.isEmpty()) {
        m_doc.exportWiki(folder+"/");
        settings.setValue("wikiFolder", folder);
    }
}

void MainWindow::exportWiki(const char *path)
{
    QString folder = path;
    if (folder[folder.length()-1].toLatin1()!='/') {
        folder += "/";
    }
    m_doc.exportWiki(folder);
}

void MainWindow::on_actionGameLua_triggered()
{
    const char fileFilter[] = "GameLua.h (GameLua.h)";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import..."), "", tr(fileFilter));
    if (!fileName.isEmpty()) {
        CFileWrap file;
        if (file.open(fileName, QIODevice::ReadOnly)) {
            int size = file.getSize();
            char *buf = new char[size+1];
            buf[size]=0;
            file.read(buf, size);
            file.close();
            m_doc.importGameLua(buf);
            delete [] buf;
            m_tabs->init(&m_doc);
        }  else {
            // read error
            warningMessage( QString(tr("can't read %1")).arg(fileName) );
        }
    }
}

void MainWindow::on_actionSave_HTML_triggered(bool checked)
{
    m_saveHTML = checked;
    QSettings settings(m_author, m_appName);
    settings.setValue("saveHTML", m_saveHTML);
}

void MainWindow::restoreSettings()
{
    QSettings settings(m_author, m_appName);
    m_saveHTML = settings.value("saveHTML", false).toBool();
    ui->actionSave_HTML->setChecked(m_saveHTML);
}
