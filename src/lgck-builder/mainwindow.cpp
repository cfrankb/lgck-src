/*
    LGCK Builder GUI
    Copyright (C) 1999, 2016  Francois Blanchette

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

#include <QtGui>
#include <QMessageBox>
#include <QResizeEvent>
#include <QFileDialog>
#include <QPoint>
#include <QLabel>
#include <QComboBox>
#include <QDesktopServices>
#include <QSplashScreen>
#include <QToolBar>
#include <QProcess>
#include <QOpenGLWidget>
#include <QScrollArea>
#include <QFileSystemWatcher>
#include <QByteArray>
#include <QOperatingSystemVersion>
#include <QFlags>
#include <QSysInfo>
#include <QToolButton>
#include "../shared/qtgui/cheat.h"
#include "../shared/Level.h"
#include "../shared/Layer.h"
#include "../shared/StringTable.h"
#include "../shared/PathBlock.h"
#include "../shared/Path.h"
#include "../shared/interfaces/IImageManager.h"
#include "../shared/interfaces/IMusic.h"
#include "../shared/interfaces/ISound.h"
#include "../shared/implementers/sdl/mu_sdl.h"
#include "../shared/implementers/sdl/sn_sdl.h"
#include "../shared/GameEvents.h"
#include "../shared/Frame.h"
#include "../shared/inputs/qt/kt_qt.h"
#include "../shared/qtgui/qthelper.h"
#include "../shared/Countdown.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DlgEditLevel.h"
#include "DlgGotoLevel.h"
#include "DlgGame.h"
#include "DlgEntry.h"
#include "DlgAbout.h"
#include "DlgAppSettings.h"
#include "DlgLayer.h"
#include "DlgSource.h"
#include "DlgObject.h"
#include "DlgFrameSet.h"
#include "DlgTestLevel.h"
#include "DlgExportSprite.h"
#include "WEditEvents.h"
#include "WizFrameSet.h"
#include "WizGame.h"
#include "WizScript.h"
#include "Snapshot.h"
#include "ToolBoxDock.h"
#include "thread_updater.h"
#include "levelviewgl.h"
#include "levelscroll.h"
#include "helper.h"
#include "DlgDistributeGame.h"
#include "exportgame.h"
#include "infodock.h"
#include "gamefixer.h"
#include "dlgindicator.h"
#include "launcher.h"
#include "options.h"
#include "../shared/qtgui/qfilewrap.h"
#include "dlgexportsounds.h"

#ifdef LGCK_GAMEPAD
#include <QtGamepad/QGamepad>
#endif

const char MainWindow::m_appName[] = "LGCK builder";
const char MainWindow::m_author[] = "cfrankb";
const QString MainWindow::m_appTitle = MainWindow::tr("LGCK builder IDE");

constexpr const char WEB_PATH[] = "https://cfrankb.com/lgck/";
constexpr const char UPDATER_URL[] = "https://cfrankb.com/lgck/api/chkv.php?ver=%s&driver=%s&os=%s&uuid=%s&product=%s";

/* do not translate */
constexpr const char EDITOR[] = "Editor";
constexpr int MIN_FONT_SIZE = 10;
constexpr int MAX_FONT_SIZE = 50;
constexpr int DEFAULT_FONT_SIZE = MIN_FONT_SIZE;
constexpr char DEFAULT_FONT_NAME[] = "Courier";
constexpr char FONT_SIZE[] = "fontSize";
constexpr char FONT_NAME[] = "fontName";
constexpr char SKIP_SPLASH[] = "skipSplash";
constexpr char ENABLE_AUTO_COMPLETE[] = "enableAutocomplete";
constexpr char ENABLE_HIGHLIGHT[] = "enableHighlight";
constexpr char ENABLE_WHITESPACE[] = "enableWhiteSpace";
constexpr char ENABLE_WORDWRAP[] = "enableWordWrap";
constexpr char GENERAL [] = "";
constexpr char GRIDCOLOR[] = "gridColor";
constexpr char SHOWGRID[] = "showGrid";
constexpr char TRIGGER_KEY_COLOR[] = "triggerKeyColor";
constexpr char TRIGGER_KEY_FONT_SIZE[] = "triggerKeyFontSize";
constexpr char TRIGGER_KEY_SHOW[] = "showTriggerKey";
constexpr char GRIDSIZE[] = "gridSize";
constexpr char LAST_PROJECTS[] = "lastProjects";
constexpr char TESTLEVEL[] = "TestLevel";
constexpr char SKILL[] = "skill";
constexpr char LIVES[] = "lives";
constexpr char SCORE[] = "score";
constexpr char START_HP[] = "start_hp";
constexpr char CONTINUE[] = "continue";
constexpr char XTICK_MAX_RATE[] = "tick_max_rate";
constexpr char LAST_FOLDER[] = "rememberLastFolder";
constexpr char FOLDERS[] = "Folders";
constexpr char FOLDER_LGCKDB[] = "lgckdb";
constexpr char INPUTS [] = "Inputs";
constexpr char SKILL_FILTER[] = "skill_filter";

#define O_STR(s,k) (*m_options)[s].get(k).toString()
#define O_INT(s,k) (*m_options)[s].get(k).toInt()
#define O_BOOL(s,k) (*m_options)[s].get(k).toBool()
#define O_SET(s,k,v) (*m_options)[s].set(k,v)

/* do not translate */
constexpr char actionNames[][16] = {
    "Up",
    "Down",
    "Left",
    "Right",
    "Jump",
    "Fire",
    "Action",
    "Special1",
    "Special2"
};

/* do not translate */
constexpr char actionKeys[][16] = {
    "Up", "Down", "Left", "Right",
    "Space", "Shift", "Z", "", ""
};

/* do not translate */
constexpr char actionButtons[][16] = {
    "Up", "Down", "Left", "Right",
    "A", "B", "X", "", ""
};


MainWindow *me = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Gamepad
#ifdef LGCK_GAMEPAD
    m_gamepad = nullptr;
    initGamePad();
#endif

    // Init Settings
    initSettings();

    // Setup the timer for the indicator
    changeTickMaxRate();
    m_timerIndicator.setInterval(500);
    m_timerIndicator.stop();
    m_ready = false;
    me = this;
    QString appVersion = formatVersion(false);

    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setOrganizationName(m_author);
    QCoreApplication::setApplicationName(m_appName);
    QCoreApplication::setApplicationVersion(appVersion);

    ui->setupUi(this);
#ifndef LGCK_RUNTIME
    ui->actionDistribution_Package->setDisabled(true);
#endif
    m_proto = CGame::INVALID;
    m_event = CGame::INVALID;
    m_viewMode = VM_EDITOR;
    m_fixer = new CGameFixer;
    m_fixer->setGame(&m_doc);
    // initToolBar() must be placed before update menus.
    initToolBar();
    m_scroll = new CLevelScroll(this, &m_doc);
    m_lview = static_cast<CLevelViewGL*>(m_scroll->viewport());
    setCentralWidget(m_scroll);

    connect(m_scroll, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&))) ;
    connect(m_scroll, SIGNAL(keyChanged(int, int)),
        this, SLOT( notifyKeyEvent(int, int))) ;
    connect(m_scroll, SIGNAL(menuChanged()),
        this, SLOT( updateMenus())) ;
    connect(m_scroll, SIGNAL(statusChanged(int, QString)),
        this, SLOT( setStatus(int, QString))) ;
    connect(m_scroll, SIGNAL(newLevelReq()),
        this, SLOT( addLevel())) ;
    connect(this, SIGNAL(gameModeEnabled(bool)),
           m_scroll, SLOT(setGameMode(bool)));
    connect(m_scroll, SIGNAL(pathEnded()),
            this, SLOT(closePath()));
    connect(this, SIGNAL(pathStarted()),
            m_scroll, SLOT(enterEditPath()));
    connect(this, SIGNAL(pathEnded()),
            m_scroll, SLOT(leaveEditPath()));
    setMinimumHeight(this->maximumHeight());

    m_labels[0] = new QLabel("", ui->statusBar);
    ui->statusBar->addWidget(m_labels[0], LABEL0_SIZE);
    m_labels[2] = new QLabel("", ui->statusBar);
    ui->statusBar->addWidget(m_labels[2], 150);
    m_labels[2]->setAlignment(Qt::AlignRight);
    m_labels[1] = new QLabel("", ui->statusBar);
    m_labels[1]->setAlignment(Qt::AlignRight);
    ui->statusBar->addWidget(m_labels[1], 640 - LABEL0_SIZE);

    updateTitle();
    initFileMenu();
    updateMenus();

    // Setup the timer for the screen refresh
    m_time.start();
    m_nextTick = m_time.elapsed() + TICK_SCALE;

    // create the toolbox
    m_toolBox = new CToolBoxDock(this);
    m_toolBox->hide();
    m_toolBox->setGameDB(&m_doc);
    this->addDockWidget(Qt::LeftDockWidgetArea, m_toolBox);

    connect(this, SIGNAL(frameSetChanged(int)),
            m_toolBox, SLOT(updateFrameSet(int)));

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(viewEvent()));
    connect(&m_timerIndicator, SIGNAL(timeout()), this, SLOT(updateIndicator()));
    m_toolBox->init();

    // link the toolBox to MainWindow
    connect(this, SIGNAL(levelDeleted(int)),
            m_toolBox, SLOT(deleteLevel(int)));

    connect(m_toolBox, SIGNAL(levelDeleted(int)),
            this, SLOT(deleteLevel(int)));

    connect(this, SIGNAL(levelEdited(int)),
            m_toolBox, SLOT(editLevel(int)));

    connect(m_toolBox, SIGNAL(levelEdited(int)),
            this, SLOT(editLevel(int)));

    connect(this, SIGNAL(levelAdded()),
            m_toolBox, SLOT(addLevel()));

    connect(m_toolBox, SIGNAL(levelAdded()),
            this, SLOT(addLevel()));

    connect(m_toolBox, SIGNAL(levelSelection(int)),
            this, SLOT(selectLevel(int)));

    connect(this, SIGNAL(levelSelected(int)),
            m_toolBox, SLOT(selectLevel(int)));

    connect(m_toolBox, SIGNAL(spriteDeleted(int)),
            this, SLOT(deleteSprite(int)));

    connect(m_toolBox, SIGNAL(visibilityChanged(bool)),
                     ui->action_ShowToolbox, SLOT(setChecked(bool)));
    connect(ui->action_ShowToolbox, SIGNAL(triggered(bool)),
                     m_toolBox, SLOT(setVisible(bool)));

    connect(m_toolBox, SIGNAL(spriteChanged(int)),
            this, SLOT(changeSprite(int)));

    connect(this, SIGNAL(levelMoved(int, int)),
            m_toolBox, SLOT(moveLevel(int, int)));

    connect(this, SIGNAL(eventStatusShown()),
            this, SLOT(showEventStatus()));

    connect(this, SIGNAL(spriteCreated()),
            m_toolBox, SLOT(createSprite()));

    connect(this, SIGNAL(spriteUpdated(int)),
            m_toolBox, SLOT(updateIcon(int)));

    connect(this, SIGNAL(gridVisible(bool)),
            m_lview, SLOT(showGrid(bool)));

    connect(this, SIGNAL(scrollbarShown(bool)),
            m_scroll, SLOT(showScrollbars(bool)));

    connect(this, SIGNAL(gridColorChanged(QString)),
            m_lview, SLOT(setGridColor(QString)));

    connect(this, SIGNAL(gridSizeChanged(int)),
            m_lview, SLOT(setGridSize(int)));

    connect(this, SIGNAL(gridSizeChanged(int)),
            m_scroll, SLOT(setGridSize(int)));

    connect(this, SIGNAL(levelSelected(int)),
            m_scroll, SLOT(changeLevel(int)));

    connect(this, SIGNAL(spritePaintStateChanged(bool)),
            m_scroll, SLOT(setPaintState(bool)));

    connect(m_toolBox, SIGNAL(currentProtoChanged(int)),
            m_scroll, SLOT(changeProto(int)));

    connect(m_toolBox, SIGNAL(currentProtoChanged(int)),
            this, SLOT(changeProtoIcon(int)));

    connect(this, &MainWindow::currentFrameChanged,
            m_scroll, &CLevelScroll::changeProtoFrame);
    connect(this, &MainWindow::skillFilterChanged,
            m_lview, &CLevelViewGL::setSkillFilters);
    connect(this, &MainWindow::skillFilterChanged,
            m_scroll, &CLevelScroll::setSkillFilters);

    connect(this, SIGNAL(eraserStateChanged(bool)),
            m_scroll, SLOT(setEraserState(bool)));

    connect(this, SIGNAL(triggerKeyColorChanged(QString)),
            m_lview, SLOT(setTriggerKeyColor(QString)));

    connect(this, SIGNAL(triggerKeyShow(bool)),
            m_lview, SLOT(showTriggerKey(bool)));

    connect(this, SIGNAL(triggerKeyFontSizeChanged(int)),
            m_lview, SLOT(setTriggerFontSize(int)));

    // debugOutput
    m_infoDock = new CInfoDock(this);
    m_infoDock->setWindowTitle(tr("Debug output"));
    m_infoDock->setAllowedAreas(Qt::DockWidgetAreas(Qt::BottomDockWidgetArea));
    this->addDockWidget(Qt::BottomDockWidgetArea, m_infoDock);
    connect(this, SIGNAL(debugText(QString)),
            m_infoDock, SLOT(appendText(QString)));
    connect(this, SIGNAL(errorText(QString)),
            m_infoDock, SLOT(appendError(QString)));
    connect(m_infoDock, SIGNAL(visibilityChanged(bool)),
                     ui->actionDebugOutput, SLOT(setChecked(bool)));

    CLuaVM::setCallback(CLuaVM::Debug, newDebugString);
    CLuaVM::setCallback(CLuaVM::Error, newErrorString);
    emit debugText(tr("LuaVM ready.\n"));

    // reload settings
    reloadSettings();
    CSndSDL *sn = new CSndSDL();
    m_doc.attach(static_cast<ISound*>(sn));
    CMusicSDL *mu = new CMusicSDL();
    m_doc.attach(static_cast<IMusic*>(mu));
    m_updater = new CThreadUpdater();
    if (m_bUpdate) {
        connect(m_updater,SIGNAL(newVersion(QString, QString)),this, SLOT(updateEditor(QString, QString)));
    }
}

void MainWindow::createEventEditor()
{
    // Create the event Editor
    qDebug("createEventEditor");

    m_editEvents = new CWEditEvents(this);
    m_editEvents->setWindowTitle(tr("Event script"));
    m_editEvents->hide();
    m_editEvents->setGameFile(&m_doc);
    QPoint pt = m_scroll->pos();
    pt.setX(pt.x() + 4);
    m_lview->move(pt);
    m_editEvents->move(pt);

    connect(this, SIGNAL(protoChanged(int,int)),
            m_editEvents, SLOT(setProto(int, int)));

    connect(this, SIGNAL(eventsCommited()),
            m_editEvents, SLOT(commitEvents()));

    this->addDockWidget(Qt::RightDockWidgetArea, m_editEvents);

    connect(this, SIGNAL(spriteDeleted(int)),
            m_editEvents, SLOT(deleteSprite(int)));

    connect(m_editEvents, SIGNAL(viewModeChanged()),
            this, SLOT(changeViewMode()));

    connect(m_editEvents, SIGNAL(eventModified()),
            this, SLOT(reloadEventCombo()));

    connect(this, SIGNAL(textInserted(const char*)),
            m_editEvents, SLOT(insertText(const char*)));

    connect(this, SIGNAL(fontChanged(const QFont &)),
            m_editEvents, SLOT(setFont(const QFont &)));

    connect(this, SIGNAL(editorOptionChanged(COptionGroup &)),
            m_editEvents, SLOT(setOptions(COptionGroup &)));

    if (m_editEvents) {
        m_editEvents->hide();
    }

    emit fontChanged(currentFont());
    emit editorOptionChanged((*m_options)[EDITOR]);
}


QFont MainWindow::currentFont()
{
    return QFont(O_STR(EDITOR, FONT_NAME), O_INT(EDITOR, FONT_SIZE), QFont::DemiBold);
}

QElapsedTimer & MainWindow::getTime()
{
    return m_time;
}

void MainWindow::showStatus(const QString msg)
{
    ui->statusBar->showMessage(msg);
}

void MainWindow::setStatus(int i, const QString message)
{
    m_labels[i]->setText(message);
}

void MainWindow::hideView(bool hide)
{
    if (hide) {
        m_scroll->hide();
        ui->centralWidget->show();
    } else {
        m_scroll->show();
        ui->centralWidget->hide();
    }
}

void MainWindow::focusInEvent ( QFocusEvent * event )
{
   // m_lview->setFocus();
    event->ignore();
}

MainWindow::~MainWindow()
{
    m_timer.stop();
    m_timerIndicator.stop();
    delete ui;
    delete static_cast<CMusicSDL*>(m_doc.music());
    m_doc.attach(static_cast<IMusic*>(nullptr));
    delete static_cast<CSndSDL*>(m_doc.sound());
    m_doc.attach(static_cast<ISound*>(nullptr));
    while (m_updater && m_updater->isRunning());
    if (m_updater) {
        delete m_updater;
    }
    if (m_options) {
        delete m_options;
    }
    if (m_fixer) {
        delete m_fixer;
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

void MainWindow::open(QString fileName)
{
    commitAll();
    if (maybeSave()) {
        if (fileName.isEmpty()) {
            fileName = QFileDialog::getOpenFileName(nullptr, tr("Open"), fileName, tr("LGCK Projects (*.lgckdb)"));
        }
        loadFileName(fileName);
    }
    updateMenus();
    m_doc.initFonts();
}

void MainWindow::loadFileName(const QString & fileName)
{
    if (!fileName.isEmpty()) {
        QString oldFileName = m_fileName;
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_doc.setFileName(fileName.toStdString().c_str());
        m_fileName = fileName;
        QFileWrap file;
        if (!file.open(fileName) || !m_doc.read(file))  {
            warningMessage(tr("cannot open file:\n") + m_doc.getLastError());
            m_doc.forget();
            m_doc.init();
            m_doc.setFileName(q2c(oldFileName));
            m_fileName = oldFileName;
            // update fileList
            QSettings settings;
            QStringList files = settings.value("recentFileList").toStringList();
            files.removeAll(fileName);
            settings.setValue("recentFileList", files);
        }
        QApplication::restoreOverrideCursor();
        updateRecentFileActions();
        reloadRecentFileActions();
        memorizeFilePath();
    }
    updateTitle();
    m_lview->makeCurrent();
    m_doc.cacheImages();
    m_toolBox->init();
    showLayerName();
    reloadLayerCombo();
    setViewMode(VM_EDITOR);
    emit levelSelected(0);
    updateMenus();
    m_doc.initFonts();
}

bool MainWindow::save()
{
    commitAll();
    QString oldFileName = m_fileName;
    if (m_doc.isUntitled()) {
        if (!saveAs())
            return false;
    }

    QFileWrap file;
    if (!file.open(m_fileName, "wb") || !m_doc.write(file) || !updateTitle()) {
        warningMessage(tr("Can't write file"));
        m_doc.setFileName(q2c(oldFileName));
        m_fileName = oldFileName;
        return false;
    }
    m_doc.setDirty(false);
    updateRecentFileActions();
    reloadRecentFileActions();
    return true;
}

bool MainWindow::saveAs()
{
    commitAll();
    QString fileName =
            QFileDialog::getSaveFileName(
                this,
                tr("Save As"),
                m_doc.getFileName(),
                tr("LGCK Projects (*.lgckdb)")
            );
    if (fileName.isEmpty())
        return false;
    if (!fileName.endsWith(".lgckdb", Qt::CaseInsensitive)) {
        fileName.append(".lgckdb");
    }
    m_doc.setFileName(q2c(fileName));
    m_fileName = fileName;
    memorizeFilePath();
    return true;
}

void MainWindow::warningMessage(const QString message)
{
    QMessageBox::warning(this, m_appName, message);
}

bool MainWindow::updateTitle()
{
    QString file;
    if (!m_doc.getFileName()[0]) {
        file = tr("untitled");
    } else {
        file = QFileInfo(m_doc.getFileName()).fileName();
    }
    setWindowTitle(tr("%1[*] - %2").arg(file, m_appTitle));
    return true;
}

void MainWindow::on_actionPrevious_triggered()
{
    if (m_doc.m_nCurrLevel) {
        --m_doc.m_nCurrLevel;
       // m_lview->setFocus();
        repaint();
        updateMenus();
        showLayerName();
        reloadLayerCombo();
        emit levelSelected(m_doc.m_nCurrLevel);
    }
}

void MainWindow::on_actionNext_triggered()
{
    if (m_doc.m_nCurrLevel < m_doc.getSize() - 1 ) {
        ++m_doc.m_nCurrLevel;
      //  m_lview->setFocus();
        repaint();
        updateMenus();
        showLayerName();
        reloadLayerCombo();
        emit levelSelected(m_doc.m_nCurrLevel);
    }
}

void MainWindow::resizeEvent (QResizeEvent *event)
{
    event->accept();    
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
    leaveGameMode();
    saveSettings();
    CLuaVM::setCallback(CLuaVM::Debug, nullptr);
    CLuaVM::setCallback(CLuaVM::Error, nullptr);
}

void MainWindow::on_actionOpen_triggered()
{
    open("");
    //m_lview->setFocus();
    updateMenus();
}

void MainWindow::on_actionSave_triggered()
{
    save();
   // m_lview->setFocus();
}

void MainWindow::on_actionSave_as_triggered()
{
    if (saveAs()) {
        save();
    }
   // m_lview->setFocus();
}

void MainWindow::on_actionRevert_triggered()
{
    if (m_doc.isDirty()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr(m_appName),
                     tr("The document has been modified.\n"
                        "If you revert it back, all changes will be lost."),
                     QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Discard) {
            m_doc.setDirty(false);
            open(m_doc.getFileName());
        }
    }
}

void MainWindow::openRecentFile()
{    
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        open(action->data().toString());
    }
  //  m_lview->setFocus();
    updateMenus();
}

void MainWindow::initFileMenu()
{
    // gray out the open recent `nothin' yet`
    ui->action_nothin_yet->setEnabled(false);
    for (int i = 0; i < MAX_PROJECTS; i++) {
        m_recentFileActs[i] = new QAction(this);
        m_recentFileActs[i]->setVisible(false);
        ui->menuOpen_recent->addAction(m_recentFileActs[i]);
        connect(m_recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
    reloadRecentFileActions();
    // connect the File->Quit to the close app event
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    ui->actionQuit->setMenuRole(QAction::QuitRole);
}

void MainWindow::reloadRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    int numRecentFiles = qMin(files.size(), (int)O_INT(GENERAL, LAST_PROJECTS));
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        m_recentFileActs[i]->setText(text);
        m_recentFileActs[i]->setData(files[i]);
        m_recentFileActs[i]->setVisible(true);
        m_recentFileActs[i]->setStatusTip(files[i]);
    }
    for (int j = numRecentFiles; j < (int)O_INT(GENERAL, LAST_PROJECTS); ++j)
        m_recentFileActs[j]->setVisible(false);
    ui->action_nothin_yet->setVisible(numRecentFiles == 0);
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    QString fileName = m_doc.getFileName();
    files.removeAll(fileName);
    if (!fileName.isEmpty()) {
        files.prepend(fileName);
        while (files.size() > O_INT(GENERAL, LAST_PROJECTS))
            files.removeLast();
    }
    settings.setValue("recentFileList", files);
}

void MainWindow::on_actionFirst_level_triggered()
{
    if (m_doc.m_nCurrLevel) {
        m_doc.m_nCurrLevel = 0;
        repaint();
        updateMenus();
        showLayerName();
        reloadLayerCombo();
        emit levelSelected(m_doc.m_nCurrLevel);
    }
}

void MainWindow::on_actionLast_level_triggered()
{
    if (m_doc.getSize()) {
        m_doc.m_nCurrLevel = m_doc.getSize() - 1;
        repaint();
        updateMenus();
        showLayerName();
        reloadLayerCombo();
        emit levelSelected(m_doc.m_nCurrLevel);
    }
}

void MainWindow::on_actionEdit_Level_triggered()
{
    if (m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        CDlgEditLevel *dlg = new CDlgEditLevel(this);
        dlg->setGameDB(&m_doc);
        QString s = QString::asprintf(q2c(tr("Edit level %.3d")), m_doc.m_nCurrLevel + 1);
        dlg->setWindowTitle(s);
        dlg->load(& level);
        if (dlg->exec() == QDialog::Accepted) {
            dlg->save(& level);
            m_doc.setDirty( true );
        }
        delete dlg;
        emit levelEdited( m_doc.m_nCurrLevel );
    }
}

void MainWindow::on_actionEdit_Game_Info_triggered()
{
    CDlgGame *d = new CDlgGame(this);
    d->setGameDB(&m_doc);
    d->init();
    d->exec();
    m_toolBox->reload();
   // m_lview->setFocus();
    showLayerName();
    reloadLayerCombo();
    delete d;
}

void MainWindow::paintEvent(QPaintEvent * event) {
    event->accept();
}

void MainWindow::updateMenus()
{
    QAction * actionsGame[] = {
        ui->actionDebug,
        ui->actionQuit_Game,
        ui->actionPause,
        ui->actionRestart
    };

    for (uint32_t i=0; i < sizeof(actionsGame)/sizeof(QAction*); ++i) {
        actionsGame[i]->setEnabled( m_viewMode == VM_GAME );
    }

    for (int j = 0; j < O_INT(GENERAL, LAST_PROJECTS); ++j) {
        m_recentFileActs[j]->setEnabled( m_viewMode != VM_GAME );
    }

    if (m_viewMode == VM_GAME ||
            m_viewMode == VM_EDIT_PATH) {
        QAction * actionsEditor[] = {
            ui->actionNew_file,
            ui->actionOpen,
            ui->actionSave,
            ui->actionSave_as,
            ui->actionPrevious,
            ui->actionNext,
            ui->actionCreate_Level,
            ui->actionMove_Level,
            ui->actionDelete_Level,
            ui->actionRemove_All,
            ui->actionMark_All_as_Goals,
            ui->actionUnmark_All_as_Goals,
            ui->actionCopy_Object,
            ui->actionDelete_Object,
            ui->actionSend_to_back,
            ui->actionBring_to_front,
            ui->actionCustomize,
            ui->actionEdit_Object,
            ui->actionEdit_Images,
            ui->actionGo_to_level,
            ui->actionFirst_level,
            ui->actionLast_level,
            ui->actionRevert,
            ui->action_nothin_yet,
            ui->actionEdit_Game_Info,
            ui->actionEdit_Level,
            ui->actionTest_Level,
            ui->action_ShowGrid,
            ui->actionCreate_layer,
            ui->actionDelete_layer,
            ui->actionGo_to_layer,
            ui->actionMove_Layer,
            ui->actionNext_layer,
            ui->actionPrevious_layer,
            ui->actionEdit_layer,
            ui->actionUndo,
            ui->actionRedo,
            ui->actionCopy,
            ui->actionCut,
            ui->actionPaste,
            ui->actionSearch,
            ui->actionDelete,
            ui->actionEdit_Path,
            ui->actionView_Code,
            ui->actionNormal,
            ui->actionHell,
            ui->actionNightmare,
            ui->actionInsane,
            ui->actionAll_Skills
        };
        for (uint32_t i=0; i < sizeof(actionsEditor)/sizeof(QAction*); ++i) {
            actionsEditor[i]->setEnabled(false);
        }
        m_comboEvents->setEnabled(false);
        m_comboLayers->setEnabled(false);
    } else {
        bool result = m_doc.getSize() != 0;
        ui->actionNew_file->setEnabled(true);
        ui->actionOpen->setEnabled(true);
        ui->actionSave->setEnabled(true);
        ui->actionSave_as->setEnabled(true);
        ui->action_ShowGrid->setEnabled(true);
        ui->actionAbout->setEnabled(true);
        ui->actionCreate_Level->setEnabled(true);
        ui->actionEdit_Game_Info->setEnabled(true);
        ui->actionTest_Level->setEnabled( result );
        ui->actionEdit_Level->setEnabled( result );
        ui->actionMove_Level->setEnabled ( result );
        ui->actionDelete_Level->setEnabled ( result );
        ui->actionFirst_level->setEnabled ( result );
        ui->actionLast_level->setEnabled ( result );
        ui->actionGo_to_level->setEnabled ( result );
        ui->actionNext->setEnabled ( m_doc.m_nCurrLevel < m_doc.getSize() - 1 );
        ui->actionPrevious->setEnabled ( m_doc.m_nCurrLevel > 0 );
        ui->actionCreate_layer->setEnabled( result );

        bool resultSingle = result;
        bool resultMulti = result;
        if (result) {
            CLevel & level = m_doc.getCurrentLevel();
            CLayer & layer = * level.getCurrentLayer();
            resultSingle = layer.isSingleSelection();
            resultMulti = layer.getSelectionSize();
            ui->actionGo_to_layer->setEnabled(true);
            ui->actionDelete_layer->setEnabled( layer.getType() != CLayer::LAYER_MAIN );
            ui->actionMove_Layer->setEnabled( level.getSize() > 1 );
            ui->actionNext_layer->setEnabled( level.getCurrentLayerById() < level.getSize() - 1);
            ui->actionPrevious_layer->setEnabled( level.getCurrentLayerById() );
            ui->actionEdit_layer->setEnabled( layer.getType() != CLayer::LAYER_MAIN );
        } else {
            ui->actionDelete_layer->setEnabled( false );
            ui->actionGo_to_layer->setEnabled( false );
            ui->actionMove_Layer->setEnabled( false );
            ui->actionNext_layer->setEnabled( false );
            ui->actionPrevious_layer->setEnabled(false );
            ui->actionEdit_layer->setEnabled( false );
        }

        bool enable = resultSingle;
        bool enableMulti = resultMulti;
        if (m_viewMode == VM_SPRITE_EVENTS) {
            enable = false;
            enableMulti = false;
            ui->actionCreateSprite->setEnabled( false );
        } else {
            ui->actionCreateSprite->setEnabled( true ) ;
        }
        ui->actionCut->setEnabled( enableMulti );
        ui->actionCopy->setEnabled( enableMulti );
        ui->actionDelete->setEnabled( enableMulti );
        ui->actionPaste->setEnabled( m_viewMode != VM_SPRITE_EVENTS
                && m_doc.getClipboard()->getSize() > 0 );
        ui->actionUndo->setEnabled( false );
        ui->actionRedo->setEnabled( false );
        ui->actionSearch->setEnabled( false );
        ui->actionMark_All_as_Goals->setEnabled( enableMulti );
        ui->actionUnmark_All_as_Goals->setEnabled( enableMulti );

        ui->actionRemove_All->setEnabled( enableMulti );
        ui->actionBring_to_front->setEnabled( enableMulti );
        ui->actionSend_to_back->setEnabled( enableMulti );
        ui->actionCopy_Object->setEnabled( enableMulti );
        ui->actionDelete_Object->setEnabled( enableMulti );
        ui->actionCustomize->setEnabled( enableMulti );
        ui->actionEdit_Object->setEnabled( enable );
        ui->actionEdit_Images->setEnabled( enable );
        ui->actionEdit_Path->setEnabled( enable );
        if (result && m_doc.getSize()) {
            CLevel & level = m_doc.getCurrentLevel();
            CLayer & layer = * level.getCurrentLayer();
            result = false;
            if (layer.isSingleSelection()) {
                CLevelEntry & entry = layer.getSelection(0);
                CProto & proto = m_doc.m_arrProto[entry.m_nProto];
                result = !CGame::isBackgroundClass(proto.m_nClass);
            }
        } else {
            result = false;
        }
        ui->actionView_Code->setEnabled( result && m_viewMode != VM_EDIT_PATH);
        m_comboEvents->setEnabled( result );
        m_comboLayers->setEnabled( m_doc.getSize() != 0
                && m_comboLayers->count() > 1 && (m_viewMode != VM_EDIT_PATH)
                && m_viewMode != VM_SPRITE_EVENTS);
        ui->actionEdit_Path->setEnabled(result && (m_viewMode != VM_EDIT_PATH)
                && m_viewMode != VM_SPRITE_EVENTS);
        if (result) {
            reloadEventCombo();
        } else {
            m_comboEvents->setCurrentIndex(0);
        }
        ui->actionNormal->setEnabled(true);
        ui->actionHell->setEnabled(true);
        ui->actionNightmare->setEnabled(true);
        ui->actionInsane->setEnabled(true);
        ui->actionAll_Skills->setEnabled(true);
    }
    ui->actionScriptWizard->setEnabled(m_viewMode == VM_SPRITE_EVENTS);
}

void MainWindow::on_actionCreate_Level_triggered()
{
    CDlgEditLevel *dlg = new CDlgEditLevel(this);
    dlg->setGameDB(&m_doc);
    dlg->setWindowTitle(tr("Create New Level"));
    CLevel * level = new CLevel;
    level->setSetting("title", q2c(tr("untitled %1").arg(m_doc.getSize() + 1)));
    level->setSetting("hint", q2c(tr("put your hints here...")));
    dlg->setNewLevel();
    dlg->load( level );
    if (dlg->exec() == QDialog::Accepted) {
        if (m_doc.getSize()==0) {
            // init font on first level
            m_doc.initFonts();
        }
        dlg->save( level );
        m_doc.addLevel( level );
        m_doc.setDirty( true );
        m_doc.m_nCurrLevel = m_doc.getSize() - 1;
        updateMenus();
        showLayerName();        
        reloadLayerCombo();
        emit levelAdded();
        emit levelSelected(m_doc.m_nCurrLevel);
    } else {
        delete level;
    }
    delete dlg;
}

void MainWindow::on_actionDelete_Level_triggered()
{
    if (m_doc.getSize()) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr(m_appName),
                     tr("Are you sure that you want to delete\n"
                        "this level?"),
                     QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Ok) {
            m_doc.setDirty( true );
            int index = m_doc.m_nCurrLevel;
            delete & m_doc.getCurrentLevel();
            m_doc.removeLevel ( m_doc.m_nCurrLevel );
            if ( m_doc.m_nCurrLevel > m_doc.getSize() - 1) {
                -- m_doc.m_nCurrLevel ;
            }
            if (!m_doc.getSize()) {
                m_doc.m_nCurrLevel = -1;
            }
            emit levelSelected(m_doc.m_nCurrLevel);
            showLayerName();
            reloadLayerCombo();
            updateMenus();
            emit levelDeleted(index);
        }
    }
}

void MainWindow::on_actionGo_to_level_triggered()
{
    if (m_doc.getSize()) {
        CDlgGotoLevel *dlg = new CDlgGotoLevel(this);
        dlg->setWindowTitle(tr("Go to level..."));
        dlg->setGameDB(& m_doc);
        dlg->init();
        dlg->setLevelId( m_doc.m_nCurrLevel );
        if (dlg->exec() == QDialog::Accepted) {
            m_doc.m_nCurrLevel = dlg->getLevelId();
            updateMenus();
            showLayerName();
            reloadLayerCombo();
            emit levelSelected(m_doc.m_nCurrLevel);
        }
        delete dlg;
    }
}

void MainWindow::on_actionMove_Level_triggered()
{
    if (m_doc.getSize()) {
        CDlgGotoLevel *dlg = new CDlgGotoLevel(this);
        dlg->setWindowTitle(tr("Move level to..."));
        dlg->setGameDB(& m_doc);
        dlg->init();
        dlg->setLevelId( m_doc.m_nCurrLevel );
        int indexFrom = m_doc.m_nCurrLevel;
        if (dlg->exec() == QDialog::Accepted
            && m_doc.m_nCurrLevel != dlg->getLevelId()) {
            int levelId = dlg->getLevelId();
            CLevel & level = m_doc.getCurrentLevel();
            m_doc.removeLevel(m_doc.m_nCurrLevel);
            m_doc.insertLevel(levelId, &level);
            m_doc.m_nCurrLevel = levelId;
            m_doc.setDirty( true );
            updateMenus();
            showLayerName();
            reloadLayerCombo();
            emit levelMoved(indexFrom, levelId);
        }
        delete dlg;
    }
}

void MainWindow::on_actionDelete_Object_triggered()
{
    if (m_doc.getSize()
            && m_viewMode == VM_EDITOR) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer *layer = level.getCurrentLayer();
        if (layer->isMultiSelection()
                || layer->isSingleSelection()) {
            layer->removeSelectedSprites();
            m_doc.setDirty(true);
            m_lview->repaint();
            updateMenus();
        }        
    }
}

void MainWindow::on_actionRemove_All_triggered()
{
    if (m_doc.getSize()
            && m_viewMode == VM_EDITOR) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = *(level.getCurrentLayer());
        if (layer.getSelectionSize()) {
            QString msg;
            if (layer.isSingleSelection()) {
                msg = tr("Do you want to deleted all instances of this sprite on\n" \
                   "the current layer?");
            } else {
                msg = tr("Do you want to deleted all instances of the selected sprites on\n" \
                   "the current layer?");
            }
            QMessageBox::StandardButton ret =  QMessageBox::warning(this, tr(m_appName), msg  ,
                     QMessageBox::Yes | QMessageBox::No);
            if (ret == QMessageBox::Yes) {
                for (int k=0; k < layer.getSelectionSize(); ++k) {
                    CLevelEntry obj = layer.getSelection(k);                    
                    int j=0;
                    for (int i=0; i < layer.getSize(); ++i) {
                        if (layer[i].m_nProto != obj.m_nProto) {
                            if (i != j) {
                                layer[j] = layer[i];
                            }
                            ++j;
                        }
                    }
                    layer.setSize(j);
                }
                layer.clearSelection();
                m_doc.setDirty(true);
            }
            m_lview->repaint();
            updateMenus();
        }
    }
}

void MainWindow::on_actionSend_to_back_triggered()
{
    // TODO: implement multi select
    if (m_doc.getSize()
            && m_viewMode == VM_EDITOR) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        int selCount = layer.getSelectionSize();
        if (selCount) {
            // send to back
            for (int i = selCount -1 ; i >= 0 ; --i) {
                CLevelEntry entry = layer.getSelection(i);
                int k = layer.getSelectionIndex(i);
                layer.removeAt( k );
                layer.insertAt(0, entry);

                // adjust index of the other entries
                for (int j = i - 1; j >= 0; --j ) {
                    int index = layer.getSelectionIndex( j );
                    if (index < k) {
                        layer.setSelectionIndex(j, ++index);
                    }
                }
            }

            // clear the old selection
            layer.clearSelection();

            // select new entries
            for (int i=0; i < selCount; ++i) {
                layer.select( i);
            }
            m_doc.setDirty(true);
            m_lview->repaint();
            updateMenus();
        }
    }
}

void MainWindow::on_actionBring_to_front_triggered()
{
    if (m_doc.getSize()
            && m_viewMode == VM_EDITOR) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        int selCount = layer.getSelectionSize();
        if (selCount) {
            int layerSize = layer.getSize();
            // bring to top of the pile
            for (int i=0; i < selCount; ++i) {
                CLevelEntry entry = layer.getSelection(i);
                int k = layer.getSelectionIndex(i);
                layer.removeAt( k );
                layer.add(entry);
                // adjust index of the other entries
                for (int j = i + 1; j < selCount; ++j ) {
                    int index = layer.getSelectionIndex( j );
                    if (index > k) {
                        layer.setSelectionIndex(j, --index);
                    }
                }
            }
            // clear the old selection
            layer.clearSelection();
            // select new entries
            for (int i=0; i < selCount; ++i) {
                layer.select(layerSize - selCount + i);
            }
            m_doc.setDirty(true);
            m_lview->repaint();
            updateMenus();
        }
    }
}

void MainWindow::on_actionNew_file_triggered()
{
    commitAll();
    if (maybeSave()) {
        CWizGame *wiz = new CWizGame();
        wiz->init(&m_doc);
        if (wiz->exec()) {
            m_doc.forget();
            m_doc.init();
            wiz->save();
            initToolBox();
            m_doc.cacheImages();
            updateMenus();
            m_toolBox->init();
            setViewMode(VM_EDITOR);
            m_doc.setDirty(true);
            on_actionCreate_Level_triggered();
        }
        delete wiz;
    }
    updateTitle();
}

void MainWindow::on_actionAbout_triggered()
{
    QString vendor;
    QString renderer;
    QString version;
    QString extensions;
    m_scroll->getGLInfo(vendor, renderer, version, extensions);
    CDlgAbout *d = new CDlgAbout (this);
    d->setGLInfo(vendor, renderer, version, extensions);
    d->exec();
    delete d;
}

void MainWindow::on_actionTest_Level_triggered()
{
    if (m_doc.getSize() && !(m_viewMode == VM_GAME)) {
        m_doc.initFonts();
        m_doc.countdowns()->removeAll();
        CDlgTestLevel *dlg = new CDlgTestLevel(this);
        dlg->setSkill(O_INT(TESTLEVEL, SKILL));
        dlg->setHP(O_INT(TESTLEVEL, START_HP));
        dlg->setScore(O_INT(TESTLEVEL, SCORE));
        dlg->setLives(O_INT(TESTLEVEL, LIVES));
        dlg->setContinue(O_BOOL(TESTLEVEL, CONTINUE));
        dlg->setExternal(m_runtimeExternal);
        dlg->setRez(m_rez);
        CLevel & level = m_doc.getCurrentLevel();
        dlg->analyseLevel(&level);
        if (dlg->exec()) {
            O_SET(TESTLEVEL, SKILL, dlg->getSkill());
            O_SET(TESTLEVEL, START_HP, dlg->getHP());
            O_SET(TESTLEVEL, SCORE, dlg->getScore());
            O_SET(TESTLEVEL, LIVES, dlg->getLives());
            O_SET(TESTLEVEL, CONTINUE, dlg->getContinue());
            m_rez = dlg->getRez();
            m_rezH = dlg->getHeight();
            m_rezW = dlg->getWidth();
            qDebug("skill: %d", O_INT(TESTLEVEL, SKILL));
            QString errMsg = "";
            if (!m_doc.getSize()) {
                errMsg = tr("No level available to play.");
            } else {
                int currLevel = m_doc.m_nCurrLevel;
                CLevel * level = m_doc[currLevel];
                if (!level) {
                    errMsg = tr("No valid level found.");
                } else {
                    CLayer * layer = level->getMainLayer();
                    if (!layer) {
                        errMsg = tr("No main layer detected.");
                    } else {
                        if (layer->getSize() < 1) {
                            errMsg = tr("Level is empty.");
                        }
                    }
                }
            }

            if (errMsg.isEmpty()) {
                m_doc.clearKeys();
                m_doc.resetAllCounters();
                m_doc.setVitals(
                            O_INT(TESTLEVEL, START_HP),
                            O_INT(TESTLEVEL, LIVES),
                            O_INT(TESTLEVEL, SCORE)
                            );
                m_doc.setSkill(O_INT(TESTLEVEL, SKILL));
                m_doc.initLua();
                // disable external runtime
                m_runtimeExternal = false;//dlg->isExternal();
                if (m_runtimeExternal){
                    goExternalRuntime();
                } else {
                    // set the start level
                    m_start_level = m_doc.m_nCurrLevel;
                    testLevel(true);
                }
            } else {
                 QMessageBox::warning(this, m_appName, errMsg);
            }
        }
        delete dlg;
    }
    m_lview->setFocus();
}

void MainWindow::testLevel(bool initSound)
{   
    QSplashScreen splash(QPixmap(":/images/0000-1.png"));
    splash.show();
    splash.showMessage(tr("Please wait..."));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (initSound) {
        m_doc.snapshot().clear();
        m_doc.setEngineState(CGame::ES_INTRO);
        m_doc.initSounds();
        emit focusGL();
        m_doc.initSettings();
        m_doc.callGameEvent(CGameEvents::EG_INIT_GAME);
    }
    m_doc.var("level") = m_doc.m_nCurrLevel;
    setViewMode(VM_GAME);
    splash.finish(this);
    QApplication::restoreOverrideCursor();
}

void MainWindow::handleGameEvents()
{
    int result = m_doc.runEngine();
    if (result) {
        m_doc.stopMusic();
        setViewMode( VM_EDITOR );
        m_doc.clearKeys();
        bool restoreEditor = true;
        switch (result) {
        case CGame::EVENT_QUIT:
            break;
        case CGame::EVENT_LEVEL_COMPLETED:
            m_doc.snapshot().clear();
            if (m_doc.svar("WarpTo") == CGame::INVALID) {
                // we're not warping to another level
                if (m_doc.m_nCurrLevel < m_doc.getSize() - 1) {
                    ++m_doc.m_nCurrLevel;
                } else {
                    qDebug(">>>> images: %d", m_doc.frames().getSize());
                    QMessageBox::information(this, tr(m_appName),
                                             tr("You have sucessfully completed this level."));
                    break;
                }
            } else {
                m_doc.m_nCurrLevel = m_doc.svar("WarpTo");
            }
            m_doc.setEngineState(CGame::ES_INTRO);
            testLevel(false);
            restoreEditor = false;
            break;

        case CGame::EVENT_TIMEOUT:
            if (!O_BOOL(TESTLEVEL, CONTINUE)) {
                QMessageBox::information(this, tr(m_appName),
                                     tr("You ran out of time."));
            }
            /* fall through */

        case CGame::EVENT_PLAYER_DIED:
            --m_doc.counter("lives");
            if (!O_BOOL(TESTLEVEL, CONTINUE)) {
                QMessageBox::information(this, tr(m_appName),
                                         tr("You were killed."));
            } else {
                if (!m_doc.counter("lives")) {
                    QMessageBox::information(this, tr(m_appName),
                                             tr("Game Over"));
                    break;
                }
                if (result==CGame::EVENT_TIMEOUT) {
                    m_doc.setEngineState(CGame::ES_TIMEOUT);
                } else {
                    m_doc.setEngineState(CGame::ES_INTRO);
                }
                testLevel(false);
                restoreEditor = false;
            }
            break;

        case CGame::EVENT_NO_PLAYER:
            QMessageBox::information(this, tr(m_appName),
                                     tr("No player object."));
            break;

        default:
            QMessageBox::warning(this, tr(m_appName),
                                 QString(tr("unknown event: %1")).arg(result));
            break;
        }
        if (restoreEditor) {
            qDebug("restore editor");
            // set the start level
            m_doc.m_nCurrLevel = m_start_level;
            emit levelSelected(m_start_level);
            emit focusGL();
            leaveGameMode();
        }
    }
}

void MainWindow::viewEvent()
{
   // m_lview->setFocus();
    switch (m_viewMode) {
    case VM_GAME:
        handleGameEvents();
        break;

    case VM_EDITOR:
        if (m_time.elapsed() > m_nextTick) {
            m_nextTick = m_time.elapsed() + TICK_SCALE;
            m_scroll->eventHandler();
        }
        break;

    case VM_EDIT_PATH:
        if (m_time.elapsed() > m_nextTick) {
            m_nextTick = m_time.elapsed() + TICK_SCALE;
            m_scroll->editPath();
        }
    }
}

void MainWindow::on_actionConfigure_LGCK_Builder_triggered()
{
    showAppSettings(CDlgAppSettings::TAB_DEFAULT);
}

void MainWindow::showAppSettings(int tab)
{
    CDlgAppSettings *d = new CDlgAppSettings(this);
    connect(d, SIGNAL(versionCheck()), this, SLOT(checkVersion()));
    QString s = QString(tr("%1 Settings").arg(m_appName));
    d->setWindowTitle(s);
    d->showGrid(O_BOOL(GENERAL, SHOWGRID));
    d->setGridColor(O_STR(GENERAL, GRIDCOLOR));
    d->setGridSize(O_INT(GENERAL, GRIDSIZE));
    d->setTriggerKeyColor(O_STR(GENERAL, TRIGGER_KEY_COLOR));
    d->setShowTriggerKey(O_BOOL(GENERAL, TRIGGER_KEY_SHOW));
    d->setUpdater(m_bUpdate, m_updateURL);
    d->setFontSize(O_INT(EDITOR, FONT_SIZE));
    d->setFont(O_STR(EDITOR, FONT_NAME));
    d->enableWordWrap(O_BOOL(EDITOR, ENABLE_WORDWRAP));
    d->enableHighlight(O_BOOL(EDITOR, ENABLE_HIGHLIGHT));
    d->enableWhiteSpace(O_BOOL(EDITOR, ENABLE_WHITESPACE));
    d->enableAutocomplete(O_BOOL(EDITOR, ENABLE_AUTO_COMPLETE));
    d->setLastProjects(O_INT(GENERAL, LAST_PROJECTS));
    d->setTickMaxRate(O_INT(GENERAL, XTICK_MAX_RATE));
    d->setLastFolder(O_BOOL(GENERAL, LAST_FOLDER));
    d->setCurrentTab(tab);
    QAction **actions = actionShortcuts();
    QStringList listActions;
    QStringList listShortcuts;
    for (uint i=0; actions[i];++i) {
        QAction *a = actions[i];
        QKeySequence s = a->shortcut();
        listShortcuts.append(s.toString());
        listActions.append(a->text());
    }
    d->setSkill(O_INT(TESTLEVEL, SKILL));
    d->setHP(O_INT(TESTLEVEL, START_HP));
    d->setScore(O_INT(TESTLEVEL, SCORE));
    d->setLives(O_INT(TESTLEVEL, LIVES));
    d->setContinue(O_BOOL(TESTLEVEL, CONTINUE));
    d->setRuntime(m_runtime, m_runtimeArgs);
    d->setSkipSplashScreen(O_BOOL(EDITOR, SKIP_SPLASH));
    d->setTriggerFontSize(O_INT(GENERAL, TRIGGER_KEY_FONT_SIZE));
    d->setJoyButtons(& m_doc.joyStateEntry(0));
    d->init();
    d->load(listActions, listShortcuts, defaultShortcuts());
    if (d->exec() == QDialog::Accepted) {
        d->save(listShortcuts);
        d->getUpdater(m_bUpdate, m_updateURL);
        for (uint i=0; actions[i];++i) {
            QAction *a = actions[i];
            a->setShortcut(QKeySequence(listShortcuts[i]));
        }
        O_SET(GENERAL, SHOWGRID, d->isShowGrid());
        emit gridVisible(O_BOOL(GENERAL, SHOWGRID));
        emit gridSizeChanged(d->getGridSize());
        O_SET(GENERAL, GRIDSIZE, d->getGridSize());
        emit gridColorChanged(d->getGridColor());
        O_SET(GENERAL, GRIDCOLOR, d->getGridColor().mid(0,6));
        O_SET(GENERAL, TRIGGER_KEY_COLOR, d->getTriggerKeyColor().mid(0,6));
        ui->action_ShowGrid->setChecked(O_BOOL(GENERAL, SHOWGRID));
        m_lview->repaint();
        O_SET(TESTLEVEL, SKILL, d->getSkill());
        O_SET(TESTLEVEL, START_HP, d->getHP());
        O_SET(TESTLEVEL, SCORE, d->getScore());
        O_SET(TESTLEVEL, LIVES, d->getLives());
        O_SET(TESTLEVEL, CONTINUE, d->getContinue());
        O_SET(EDITOR, FONT_SIZE, d->getFontSize());
        O_SET(EDITOR, FONT_NAME, d->getFont());
        O_SET(EDITOR, SKIP_SPLASH, d->getSkipSplashScreen());
        O_SET(EDITOR, ENABLE_WORDWRAP, d->wordWrap());
        O_SET(EDITOR, ENABLE_HIGHLIGHT, d->highlight());
        O_SET(EDITOR, ENABLE_WHITESPACE, d->whiteSpace());
        O_SET(EDITOR, ENABLE_AUTO_COMPLETE, d->autocomplete());
        O_SET(GENERAL, TRIGGER_KEY_SHOW, d->getShowTriggerKey());
        O_SET(GENERAL, TRIGGER_KEY_FONT_SIZE, d->getTriggerFontSize());
        emit triggerKeyFontSizeChanged(O_INT(GENERAL, TRIGGER_KEY_FONT_SIZE));
        d->getRuntime(m_runtime, m_runtimeArgs);
        emit fontChanged(currentFont());
        emit editorOptionChanged((*m_options)[EDITOR]);
        emit triggerKeyColorChanged(O_STR(GENERAL, TRIGGER_KEY_COLOR));
        emit triggerKeyShow(O_BOOL(GENERAL, TRIGGER_KEY_SHOW));
        O_SET(GENERAL, LAST_PROJECTS, d->lastProjects());
        O_SET(GENERAL, XTICK_MAX_RATE, d->tickMaxRate());
        O_SET(GENERAL, LAST_FOLDER, d->lastFolder());
        changeTickMaxRate();
        d->getJoyButtons(& m_doc.joyStateEntry(0));
    }
    delete d;
}

void MainWindow::showToolBox(bool show)
{
    m_bShowToolBox = show;
    show ? m_toolBox->show() : m_toolBox->hide();
    ui->action_ShowToolbox->setChecked( m_bShowToolBox );
}

void MainWindow::showLayerName()
{
    if (m_doc.m_nCurrLevel != -1
        && m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        QString s = QString("%1: %2").arg(m_doc.m_nCurrLevel+1)
                    .arg(layer.getName());
        setStatus (2, s);
    } else {
        setStatus (2, QString(""));
    }
}

void MainWindow::on_actionGo_to_layer_triggered()
{
    if (m_doc.m_nCurrLevel != -1
        && m_doc.getSize()) {
        CDlgGotoLevel * dlg = new CDlgGotoLevel(this);
        dlg->setWindowTitle(tr("goto Layer"));
        dlg->setGameDB(&m_doc);
        dlg->initLayers(m_doc.m_nCurrLevel);
        if (dlg->exec()) {
            CLevel & level = m_doc.getCurrentLevel();
            int layerId = dlg->getLevelId();
            level.setCurrentLayerById(layerId);
            showLayerName();
            reloadLayerCombo();
        }
        delete dlg;
    }
    updateMenus();
}

void MainWindow::on_actionMove_Layer_triggered()
{    
    if (m_doc.m_nCurrLevel != -1
        && m_doc.getSize()) {
        CDlgGotoLevel * dlg = new CDlgGotoLevel(this);
        dlg->setWindowTitle(tr("Move Layer to..."));
        dlg->setGameDB(&m_doc);
        dlg->initLayers(m_doc.m_nCurrLevel);
        if (dlg->exec()) {
            CLevel & level = m_doc.getCurrentLevel();
            int to = dlg->getLevelId();
            int from = level.getCurrentLayerById();
            if (to != from) {
                CLayer * layer = level.removeLayerById(from);
                level.insertAt(to, layer);
                m_doc.setDirty( true );
                level.updateLayerTypes();
            }
            level.setCurrentLayerById(to);
            showLayerName();
            reloadLayerCombo();
        }
        delete dlg;
    }
    updateMenus();
}

void MainWindow::on_actionCreate_layer_triggered()
{
    if (m_doc.m_nCurrLevel != -1
        && m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        CDlgLayer * dlg = new CDlgLayer(this);
        dlg->setWindowTitle(tr("Create new layer"));
        if (dlg->exec()) {
            CLayer *layer = new CLayer( dlg->getType() );
            int layerId;
            if (layer->getType() == CLayer::LAYER_BK) {
                layerId = level.getMainLayerId();
                level.insertAt(layerId, layer);
            } else {
                layerId = level.addLayer(layer);
            }
            level.setCurrentLayerById(layerId);
            QString s;
            int h, v;
            dlg->getParams(s, h, v);
            layer->setName(q2c(s));
            layer->setSpeed(h, v);
            showLayerName();
            reloadLayerCombo();
            m_doc.setDirty( true );
        }
        delete dlg;
    }
    updateMenus();
}

void MainWindow::on_actionDelete_layer_triggered()
{
    if (m_doc.m_nCurrLevel != -1
        && m_doc.getSize()) {

        CLevel & level = m_doc.getCurrentLevel();
        int layerId = level.getCurrentLayerById();
        CLayer * layer = level.getCurrentLayer();
        // TODO: detect main layer by type
        if (layer->getType() != CLayer::LAYER_MAIN) {
            QMessageBox::StandardButton
                    ret = QMessageBox::warning(this, tr(m_appName),
                                               tr("Are you sure that you want to delete the current layer?"),
                                               QMessageBox::Yes| QMessageBox::No
                                               );
            if (ret == QMessageBox::Yes) {
                CLayer * layer = level.removeLayerById(layerId);
                delete layer;
                showLayerName();
                reloadLayerCombo();
                m_doc.setDirty( true );
            }
        } else {
           warningMessage(tr("cannot delete mainLayer"));
        }
    }
    updateMenus();
}

void MainWindow::on_actionPrevious_layer_triggered()
{
    if (m_doc.m_nCurrLevel != -1
        && m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        int layerId = level.getCurrentLayerById();
        if (layerId) {
            level.setCurrentLayerById(--layerId);
            showLayerName();
            reloadLayerCombo();
        }
    }
    updateMenus();
}

void MainWindow::on_actionNext_layer_triggered()
{
    if (m_doc.m_nCurrLevel != -1
        && m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        int layerId = level.getCurrentLayerById();
        if (layerId < level.getSize() - 1) {
            level.setCurrentLayerById(++layerId);
            showLayerName();
            reloadLayerCombo();
        }
    }
    updateMenus();
}

void MainWindow::on_actionEdit_layer_triggered()
{
    if (m_doc.m_nCurrLevel != -1
        && m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        // TODO: detect main layer by type
        CLayer * layer = level.getCurrentLayer();
        if (layer->getType() != CLayer::LAYER_MAIN) {
            int h, v;
            layer->getSpeed(h, v);
            CDlgLayer * dlg = new CDlgLayer(this);
            dlg->setWindowTitle(tr("Edit layer"));
            dlg->setParams(layer->getName(), h, v);
            dlg->setType( layer->getType() );
            dlg->lockType();
            if (dlg->exec()) {
                QString s;
                dlg->getParams(s, h, v);
                layer->setName(q2c(s));
                layer->setSpeed(h, v);
                showLayerName();
                reloadLayerCombo();
                m_doc.setDirty( true );
                layer->setType( dlg->getType() );
            }
            delete dlg;
        } else {
           warningMessage(tr("You cannot modify mainLayer."));
        }
    }
    updateMenus();
}

void MainWindow::on_actionC_declarations_triggered()
{
    QString fileFilter = tr("Header files (*.h)");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export..."), "", fileFilter);
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".h", Qt::CaseInsensitive)) {
            fileName.append(".h");
        }
        QFileWrap file;
        if (file.open(fileName, "wb")) {
            file += "// Constants declarations for C/C++\r\n" \
                    "// ====================================\r\n\r\n" \
                    "// *** OBJECT TYPES\r\n\r\n" ;
            for (int n = 0; n < m_doc.m_arrProto.getSize(); ++n) {
                QString name = QString(m_doc.m_arrProto[n].m_szName).trimmed().toUpper();
                for (int i = 0; i < name.length(); ++i) {
                    if (!name[i].isLetterOrNumber()) {
                        name[i] = '_';
                    }
                }
                QString s = QString::asprintf("#define OBJECT_%-40s %d \r\n", q2c(name), n);
                file += q2c(s);
            }
            file += "\r\n// *** OBJECT CLASSES\r\n\r\n" ;
            for (int n = 0; n < CGame::MAX_CLASSES; ++n) {
                if (!m_doc.m_className[n].empty()) {
                    QString name = QString(m_doc.m_className[n].c_str()).trimmed().toUpper();
                    for (int i = 0; i < name.length(); ++i) {
                        if (!name[i].isLetterOrNumber()) {
                            name[i] = '_';
                        }
                    }
                    QString s = QString::asprintf("#define CLASS_%-40s0x%.2x\r\n",  q2c(name), n);
                    file += q2c(s);
                }
            }
            file.close();
        }  else {
            // write error
            warningMessage( QString(tr("can't write to %1")).arg(fileName) );
        }
    }
}

void MainWindow::on_actionRuntime_Lua_triggered()
{
    QString fileFilter = tr("Lua script (*.lua)");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export..."), "", fileFilter);
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".lua", Qt::CaseInsensitive)) {
            fileName.append(".lua");
        }
        QFileWrap file;
        if (file.open(fileName, "wb")) {
            std::string lua;
            m_doc.generateRuntimeLua(lua);
            file += lua.c_str();
            file.close();
        }  else {
            // write error
            warningMessage(QString(tr("can't write to %1")).arg(fileName) );
        }
    }
}

void MainWindow::initToolBar()
{
    QSize size = QSize(TOOLBAR_ICON_SIZE,TOOLBAR_ICON_SIZE);

    // main toolbar
    ui->toolBar->setObjectName("mainToolbar");
    ui->toolBar->setWindowTitle(tr("Main"));
    ui->toolBar->setIconSize(size);
    ui->toolBar->addAction(ui->actionNew_file);
    ui->toolBar->addAction(ui->actionOpen);
    ui->toolBar->addAction(ui->actionSave);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionCreate_Level);
    ui->toolBar->addAction(ui->actionEdit_Level);
    ui->toolBar->addAction(ui->actionTest_Level);
    m_btnIndicator = new QToolButton(ui->toolBar);
    m_btnIndicator->setIcon(QIcon(":/images/Light-bulb-white.png"));
    m_btnIndicator->setToolTip(tr("Analyzing"));
    m_btnIndicator->resize(size);
    connect(m_btnIndicator, SIGNAL (clicked()), this, SLOT(indicatorTriggered()));
    ui->toolBar->addWidget(m_btnIndicator);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionPrevious);
    ui->toolBar->addAction(ui->actionNext);
    ui->toolBar->addAction(ui->actionGo_to_level);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionSprite_Editor);

    // layer toolbar
    m_comboLayers = new QComboBox(this);
    m_comboLayers->setDisabled(true);
    m_layerToolbar = new QToolBar("Layer", this);
    m_layerToolbar->setIconSize(size);
    m_layerToolbar->setObjectName("toolbar2");
    m_layerToolbar->addWidget(m_comboLayers);
    m_layerToolbar->addAction(ui->actionCreate_layer);
    m_layerToolbar->addAction(ui->actionEdit_layer);
    m_layerToolbar->addAction(ui->actionDelete_layer);
    addToolBar(m_layerToolbar);

    // level toolbar
    m_comboEvents = new QComboBox(this);
    reloadEventCombo();
    m_levelToolbar = new QToolBar(tr("Level"), this);
    m_levelToolbar->setIconSize(size);
    m_levelToolbar->setObjectName("toolbar1");
    m_levelToolbar->addWidget(m_comboEvents);
    m_levelToolbar->addAction(ui->actionScriptWizard);
    m_levelToolbar->addAction(ui->actionEdit_Object);
    m_levelToolbar->addAction(ui->actionCustomize);
    m_levelToolbar->addAction(ui->actionEdit_Path);
    m_levelToolbar->addSeparator();
    m_levelToolbar->addAction(ui->actionCopy);
    m_levelToolbar->addAction(ui->actionCut);
    m_levelToolbar->addAction(ui->actionPaste);
    m_levelToolbar->addAction(ui->actionDelete);
    m_levelToolbar->addSeparator();
    m_levelToolbar->addAction(ui->actionSprite_Paint);
    ui->actionSprite_Paint->setIcon(QIcon(":/images/strawberrypntbrush.png"));
    ui->actionSprite_Paint->setStatusTip(tr("Paint the tile under the cursor with the given sprite"));
    m_actionEraser = m_levelToolbar->addAction(QIcon(":/images/icons8-erase-40.png"), tr("Eraser"));
    m_actionEraser->setCheckable(true);
    m_actionEraser->setStatusTip(tr("Erase the sprite under the cursor"));
    connect(m_actionEraser, SIGNAL(toggled(bool)), this, SLOT(eraserToggled(bool)));
    addToolBar(m_levelToolbar);
    m_protoIcon = new QToolButton();
    m_protoIcon->setToolTip(tr("Sprite painter"));
    m_protoIcon->setStatusTip(tr("Current Sprite selected for painting"));
    m_protoIcon->setPopupMode(QToolButton::MenuButtonPopup);
    m_levelToolbar->addWidget(m_protoIcon);

    connect(m_comboEvents, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));
    connect(m_comboLayers, SIGNAL(currentIndexChanged(int)), this, SLOT(layerChanged(int)));
    connect(m_comboEvents, SIGNAL(highlighted(int)), this, SLOT(reloadEventCombo()));

    connect(ui->toolBar, SIGNAL(visibilityChanged(bool)),
            this, SLOT(on_actionMainToolbar_toggled(bool)));
    connect(m_levelToolbar, SIGNAL(visibilityChanged(bool)),
            this, SLOT(on_actionLevelToolbar_toggled(bool)));
    connect(m_layerToolbar, SIGNAL(visibilityChanged(bool)),
            this, SLOT(on_actionLayer_ToolBar_toggled(bool)));
}

void MainWindow::comboChanged(int v)
{
    if (v != 0){
        --v;
        if (m_viewMode == VM_EDITOR && m_doc.getSize()) {
            CLevel & level = m_doc.getCurrentLevel();
            CLayer & layer = * level.getCurrentLayer();
            if (layer.isSingleSelection()) {
                CLevelEntry entry = layer.getSelection(0);
                if (m_viewMode != VM_SPRITE_EVENTS) {
                    setViewMode(VM_SPRITE_EVENTS);
                }
                m_proto = entry.m_nProto;
                m_event = v;
                emit protoChanged(entry.m_nProto, v);
            }
        } else {
            setViewMode(VM_SPRITE_EVENTS);
            m_event = v;
            emit protoChanged(m_proto, v);
        }
    } else {
        if (m_viewMode != VM_EDITOR) {
            setViewMode(VM_EDITOR);
        }
    }
}

void MainWindow::on_actionPause_triggered()
{
    if (m_viewMode == VM_GAME) {
        m_doc.flipPause();
    }
}

void MainWindow::leaveGameMode()
{
    qDebug("IN leaveGameMode: frames %d", m_doc.frames().getSize());
    qDebug("IN pointsOBL_texture: %llu", m_doc.var("pointsOBL_texture"));
    if (m_viewMode == VM_GAME) {
        setViewMode(VM_EDITOR);
        updateMenus();
        m_doc.stopMusic();
        emit scrollbarShown(true);
    }
    m_doc.removePointsOBL();
    qDebug("OUT leaveGameMode: frames %d", m_doc.frames().getSize());
    qDebug("OUT pointsOBL_texture: %llu", m_doc.var("pointsOBL_texture"));
}

void MainWindow::on_actionQuit_Game_triggered()
{
    leaveGameMode();
}

void MainWindow::on_actionDebug_triggered()
{
    //if (m_viewMode == VM_GAME) {
      //  m_doc.flipDebugConsole();
    //}
}

void MainWindow::on_actionRestart_triggered()
{
    testLevel(false);
}

void MainWindow::on_actionExport_Sprite_triggered()
{
    //warningMessage("Not implemented yet!");
    CDlgExportSprite dlg(this,&m_doc);
    dlg.exec();
}

void MainWindow::on_actionView_Source_triggered()
{
    std::string lua;
    m_doc.generateRuntimeLua(lua);
    CDlgSource dlg;   
    dlg.setText( lua.c_str() );
    dlg.setReadOnly();
    dlg.setWindowTitle(tr("View source (read-only)"));
    dlg.exec();
}

void MainWindow::on_actionEdit_Object_triggered()
{
    if (m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        if (layer.isSingleSelection()) {
            CLevelEntry entry = layer.getSelection(0);
            CProto & currProto = m_doc.m_arrProto[entry.m_nProto];
            const CProto proto = m_doc.m_arrProto[entry.m_nProto];
            CDlgObject *d = new CDlgObject (this);
            d->setGameDB(&m_doc);
            d->load( entry.m_nProto );
            if (d->exec() == QDialog::Accepted) {
                d->save( entry.m_nProto );
                if (proto != currProto) {
                    m_doc.setDirty(true);
                }
                emit spriteUpdated(entry.m_nProto);
            }
            delete d;
        }
    }
}

void MainWindow::on_actionCustomize_triggered()
{
    if (m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        if (layer.selection().getSize()) {
            CDlgEntry *d = new CDlgEntry(this);
            d->setGameDB( & m_doc);
            d->load(layer.getSelectionIndex(0));
            if (layer.isMultiSelection()) {
                d->loadMultiSelection(layer.selection());
            }
            if (d->exec() == QDialog::Accepted) {
                CSelection & selection = layer.selection();
                for (int i=0; i < selection.getSize(); ++i) {
                    int j = layer.getSelectionIndex(i);
                    d->save(j, layer.isMultiSelection());
                    if (selection.cacheAtIndex(i) != layer[j]) {
                        selection.resync(layer[j], i);
                        m_doc.setDirty(true);
                    }
                    emit spriteUpdated(layer[j].m_nProto);
                }
            }
            delete d;
        }
    }
}

void MainWindow::on_actionImages_triggered()
{
    CWizFrameSet *wiz = new CWizFrameSet(static_cast<QWidget*>(parent()));
    wiz->init(m_doc.frames().getSize());
    if (wiz->exec()) {
        CFrameSet *frameSet = new CFrameSet (wiz->getFrameSet());
        std::string name = wiz->getName();
        frameSet->setName(name.c_str());
        m_doc.frames().add(frameSet);
        // add this new imageSet to the cache
        m_doc.cache()->add(frameSet);
        // set the doc to dirty
        m_doc.setDirty( true );
    }
    delete wiz;
}

void MainWindow::showContextMenu(const QPoint& pos)
{
    // TODO: disable in game mode
    if (!(m_viewMode == VM_GAME)) {
        if (m_doc.getSize()) {
            CLevel & level = m_doc.getCurrentLevel();
            CLayer & layer = * level.getCurrentLayer();
            int x = pos.x() & 0xfff8;
            int y = pos.y() & 0xfff8;
            CGameFile * gf = &m_doc;
            int index = gf->whoIs( level, x + level.m_mx, y + level.m_my, O_INT(GENERAL, SKILL_FILTER));
            if (index == -1 || !layer.isInSelection(index)) {
                layer.selectSingle( index );
            }
            updateMenus();
            if (layer.getSelectionSize()) {
                // if we got a selection display the sprite menu
                QMenu menu(m_lview);
                menu.addAction(ui->actionEdit_Object);
                menu.addAction(ui->actionCustomize);
                menu.addAction(ui->actionView_Code);
                if (layer.getSelectionSize()==1) {
                    menu.addAction(ui->actionEdit_Path);
                    menu.addAction(ui->actionEdit_Images);
                }
                menu.addSeparator();
                menu.addAction(ui->actionCopy);
                menu.addAction(ui->actionSend_to_back);
                menu.addAction(ui->actionBring_to_front);
                menu.addSeparator();
                menu.addAction(ui->actionCut);
                menu.addAction(ui->actionDelete);
                QMenu* submenuA = menu.addMenu( tr("All instances") );
                submenuA->addAction(ui->actionRemove_All);
                submenuA->addAction(ui->actionMark_All_as_Goals);
                submenuA->addAction(ui->actionUnmark_All_as_Goals);
                menu.exec(m_lview->mapToGlobal(pos));
            } else {
                // level menu items
                QMenu menu(m_lview);
                menu.addAction(ui->actionCreate_Level);
                menu.addAction(ui->actionEdit_Level);
                menu.addAction(ui->actionTest_Level);
                menu.addAction(ui->actionMove_Level);
                menu.addAction(ui->actionDelete_Level);
                menu.addSeparator();
                menu.addAction(ui->actionPaste);
                menu.addSeparator();
                menu.addMenu(ui->menu_Go);
                menu.addMenu(ui->menuObjects);
                menu.addMenu(ui->menuLayers);
                menu.addSeparator();
                menu.addAction(ui->actionEdit_Game_Info);
                menu.exec(m_lview->mapToGlobal(pos));
            }
        } else {
            QMenu menu(m_lview);
            menu.addAction(ui->actionCreate_Level);
            menu.exec(m_lview->mapToGlobal(pos));
        }
    }
}

void MainWindow::reloadLayerCombo()
{
    bool enable = false;
    m_comboLayers->clear();
    if (m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        int currLayer = level.getCurrentLayerById();
        for (int i=0; i < level.getSize(); ++i) {
            CLayer & layer = level[i];
            m_comboLayers->addItem(layer.getName());
        }
        enable = level.getSize() > 1;
        m_comboLayers->setCurrentIndex(currLayer);
    }
    m_comboLayers->setEnabled(enable && m_viewMode != VM_EDIT_PATH);
}

void MainWindow::reloadEventCombo()
{
    if (!m_comboEvents->count()) {
        m_comboEvents->addItem(tr("(events)"));
        for (int i=0; i < CProtoArray::getEventCount(); ++i) {
            m_comboEvents->addItem(CProtoArray::getEventName(i));
        }
    }
    bool set = false;
    if (m_doc.getSize() && m_viewMode == VM_EDITOR) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        if (layer.isSingleSelection()) {
            CLevelEntry & entry = layer.getSelection(0);
            m_proto = entry.m_nProto;
            m_event = -1;
        }
    }

    if (m_proto != -1) {
        set = true;
        CObject & object = m_doc.m_arrProto.getObject(m_proto);
        QIcon iconBlank;
        iconBlank.addFile(":/images/blank.png");
        QIcon iconCheck;
        iconCheck.addFile(":/images/check.png");
        for (int i=0; i < CProtoArray::getEventCount(); ++i) {
            if (!object.getEvent(i)[0]) {
                m_comboEvents->setItemIcon(i + 1, iconBlank);
            } else {
                m_comboEvents->setItemIcon(i + 1, iconCheck);
            }
        }
    }

    if (!set) {
        QIcon icon;
        icon.addFile(":/images/blank.png");
        for (int i=1; i < CProtoArray::getEventCount() + 1; ++i) {
            m_comboEvents->setItemIcon(i, icon);
        }
    }
}

void MainWindow::selectLevel(int index)
{
    m_doc.m_nCurrLevel = index;
    updateMenus();
    showLayerName();
    reloadLayerCombo();
}

void MainWindow::editLevel(int index)
{
    m_doc.m_nCurrLevel = index;
    on_actionEdit_Level_triggered();
}

void MainWindow::deleteLevel(int index)
{
    m_doc.m_nCurrLevel = index;
    on_actionDelete_Level_triggered();
}

void MainWindow::addLevel()
{
    on_actionCreate_Level_triggered();
}

void MainWindow::notifyKeyEvent(int keyCode, int state)
{
    m_doc.setKey(keyCode, state);
    if (m_viewMode == VM_GAME) {
        m_doc.setLastKey(keyCode);
        if (state) {
            m_doc.callLvEvent(CLevel::EL_KEY_PRESSED);
        } else {
            m_doc.callLvEvent(CLevel::EL_KEY_UP);
        }
    }
}

void MainWindow::commitAll()
{
    if (m_viewMode == VM_SPRITE_EVENTS) {
        emit eventsCommited();
    }
}

void MainWindow::setViewMode(int viewMode)
{
    if (viewMode != m_viewMode) {
        // pre-switch
        switch (m_viewMode) {
        case VM_SPRITE_EVENTS:
            emit eventsCommited();
            m_editEvents->hide();
            break;

        case VM_GAME:
            emit gameModeEnabled(false);
            if (viewMode != VM_EDITOR) {
                m_scroll->hide();
            }
            break;

        case VM_EDITOR:
            if (viewMode == VM_SPRITE_EVENTS) {
                m_scroll->hide();
            }
            break;
        }

        // actual switch
        m_viewMode = viewMode;
        switch (viewMode) {
        case VM_EDITOR:
            m_scroll->show();
            reloadEventCombo();
            break;

        case VM_GAME:
            m_scroll->show();
            emit gameModeEnabled(true);
            break;

        case VM_SPRITE_EVENTS:
            m_editEvents->show();
            emit eventStatusShown();
            break;
        }
        updateMenus();
    }
}

void MainWindow::showEventStatus()
{
    setStatus(0, "");
    setStatus(1, "");
    if (m_proto != -1) {
        CProto & proto = m_doc.m_arrProto[m_proto];
        QString s = QString(tr("Sprite %1")).arg(proto.m_szName);
        showStatus(s);
    } else {
        showStatus("");
    }
}

void MainWindow::deleteSprite(int sprite)
{
    updateMenus();
    emit spriteDeleted(sprite);
    changeProtoIcon(CGame::INVALID);
}

void MainWindow::changeSprite(int sprite)
{
    qDebug("change Sprite to %d", sprite);
    m_proto = sprite;
    m_event = 0;
    emit protoChanged(sprite, 0);
    qDebug("set viewMode to VM_SPRITE_EVENTS");
    setViewMode(VM_SPRITE_EVENTS);
    reloadEventCombo();
    m_comboEvents->setEnabled( true );
    m_comboEvents->setCurrentIndex(1);
    emit eventStatusShown();
}

void MainWindow::changeViewMode()
{
    setViewMode(VM_EDITOR);
}

void MainWindow::on_actionView_Code_triggered()
{
    if (m_doc.getSize() && (m_viewMode == VM_EDITOR)) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        if (layer.isSingleSelection()) {
            if (m_viewMode != VM_SPRITE_EVENTS) {
                setViewMode(VM_SPRITE_EVENTS);
            }
            CLevelEntry & entry = layer.getSelection(0);
            m_proto = entry.m_nProto;
            m_event = 0;
            m_comboEvents->setCurrentIndex(1);
            emit protoChanged(entry.m_nProto, 0);
            emit eventStatusShown();
        }
    }
}

void MainWindow::on_actionCreateSprite_triggered()
{
     emit spriteCreated();
}

void MainWindow::on_actionDocumentation_triggered()
{
    QString url = QString("%1manual.htm?v=%2").arg(WEB_PATH, formatVersion(true));
    QDesktopServices::openUrl(url);
}

void MainWindow::on_actionTutorials_triggered()
{
    QString url = QString("%1tutorial.htm?v=%2").arg(WEB_PATH, formatVersion(true));
    QDesktopServices::openUrl(url);
}

void MainWindow::on_actionUndo_triggered()
{
    warningMessage("Not implemented yet!");
}

void MainWindow::on_actionRedo_triggered()
{
    warningMessage("Not implemented yet!");
}

void MainWindow::on_actionCut_triggered()
{
    if (m_doc.getSize()
            && m_viewMode == VM_EDITOR) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer *layer = level.getCurrentLayer();
        if (layer->isMultiSelection()
                || layer->isSingleSelection()) {
            // copy sprites to clippy
            CSelection *clipboard = m_doc.getClipboard();
            clipboard->clear();
            for (int i=0; i < layer->getSelectionSize(); ++i) {
                CLevelEntry & entry = layer->getSelection(i);
                int j = layer->getSelectionIndex(i);
                clipboard->addEntry(entry, j);
            }
            clipboard->applyDelta(level.m_mx, level.m_my);
            layer->removeSelectedSprites();
            m_doc.setDirty(true);
            m_lview->repaint();
            updateMenus();
        }
    }
}

void MainWindow::on_actionCopy_triggered()
{
    if (m_doc.getSize()
            && m_viewMode == VM_EDITOR) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer *layer = level.getCurrentLayer();
        if (layer->isMultiSelection()
                || layer->isSingleSelection()) {
            // copy sprites tp clippy
            CSelection *clipboard = m_doc.getClipboard();
            clipboard->clear();
            for (int i=0; i < layer->getSelectionSize(); ++i) {
                CLevelEntry & entry = layer->getSelection(i);
                int j = layer->getSelectionIndex(i);
                clipboard->addEntry(entry,j);
            }
            clipboard->applyDelta(level.m_mx, level.m_my);
            updateMenus();
        }
    }
}

void MainWindow::on_actionPaste_triggered()
{
    if (m_doc.getSize() && !(m_viewMode == VM_GAME)) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        CSelection *clipboard = m_doc.getClipboard();
        int selCount = clipboard->getSize();
        if (selCount) {
            layer.clearSelection();
            m_doc.setDirty(true);
            for (int i=0; i < selCount; ++i) {
                CLevelEntry entry = (*clipboard)[i];
                // duplicate the string entry (if applicable)
                CStringTable * table = m_doc.getStringTable();
                entry.m_string = table->duplicate(entry.m_string);
                // duplicate path (if applicable)
                CPathBlock *paths = m_doc.getPaths();
                entry.m_path = paths->duplicate(entry.m_path);
                entry.moveBy(CLevelViewGL::OBJ_SCROLL, CLevelViewGL::OBJ_SCROLL);
                layer.select( layer.add(entry) );
            }
        }
    }
}

void MainWindow::on_actionDelete_triggered()
{
    if (m_doc.getSize()
            && m_viewMode == VM_EDITOR) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer *layer = level.getCurrentLayer();
        if (layer->isMultiSelection()
                || layer->isSingleSelection()) {
            int selCount = layer->getSelectionSize();
            for (int i=0; i < selCount; ++i) {
                CLevelEntry entry = layer->getSelection(i);
                // revoke string entry (if applicable)
                CStringTable * table = m_doc.getStringTable();
                table->set(entry.m_string, "");
            }
            layer->removeSelectedSprites();
            m_doc.setDirty(true);
            m_lview->repaint();
            updateMenus();
        }
    }
}

void MainWindow::on_actionCopy_Object_triggered()
{
    if (m_doc.getSize() && !(m_viewMode == VM_GAME)) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        int selCount = layer.getSelectionSize();
        if (selCount) {
            m_doc.setDirty(true);
            for (int i=0; i < selCount; ++i) {
                CLevelEntry entry = layer.getSelection(0);
                // duplicate the string entry (if applicable)
                CStringTable * table = m_doc.getStringTable();
                entry.m_string = table->duplicate(entry.m_string);
                // duplicate path (if applicable)
                CPathBlock *paths = m_doc.getPaths();
                entry.m_path = paths->duplicate(entry.m_path);
                entry.moveBy(CLevelViewGL::OBJ_SCROLL, CLevelViewGL::OBJ_SCROLL);
                layer.select( layer.add(entry) );
                layer.unSelectedAt(0);
            }
        }
    }
}

void MainWindow::on_actionSearch_triggered()
{
    warningMessage("Not implemented yet!");
}

CGame* MainWindow::getGame()
{
    return & m_doc;
}

void MainWindow::initToolBox()
{
    m_toolBox->init();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

QStringList & MainWindow::defaultShortcuts()
{
    static QStringList listShortcuts;
    if (!listShortcuts.count()) {
        QAction **actions = actionShortcuts();
        for (uint i=0; actions[i];++i) {
            QAction *a = actions[i];
            QKeySequence s = a->shortcut();
            listShortcuts.append(s.toString());
        }
    }
    return listShortcuts;
}

QAction** MainWindow::actionShortcuts()
{
    static QAction *actions[]={
        ui->actionRestart,
        ui->actionDebug,
        ui->actionQuit_Game,
        ui->actionPause,
        ui->actionRemove_All,
        ui->actionRuntime_Lua,
        ui->actionEdit_layer,
        ui->actionNext_layer,
        ui->actionPrevious_layer,
        ui->actionDelete_layer,
        ui->actionCreate_layer,
        ui->actionMove_Layer,
        ui->actionGo_to_layer,
        ui->actionC_declarations,
        ui->actionConfigure_LGCK_Builder,
        ui->action_ShowGrid,
        ui->actionTest_Level,
        ui->actionAbout,
        ui->actionNew_file,
        ui->actionBring_to_front,
        ui->actionSend_to_back,
        ui->actionDelete_Object,
        ui->actionMove_Level,
        ui->actionGo_to_level,
        ui->actionDelete_Level,
        ui->actionCreate_Level,
        ui->actionEdit_Game_Info,
        ui->actionEdit_Level,
        ui->actionLast_level,
        ui->actionFirst_level,
        ui->actionRevert,
        ui->actionSave_as,
        ui->actionSave,
        ui->actionNext,
        ui->actionPrevious,
        ui->actionOpen,
        ui->actionExport_Sprite,
        ui->actionView_Source,
        ui->actionEdit_Object,
        ui->actionCustomize,
        ui->actionImages,
        ui->action_ShowToolbox,
        ui->actionCopy_Object,
        ui->actionView_Code,
        ui->actionCreateSprite,
        ui->actionDocumentation,
        ui->actionTutorials,
        ui->actionUndo,
        ui->actionRedo,
        ui->actionCut,
        ui->actionCopy,
        ui->actionPaste,
        ui->actionDelete,
        ui->actionSearch,
        ui->actionAbout_Qt,
        ui->actionEdit_Path,
        ui->actionSave_Settings,
        ui->actionMainToolbar,
        ui->actionLevelToolbar,
        ui->actionStatus_bar,
        ui->actionIncrease_Font_Size,
        ui->actionDecrease_Font_Size,
        ui->actionReset_Font_Size,
        ui->actionScriptWizard,
        ui->actionLayer_ToolBar,
        ui->actionEdit_Images,
        ui->actionDebugOutput,
        ui->actionSprite_Paint,
        m_actionEraser,
        nullptr
    };
    return actions;
}

void MainWindow::saveSettings()
{
    setViewMode(VM_EDITOR);
    QSettings settings;
    settings.setValue("saveSettings", ui->actionSave_Settings->isChecked());
    bool saveSettings = ui->actionSave_Settings->isChecked();
    if (saveSettings) {
        // shortcuts
        settings.beginGroup("Shortcuts");
        QAction **actions = actionShortcuts();
        for (uint i=0; actions[i];++i) {
            QAction *a = actions[i];
            QKeySequence s = a->shortcut();
            settings.setValue(a->text(), s.toString());
        }
        settings.endGroup();
        settings.beginGroup("Toolbars");
        settings.setValue("mainToolbar", ui->actionMainToolbar->isChecked());
        settings.setValue("levelToolbar", ui->actionLevelToolbar->isChecked());
        settings.setValue("layerToolbar", ui->actionLayer_ToolBar->isChecked());
        settings.setValue("statusbar", ui->actionStatus_bar->isChecked());
        settings.setValue("debugOutput", ui->actionDebugOutput->isChecked());
        settings.setValue("toolBox", m_bShowToolBox);
        settings.endGroup();

        settings.beginGroup("Tools");
        settings.setValue("eraserTool", m_actionEraser->isChecked());
        settings.setValue("paintTool", ui->actionSprite_Paint->isChecked());
        settings.endGroup();

        settings.beginGroup("UI_Components");
        settings.setValue("mainWindow:geometry", this->saveGeometry());
        settings.setValue("mainWindow:state", this->saveState());
        settings.setValue("version", UI_VERSION);
        settings.endGroup();

        settings.beginGroup("Updater");
        settings.setValue("updater_check", m_bUpdate);
        settings.setValue("updater_url", m_updateURL);
        settings.setValue("uuid", m_uuid);
        QString ver = formatVersion(false);
        settings.setValue("version", ver);
        settings.endGroup();

        // Editor
        m_options->write(settings);

        // Runtime
        settings.beginGroup("Runtime");
        settings.setValue("path", m_runtime);
        settings.setValue("args", m_runtimeArgs);
        settings.setValue("external", m_runtimeExternal);
        settings.setValue("rez", m_rez);
        settings.endGroup();

        // inputs
        writeButtonConfig(settings);
        settings.sync();
    }
}

void MainWindow::initSettings()
{
    // add default values
    qDebug("initSettings");
    m_options = new COptions;
    COptionGroup & editor = (*m_options)[EDITOR];
    editor.set(FONT_SIZE, DEFAULT_FONT_SIZE)
            .set(FONT_NAME, DEFAULT_FONT_NAME)
            .set(SKIP_SPLASH, false)
            .set(ENABLE_AUTO_COMPLETE, true)
            .set(ENABLE_WORDWRAP, true)
            .set(ENABLE_HIGHLIGHT, true)
            .set(ENABLE_WHITESPACE, true);

    COptionGroup & general = (*m_options)[GENERAL];
    general.set(GRIDCOLOR, "a0b0c0")
            .set(SHOWGRID, true)
            .set(TRIGGER_KEY_COLOR, "FFFF00")
            .set(TRIGGER_KEY_FONT_SIZE, 24)
            .set(TRIGGER_KEY_SHOW, true)
            .set(GRIDSIZE, 32)
            .set(LAST_PROJECTS, 4)
            .set(XTICK_MAX_RATE, 100)
            .set(LAST_FOLDER, true)
            .set(SKILL_FILTER, CGame::SKILL_FLAG_ALL);

    COptionGroup & testLevel = (*m_options)[TESTLEVEL];
    testLevel.set(SKILL, 0)
            .set(LIVES, 5)
            .set(SCORE, 0)
            .set(START_HP, 32)
            .set(CONTINUE, true);

    COptionGroup & folders = (*m_options)[FOLDERS];
    folders.set(FOLDER_LGCKDB, "");

    QSettings settings(m_author, m_appName);
    qDebug("read settings");
    m_options->read(settings);
}

void MainWindow::reloadSettings()
{
    qDebug("reloadSettings");
    QSettings settings;
    qDebug() << settings.fileName();

    // grid
    emit gridVisible(O_BOOL(GENERAL, SHOWGRID));
    ui->action_ShowGrid->setChecked(O_BOOL(GENERAL, SHOWGRID));
    emit gridColorChanged(O_STR(GENERAL, GRIDCOLOR));
    emit gridSizeChanged(O_INT(GENERAL, GRIDSIZE) > 0 ? O_INT(GENERAL, GRIDSIZE) : 32);
    emit triggerKeyColorChanged(O_STR(GENERAL, TRIGGER_KEY_COLOR));
    emit triggerKeyShow(O_BOOL(GENERAL, TRIGGER_KEY_SHOW));
    emit triggerKeyFontSizeChanged(O_INT(GENERAL, TRIGGER_KEY_FONT_SIZE));
    bool saveSettings = settings.value("saveSettings", true).toBool();
    ui->actionSave_Settings->setChecked(saveSettings);

    // save default shortcuts
    defaultShortcuts();
    // restore settings from config
    settings.beginGroup("Shortcuts");
    QAction **actions = actionShortcuts();
    for (uint i=0; actions[i];++i) {
        QAction *a = actions[i];
        QString s = settings.value(a->text(),"n/a").toString();
        if (s != "n/a") {
            a->setShortcut(QKeySequence(s));
        }
    }
    settings.endGroup();
    // Toolbars
    settings.beginGroup("Toolbars");
    bool mainToolbar = settings.value("mainToolbar", true).toBool();
    ui->actionMainToolbar->setChecked(mainToolbar);
    bool levelToolbar = settings.value("levelToolbar", true).toBool();
    ui->actionLevelToolbar->setChecked(levelToolbar);
    bool layerToolbar = settings.value("layerToolbar", true).toBool();
    ui->actionLayer_ToolBar->setChecked(layerToolbar);
    bool statusBar = settings.value("statusbar", true).toBool();
    ui->actionStatus_bar->setChecked(statusBar);
    mainToolbar ? ui->toolBar->show() : ui->toolBar->hide();
    levelToolbar ? m_levelToolbar->show() : m_levelToolbar->hide();
    layerToolbar ? m_layerToolbar->show() : m_layerToolbar->hide();
    statusBar ? ui->statusBar->show() : ui->statusBar->hide();
    bool debugOutput = settings.value("debugOutput", false).toBool();
    debugOutput ? m_infoDock->show() : m_infoDock->hide();
    // toolbox
    m_bShowToolBox = settings.value("toolBox", true).toBool();
    showToolBox(m_bShowToolBox);
    settings.endGroup();

    // Updater
    settings.beginGroup("Updater");
    m_bUpdate = settings.value("updater_check", true).toBool();
    m_updateURL = settings.value("updater_url", UPDATER_URL).toString();
    QString savedVersion = settings.value("version", "").toString();
    char *uuid = getUUID();
    m_uuid = settings.value("uuid", const_cast<const char*>(uuid)).toString();
    delete []uuid;
    QString currVersion = formatVersion(false);
    if (currVersion != savedVersion) {
        m_updateURL = UPDATER_URL;
    }
    settings.endGroup();

    // Runtime
    settings.beginGroup("Runtime");
    m_runtime = settings.value("path", "").toString();
    m_runtimeArgs = settings.value("args", CDlgAppSettings::defaultRuntimeArgs()).toString();
    m_runtimeExternal = settings.value("external", false).toBool();
    m_rez = settings.value("rez", 0).toInt();
    settings.endGroup();

    // Editor
    emit fontChanged(currentFont());
    emit editorOptionChanged((*m_options)[EDITOR]);

    // Tools
    settings.beginGroup("Tools");
    m_actionEraser->setChecked(settings.value("eraserTool", false).toBool());
    ui->actionSprite_Paint->setChecked(settings.value("paintTool", false).toBool());
    settings.endGroup();

    updateSkillFiltersCheckbox(O_INT(GENERAL, SKILL_FILTER));
    // Inputs
    readButtonConfig(settings);
}

void MainWindow::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    if (visible) {
        QSettings settings;
        settings.beginGroup("UI_Components");
        int version = settings.value("version", 0).toInt();
        if (version == UI_VERSION) {
            restoreGeometry(settings.value("mainWindow:geometry").toByteArray());
            restoreState(settings.value("mainWindow:state").toByteArray());
            QRect rect = this->geometry();
            rect.setTop(rect.top() + 22);
            rect.setBottom(rect.bottom() + 22);
            rect.setLeft(rect.left() + 1);
            rect.setRight(rect.right() + 1);
            this->setGeometry(rect);
        }
        settings.endGroup();
    }
}

void MainWindow::on_actionEdit_Path_triggered()
{
    if (m_doc.getSize() && m_viewMode == VM_EDITOR) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        CPathBlock & paths = *(m_doc.getPaths());
        int selCount = layer.getSelectionSize();
        if (selCount == 1) {
            CLevelEntry & entry = layer.getSelection(0);
            if (entry.m_path) {
                paths[entry.m_path].forget();
            } else {
                entry.m_path = paths.addPath();
            }            
            m_pathOrgX = entry.m_nX;
            m_pathOrgY = entry.m_nY;
            m_pathEntry = layer.getSelectionIndex(0);
            setViewMode(VM_EDIT_PATH);
            emit pathStarted();
        }
    }
}

void MainWindow::on_actionMainToolbar_toggled(bool arg1)
{
    ui->actionMainToolbar->setChecked(arg1);
    arg1 ? ui->toolBar->show() : ui->toolBar->hide();
}

void MainWindow::on_actionLevelToolbar_toggled(bool arg1)
{
    ui->actionLevelToolbar->setChecked(arg1);
    arg1 ? m_levelToolbar->show() : m_levelToolbar->hide();
}

void MainWindow::on_actionStatus_bar_toggled(bool arg1)
{
    ui->actionStatus_bar->setChecked(arg1);    
    arg1 ? ui->statusBar->show() : ui->statusBar->hide();
}

void MainWindow::on_action_ShowGrid_toggled(bool arg1)
{
    O_SET(GENERAL, SHOWGRID, arg1);
    emit gridVisible(O_BOOL(GENERAL, SHOWGRID));
    m_lview->repaint();
}

void MainWindow::on_action_ShowToolbox_toggled(bool arg1)
{
    showToolBox(arg1);
}

void MainWindow::layerChanged(int layerId)
{
    if (m_doc.m_nCurrLevel != -1
        && m_doc.getSize()
        && layerId != -1) {
        CLevel & level = m_doc.getCurrentLevel();
        level.setCurrentLayerById(layerId);
        showLayerName();
    }
    updateMenus();
}

void MainWindow::on_actionLayer_ToolBar_toggled(bool arg1)
{
    ui->actionLayer_ToolBar->setChecked(arg1);
    arg1 ? m_layerToolbar->show() : m_layerToolbar->hide();
}

void MainWindow::closePath()
{
    if (m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        CLevelEntry & entry = layer[m_pathEntry];
        CPathBlock & paths = *(m_doc.getPaths());
        if (!paths[entry.m_path].getSize()){
            entry.m_path = 0;
        }
        entry.m_nX = m_pathOrgX;
        entry.m_nY = m_pathOrgY;
        showLayerName();
    }
    setViewMode(VM_EDITOR);
    emit pathEnded();
}

void MainWindow::on_actionScriptWizard_triggered()
{
    CWizScript *wiz = new CWizScript(this);
    wiz->init(&m_doc);
    if (wiz->exec()) {
        emit textInserted(wiz->getScript().c_str());
    }
    delete wiz;
}

void MainWindow::updateEditor(const QString &url, const QString &ver)
{
    QMessageBox::StandardButton ret = QMessageBox::information(this, tr(m_appName),
                 tr("LGCK builder version %1 is out.\nDo you want to download it now?").arg(ver),
                 QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        QDesktopServices ds;
        ds.openUrl(url);
    }
}

void MainWindow::checkVersion()
{
    QString productVersion = QSysInfo::productVersion();
    QString productType = QSysInfo::productType();
    QString ver = formatVersion(false);
    QString driver = QGuiApplication::platformName();
    QString url = QString::asprintf(q2c(m_updateURL),
                                    q2c(ver),
                                    q2c(driver),
                                    q2c(QSysInfo::kernelType()),
                                    q2c(m_uuid),
                                    q2c(QString(productType + "+" + productVersion)));
    while (m_updater->isRunning());
    QString vendor;
    QString renderer;
    QString glversion;
    QString extensions;
    m_scroll->getGLInfo(vendor, renderer, glversion, extensions);

    QByteArray data;
    data.append("vendor=").append(QUrl::toPercentEncoding(vendor).constData()).append("&");
    data.append("renderer=").append(QUrl::toPercentEncoding(renderer).constData()).append("&");
    data.append("version=").append(QUrl::toPercentEncoding(glversion).constData()).append("&");
    data.append("extensions=").append(QUrl::toPercentEncoding(extensions).constData()).append("&");
    qDebug("URL: %s", q2c(url));
    m_updater->setUrl(url);
    m_updater->setData(data);
    m_updater->start();
}

void MainWindow::makeCurrent()
{
    emit focusGL();
}

void MainWindow::on_actionIncrease_Font_Size_triggered()
{
    int fontSize = O_INT(EDITOR, FONT_SIZE);
    ++ fontSize;
    O_SET(EDITOR, FONT_SIZE, fontSize > MAX_FONT_SIZE ? MAX_FONT_SIZE : fontSize);
    emit fontChanged(currentFont());
}

void MainWindow::on_actionDecrease_Font_Size_triggered()
{
    int fontSize = O_INT(EDITOR, FONT_SIZE);
    -- fontSize;
    O_SET(EDITOR, FONT_SIZE, fontSize < MIN_FONT_SIZE ? MIN_FONT_SIZE : fontSize);
    emit fontChanged(currentFont());
}

void MainWindow::on_actionReset_Font_Size_triggered()
{
    O_SET(EDITOR, FONT_SIZE, DEFAULT_FONT_SIZE);
    emit fontChanged(currentFont());
}

void MainWindow::on_actionEdit_Images_triggered()
{
    if (m_doc.getSize()) {
        CLevel & level = m_doc.getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        if (layer.isSingleSelection()) {
            CLevelEntry entry = layer.getSelection(0);
            const CProto proto = m_doc.m_arrProto[entry.m_nProto];
            int fs = proto.m_nFrameSet;
            CDlgFrameSet * d = new CDlgFrameSet (this);
            d->setWindowTitle ( QString(tr("Edit Image Set `%1`")).arg(m_doc.frames()[fs]->getName()) );
            CFrameSet * frameSet = new CFrameSet (m_doc.frames()[fs]);
            d->init(frameSet);
            if (d->exec() == QDialog::Accepted) {
                m_doc.setDirty( true );
                // replace frameSet
                delete m_doc.frames()[ fs ];
                m_doc.frames().setAt( fs, frameSet);
                // update the imageCache
                m_doc.cache()->replace(fs, frameSet);
                //updateFrameSet
                emit frameSetChanged(fs);
            } else {
                delete frameSet;
            }
            delete d;
        }
    }
}

bool MainWindow::checkExecutible(const QString exec, QString & errMsg)
{
    if (exec.isEmpty()) {
        errMsg = tr("The file path is empty.");
        return false;
    }
    QFileInfo fi(exec);
    if (!fi.exists()){
        errMsg = tr("The executable doesn't exists");
        return false;
    }
    if (!fi.isFile()) {
        errMsg = tr("This is not a file");
        return false;
    }
    if (!fi.isExecutable()) {
        errMsg = tr("The file is not executable");
        return false;
    }
    return true;
}

void MainWindow::goExternalRuntime()
{
    QString filename = m_doc.getFileName();
    QStringList list;
    QStringList tmp = m_runtimeArgs.split(" ", Qt::SkipEmptyParts);
    QListIterator<QString> itr(tmp);
    while (itr.hasNext()) {
        QString current = itr.next().trimmed();
        if (!current.isEmpty()) {
            current = current.replace("%1", filename)
                    .replace("%2", QString::number(m_doc.m_nCurrLevel))
                    .replace("%3", QString::number(O_INT(TESTLEVEL, SKILL)))
                    .replace("%4", QString::number(m_rezW))
                    .replace("%5", QString::number(m_rezH));
            list.append(current);
        }
    }
    qDebug() << list;
    QDir d = QFileInfo(m_runtime).absoluteDir();
    QString errMsg = "";
    if (checkExecutible(m_runtime, errMsg)) {
        bool result = QProcess::startDetached(m_runtime, list, d.absolutePath());
        if (!result) {
            errMsg = tr("Running external runtime failed");
        }
    }
    if (!errMsg.isEmpty()){
        warningMessage(errMsg);
        showAppSettings(CDlgAppSettings::TAB_TESTLEVEL);
    }
}

void MainWindow::on_actionSprite_Editor_triggered()
{
    Path outPath;
    if (!getCmd(SPRITE_EDITOR, outPath)) {
        QMessageBox::warning(this, m_appName, tr("Couldn't find executable: %1").arg(SPRITE_EDITOR));
        return;
    }
    QString runtime = "\"" + outPath.path + "/" + outPath.cmd + "\"";
    bool result = launchProcess(outPath);
    if (!result) {
        QMessageBox::warning(this, m_appName, tr("Running external editor failed: %1").arg(runtime));
    }
}

void MainWindow::exportGame()
{
    QMessageBox::warning(this, m_appTitle, tr("Export game is unavailable at this time."));
    return;

    if (m_doc.isDirty() || m_doc.m_path.empty()) {
        if (!saveAs()) {
            return;
        }
    }

    CDlgDistributeGame dlg(this);
    if (dlg.exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QString outMsg;
        CExportGame ex;
        if (!ex.exportCore(dlg, m_doc, outMsg)) {
            QApplication::restoreOverrideCursor();
            warningMessage(outMsg);
        }
    }
}

void MainWindow::on_actionExport_Game_triggered()
{
    exportGame();
}

void MainWindow::on_actionDistribution_Package_triggered()
{
    exportGame();
}

void MainWindow::updateFrameSet(const QString & fileName)
{
    qDebug() << QString("frameset updated: %1").arg(fileName);
}

void MainWindow::showEvent(QShowEvent* pEvent)
{
    QMainWindow::showEvent(pEvent);
    if (m_state.length()) {
        this->restoreState(m_state);
        m_state.clear();
    }
}

void MainWindow::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::WindowStateChange) {
        //   WindowStateChange = 105
        QWindowStateChangeEvent* ev = static_cast<QWindowStateChangeEvent*>(e);
        if (!(ev->oldState() & Qt::WindowMaximized) && windowState() & Qt::WindowMaximized)
        {
            qDebug("Window has been maximized");
        } else if (!(ev->oldState() & Qt::WindowActive) && windowState() & Qt::WindowActive)
        {
            qDebug("Window has been WindowActive");
        } else if  (!(ev->oldState() & Qt::WindowMinimized) && windowState() & Qt::WindowMinimized)
        {
            qDebug("Window has been WindowMinimized");
        }
    } else if (e->type() == QEvent::ActivationChange) {
        // ActivationChange = 99,                  // window activation has changed
        m_state = this->saveState();
    }
    QMainWindow::changeEvent(e);
}

 void MainWindow::markAsGoal(bool isGoal)
 {
     if (m_doc.getSize()
             && m_viewMode == VM_EDITOR) {
         CLevel & level = m_doc.getCurrentLevel();
         CLayer & layer = *(level.getCurrentLayer());
         if (layer.getSelectionSize()) {
             QString msg;
             if (layer.isSingleSelection()) {
                 if (isGoal) {
                     msg = tr("Do you want to mark all instances of this sprite on\n" \
                        "the current layer as goals?");
                 } else {
                     msg = tr("Do you want to unmark all instances of this sprite on\n" \
                        "the current layer as goals?");
                 }
             } else {
                 if (isGoal) {
                     msg = tr("Do you want to mark all instances of the selected sprites on\n" \
                        "the current layer as goals?");
                 } else {
                     msg = tr("Do you want to unmark all instances of the selected sprites on\n" \
                        "the current layer as goals?");
                 }
             }
             QMessageBox::StandardButton ret =  QMessageBox::warning(this, tr(m_appName), msg  ,
                      QMessageBox::Yes | QMessageBox::No);
             if (ret == QMessageBox::Yes) {
                 for (int k=0; k < layer.getSelectionSize(); ++k) {
                     CLevelEntry obj = layer.getSelection(k);
                     for (int i=0; i < layer.getSize(); ++i) {
                         CLevelEntry & entry = layer[i];
                         if (layer[i].m_nProto == obj.m_nProto) {
                             if (isGoal) {
                                 entry.markAsGoal();
                             } else {
                                 entry.unMarkAsGoal();
                             }
                         }
                     }
                 }
                 layer.resyncSelection();
                 m_doc.setDirty(true);
             }
             m_lview->repaint();
             updateMenus();
         }
     }
 }
void MainWindow::on_actionMark_All_as_Goals_triggered()
{
   markAsGoal( true );
}

void MainWindow::on_actionUnmark_All_as_Goals_triggered()
{
    markAsGoal( false );
}

void MainWindow::newDebugString(const char *s)
{
    emit me->debugText(QString(s));
}

void MainWindow::newErrorString(const char *s)
{
    emit me->errorText(QString(s));
}

void MainWindow::on_actionDebugOutput_toggled(bool arg1)
{
    ui->actionDebugOutput->setChecked(arg1);
    arg1 ? m_infoDock->show() : m_infoDock->hide();
}

void MainWindow::on_actionSprite_Paint_toggled(bool checked)
{
    ui->actionSprite_Paint->setChecked(checked);
    emit spritePaintStateChanged(checked);
}

void MainWindow::eraserToggled(bool checked)
{
    qDebug("eraserToggled()");
    m_actionEraser->setChecked(checked);
    emit eraserStateChanged(checked);
}

void MainWindow::changeProtoIcon(int protoId)
{
    CProto proto = m_doc.toProto(protoId > 0 ? protoId : 0);
    m_protoIcon->setToolTip(protoId > 0 ? proto.getName() : "");
    m_protoIcon->setIcon(frame2icon(m_doc.toFrame(proto)));
    QMenu *menu = new QMenu();
    for (int i=0; protoId >0 && i < m_doc.toFrameSet(proto.m_nFrameSet).getSize(); ++i) {
        QAction *action = new QAction(frame2icon(m_doc.toFrame(proto.m_nFrameSet, i)), QString("%1").arg(i + 1));
        uint w = (protoId << 16) + i;
        action->setData(w);
        menu->addAction(action);
        connect(action, &QAction::triggered, this, &MainWindow::toggleProtoFrame);
    }
    m_protoIcon->setMenu(menu);
}

void MainWindow::toggleProtoFrame()
{
    // https://stackoverflow.com/questions/28646914/qaction-with-custom-parameter

    QAction *act = qobject_cast<QAction *>(sender());
    QVariant v = act->data();
    uint32_t w = v.toUInt();
    qDebug("w: 0x%x", w);

    int protoId = w >> 16;
    int frameId = w & 0xffff;
    CProto proto = m_doc.toProto(protoId > 0 ? protoId : 0);
    m_protoIcon->setIcon(frame2icon(m_doc.toFrame(proto.m_nFrameSet, frameId)));
    emit currentFrameChanged(protoId, frameId);
}

void MainWindow::updateIndicator()
{   static int cycle = 0;
    if (m_ready) {
        if (cycle % 4 == 0) {
            m_fixer->troubleshoot();
        }
        m_btnIndicator->setIcon(QIcon(m_fixer->getIcon()));
        m_btnIndicator->setToolTip(m_fixer->getTooltip());
        m_btnIndicator->setStatusTip(m_fixer->getStatus());
    }
    ++cycle;
}

void MainWindow::setReady(bool ready)
{
    m_ready = ready;
    if (m_ready) {
        m_timerIndicator.start();
    } else {
        m_timerIndicator.stop();
    }
}

void MainWindow::indicatorTriggered()
{
    if (m_fixer->ready()){
        CDlgIndicator dlg;
        dlg.setWindowTitle(tr("Project Assistant"));
        dlg.setText(m_fixer->getText());
        dlg.exec();
    }
}

void MainWindow::changeTickMaxRate()
{
    m_timer.setInterval(1000 / O_INT(GENERAL, XTICK_MAX_RATE));
    m_timer.start();
}

void MainWindow::memorizeFilePath()
{
    QDir d = QFileInfo(m_doc.getFileName()).absoluteDir();
    QString absolutePath = d.absolutePath();
    qDebug() << "absolutePath:" << absolutePath;
    O_SET(FOLDERS, FOLDER_LGCKDB, absolutePath);
}

void MainWindow::notifyJoyEvent(lgck::Button::JoyButton button, char value)
{
    qDebug() << QString("Button %1 %2").arg(m_doc.buttonText(button)).arg(value);
    m_doc.setJoyButton(button, value);
}

#ifdef LGCK_GAMEPAD
void MainWindow::initGamePad()
{
    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    qDebug(!gamepads.isEmpty() ? "gamepad found" : "no gamepad found");
    if (gamepads.isEmpty()) {
        return;
    }
    m_gamepad = new QGamepad(*gamepads.begin(), this);
    qDebug() << "name: " << m_gamepad->name() << m_gamepad->deviceId();

    connect(this, &MainWindow::joyEventOccured, this, &MainWindow::notifyJoyEvent);

    connect(m_gamepad, &QGamepad::buttonAChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::A, pressed);
    });
    connect(m_gamepad, &QGamepad::buttonBChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::B, pressed);
    });
    connect(m_gamepad, &QGamepad::buttonXChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::X, pressed);
    });
    connect(m_gamepad, &QGamepad::buttonYChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::Y, pressed);
    });

    connect(m_gamepad, &QGamepad::buttonL1Changed, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::L1, pressed);
    });
    connect(m_gamepad, &QGamepad::buttonR1Changed, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::R1, pressed);
    });
    connect(m_gamepad, &QGamepad::buttonSelectChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::Select, pressed);
    });
    connect(m_gamepad, &QGamepad::buttonStartChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::Start, pressed);
    });

    connect(m_gamepad, &QGamepad::buttonGuideChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::Guide, pressed);
    });

    connect(m_gamepad, &QGamepad::buttonUpChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::Up, pressed);
    });
    connect(m_gamepad, &QGamepad::buttonDownChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::Down, pressed);
    });
    connect(m_gamepad, &QGamepad::buttonLeftChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::Left, pressed);
    });
    connect(m_gamepad, &QGamepad::buttonRightChanged, this, [](bool pressed){
        emit me->joyEventOccured(lgck::Button::Right, pressed);
    });
}
#endif

void MainWindow::readButtonConfig(QSettings & settings)
{
    QStringList buttonUsed;
    QStringList keyUsed;
    settings.beginGroup(INPUTS);
    for (int i=0; i < lgck::Input::Count; ++i) {
        const char *actionName = actionNames[i];
        CGame::JoyStateEntry & entry = m_doc.joyStateEntry(i);
        QString buttonText = settings.value(QString("button_%1").arg(actionName), "").toString();
        QString qtKeyText = settings.value(QString("key_%1").arg(actionName), "").toString();
        if (buttonText.isEmpty() && !buttonUsed.contains(actionButtons[i])) {
            buttonText = actionButtons[i];
            buttonUsed.append(buttonText);
        }
        if (qtKeyText.isEmpty() && !keyUsed.contains(actionKeys[i])) {
            qtKeyText = actionKeys[i];
            keyUsed.append(qtKeyText);
        }
        entry.keyCode = static_cast<lgck::Key::Code>(CKeyTranslator::translateText2Lgck(qtKeyText));
        entry.button = static_cast<lgck::Button::JoyButton>(m_doc.findButtonText(q2c(buttonText)));
    }
    settings.endGroup();
}

void MainWindow::writeButtonConfig(QSettings &settings)
{
    settings.beginGroup(INPUTS);
    for (int i=0; i < lgck::Input::Count; ++i) {
        const char *actionName = actionNames[i];
        CGame::JoyStateEntry & entry = m_doc.joyStateEntry(i);
        const char *buttonText = entry.button >= 0 ? m_doc.buttonText(entry.button) : "";
        QString qtKeyText;
        int qtKeyCode = CKeyTranslator::translateLgck2Text(entry.keyCode, qtKeyText);
        Q_UNUSED(qtKeyCode);
        settings.setValue(QString("button_%1").arg(actionName), buttonText);
        settings.setValue(QString("key_%1").arg(actionName), qtKeyText);
    }
    settings.endGroup();
}

void MainWindow::on_actionJoyState_Mapping_triggered()
{
    QString fileFilter = tr("JoyState Dump (*.bin)");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export..."), "", fileFilter);
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".bin", Qt::CaseInsensitive)) {
            fileName.append(".bin");
        }
        QFileWrap file;
        if (file.open(fileName, "wb")) {
            m_doc.exportJoyStateMap(file);
            file.close();
        }  else {
            // write error
            warningMessage( QString(tr("can't write to %1")).arg(fileName) );
        }
    }
}

void MainWindow::updateSkillFlag(int flag, bool bit)
{
    int curr = O_INT(GENERAL, SKILL_FILTER);
    curr = bit ? curr | flag : curr & (CGame::SKILL_FLAG_ALL ^ flag);
    O_SET(GENERAL, SKILL_FILTER, curr);
    emit skillFilterChanged(curr);
}

void MainWindow::on_actionHell_toggled(bool arg1)
{
    updateSkillFlag(1 << CGame::SKILL_HELL, arg1);
}

void MainWindow::on_actionNormal_toggled(bool arg1)
{
    updateSkillFlag(1 << CGame::SKILL_NORMAL, arg1);
}

void MainWindow::on_actionNightmare_toggled(bool arg1)
{
    updateSkillFlag(1 << CGame::SKILL_NIGHTMARE, arg1);
}

void MainWindow::on_actionInsane_toggled(bool arg1)
{
    updateSkillFlag(1 << CGame::SKILL_INSANE, arg1);
}

void MainWindow::on_actionAll_Skills_triggered()
{
    updateSkillFiltersCheckbox(CGame::SKILL_FLAG_ALL);
}

void MainWindow::updateSkillFiltersCheckbox(int skillval)
{
    QAction *skills [] = {
        ui->actionNormal,
        ui->actionNightmare,
        ui->actionHell,
        ui->actionInsane
    };
    for (unsigned int i=0; i < sizeof(skills) / sizeof(QAction*); ++i) {
        int flag = 1 << i;
        skills[i]->setChecked(skillval & flag);
    }
}

void MainWindow::on_actionExport_sounds_triggered()
{
    CDlgExportSounds dlg(this, &m_doc);
    dlg.setWindowTitle(tr("Export Sounds"));
    dlg.exec();
}

