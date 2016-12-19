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
#include "../shared/stdafx.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/FileWrap.h"
#include "../shared/Level.h"
#include "../shared/Layer.h"
#include "../shared/StringTable.h"
#include "../shared/PathBlock.h"
#include "../shared/Path.h"
#include "../shared/interfaces/IImageManager.h"
#include "../shared/interfaces/IMusic.h"
#include "../shared/interfaces/ISound.h"
#include "../shared/implementers/opengl/im_qtopengl.h"
#include "../shared/implementers/sdl/mu_sdl.h"
#include "../shared/implementers/sdl/sn_sdl.h"
#include "../shared/GameEvents.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DlgEditLevel.h"
#include "DlgGotoLevel.h"
#include "DlgGame.h"
#include "DlgEntry.h"
#include "DlgAbout.h"
#include "DlgAppSettings.h"
#include "DlgLayer.h"
#include "DlgSkill.h"
#include "DlgSource.h"
#include "DlgObject.h"
#include "DlgFrameSet.h"
#include "DlgTestLevel.h"
#include "DlgExportSprite.h"
#include "WSpriteList.h"
#include "WEditEvents.h"
#include "WizFrameSet.h"
#include "WizGame.h"
#include "WizScript.h"
#include "Snapshot.h"
#include "ToolBoxDock.h"
#include "thread_updater.h"
#include "../shared/ss_version.h"
#include "WizFont.h"
#include <QSysInfo>
#include "levelviewgl.h"
#include "levelscroll.h"
#include "helper.h"

char MainWindow::m_fileFilter[] = "LGCK games (*.lgckdb)";
char MainWindow::m_appName[] = "LGCK builder";
char MainWindow::m_appTitle[] = "LGCK builder IDE";
char MainWindow::m_author[] = "cfrankb";

#define WEB_PATH QString("http://cfrankb.com/lgck/")
#define UPDATER_URL "http://cfrankb.com/lgck/api/chkv.php?ver=%s&driver=%s&os=%s&uuid=%s&product=%s"

#define MAX_FONT_SIZE 50
#define MIN_FONT_SIZE 10
#define DEFAULT_FONT_SIZE MIN_FONT_SIZE
#define RUNTIME_DEFAULT_ARGS "%1"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_proto = -1;
    m_event = -1;
    m_gridColor[0] = 0;
    m_gridSize = 32;
    m_fontSize = DEFAULT_FONT_SIZE;
    m_viewMode = VM_EDITOR;
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

    m_labels[0] = new QLabel("", ui->statusBar, 0);
    ui->statusBar->addWidget(m_labels[0], LABEL0_SIZE);
    m_labels[2] = new QLabel("", ui->statusBar, 0);
    ui->statusBar->addWidget(m_labels[2], 150);
    m_labels[2]->setAlignment(Qt::AlignRight);
    m_labels[1] = new QLabel("", ui->statusBar, 0);
    m_labels[1]->setAlignment(Qt::AlignRight);
    ui->statusBar->addWidget(m_labels[1], 640 - LABEL0_SIZE);

    updateTitle();
    initFileMenu();
    updateMenus();

    // Setup the timer
    m_timer.setInterval(1000 / TICK_MAX_RATE);
    m_timer.start();
    m_time.start();
    m_nextTick = m_time.elapsed() + TICK_SCALE;

    // create the toolbox
    m_toolBox = new CToolBoxDock(this);
    m_toolBox->hide();
    m_toolBox->setGameDB(&m_doc);
    this->addDockWidget(Qt::LeftDockWidgetArea, m_toolBox);
    m_toolBox->setMaximumWidth(TOOLBAR_WIDTH);
    m_toolBox->setMinimumWidth(TOOLBAR_WIDTH);

    connect(this, SIGNAL(frameSetChanged(int)),
            m_toolBox, SLOT(updateFrameSet(int)));

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(viewEvent()));
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
            this, SLOT(showToolBox(bool)));

    connect(m_toolBox, SIGNAL(spriteChanged(int)),
            this, SLOT(changeSprite(int)));

    connect(this, SIGNAL(levelMoved(int, int)),
            m_toolBox, SLOT(moveLevel(int, int)));

    connect(this, SIGNAL(eventStatusShown()),
            this, SLOT(showEventStatus()));

    connect(this, SIGNAL(spriteCreated()),
            m_toolBox, SLOT(createSprite()));

    connect(this, SIGNAL(gridVisible(bool)),
            m_lview, SLOT(showGrid(bool)));

    connect(this, SIGNAL(scrollbarShown(bool)),
            m_scroll, SLOT(showScrollbars(bool)));

    connect(this, SIGNAL(gridColorChanged(QString)),
            m_lview, SLOT(setGridColor(QString)));

    connect(this, SIGNAL(gridSizeChanged(int)),
            m_lview, SLOT(setGridSize(int)));

    connect(this, SIGNAL(levelSelected(int)),
            m_scroll, SLOT(changeLevel(int)));

    // reload settings
    reloadSettings();
    CSndSDL *sn = new CSndSDL();
    m_doc.attach((ISound*)sn);
    CMusicSDL *mu = new CMusicSDL();
    m_doc.attach((IMusic*)mu);
    m_updater = new CThreadUpdater();
    if (m_bUpdate) {
        connect(m_updater,SIGNAL(newVersion(QString, QString)),this, SLOT(updateEditor(QString, QString)));
        //checkVersion();
    }
}

void MainWindow::createEventEditor()
{
    // Create the event Editor

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

    connect(this, SIGNAL(fontSizeChanged(int)),
            m_editEvents, SLOT(setFontSize(int)));

    if (m_editEvents) {
        m_editEvents->hide();
    }

    emit fontSizeChanged(m_fontSize);
}

QTime & MainWindow::getTime()
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
    m_lview->setFocus();
    event->ignore();
}

MainWindow::~MainWindow()
{
    delete ui;
    //delete m_lview;
    delete (CMusicSDL*) m_doc.music();
    m_doc.attach((IMusic*)NULL);
    delete (CSndSDL*) m_doc.sound();
    m_doc.attach((ISound*)NULL);
    while (m_updater && m_updater->isRunning());
    if (m_updater) {
        delete m_updater;
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
            fileName = QFileDialog::getOpenFileName(this, "", fileName, tr(m_fileFilter));
        }
        if (!fileName.isEmpty()) {
            QString oldFileName = m_doc.getFileName();
            QApplication::setOverrideCursor(Qt::WaitCursor);
            m_doc.setFileName(fileName.toStdString().c_str());
            if (!m_doc.read())  {
                warningMessage(tr("cannot open file:\n") + m_doc.getLastError());
                m_doc.setFileName(q2c(oldFileName));
                // update fileList
                QSettings settings(m_author, m_appName);
                QStringList files = settings.value("recentFileList").toStringList();
                files.removeAll(fileName);
                settings.setValue("recentFileList", files);
            }
            qDebug("MakeCurrent()");
            m_lview->makeCurrent();
            m_doc.cacheImages();
            QApplication::restoreOverrideCursor();
            updateTitle();
            updateRecentFileActions();
            reloadRecentFileActions();
            m_toolBox->init();
            showLayerName();
            reloadLayerCombo();
            setViewMode(VM_EDITOR);
            emit levelSelected(0);
        }
    }
    updateMenus();
    m_doc.initFonts();
}

bool MainWindow::save()
{
    commitAll();
    QString oldFileName = m_doc.getFileName();
    if (m_doc.isUntitled()) {
        if (!saveAs())
            return false;
    }

    if (!m_doc.write() || !updateTitle())  {
        warningMessage(tr("Can't write file"));
        m_doc.setFileName(q2c(oldFileName));
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
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), m_doc.getFileName(), tr(m_fileFilter));
    //qDebug("picked : %s\n", q2c(fileName));
    if (fileName.isEmpty())
        return false;
    m_doc.setFileName(q2c(fileName));
    return true;
}

void MainWindow::warningMessage(const QString message)
{
    QMessageBox msgBox(QMessageBox::Warning, m_appName, message, 0, this);
    msgBox.exec();
}

/*
void MainWindow::setDocument(const QString fileName)
{
    m_doc.setFileName(q2c(fileName));
    m_doc.read();
}
*/

bool MainWindow::updateTitle()
{
    QString file;
    if (!m_doc.getFileName()[0]) {
        file = tr("untitled");
    } else {
        file = QFileInfo(m_doc.getFileName()).fileName();
    }
    setWindowTitle(tr("%1[*] - %2").arg(file).arg(tr(m_appTitle)));
    return true;
}

void MainWindow::on_actionPrevious_triggered()
{
    if (m_doc.m_nCurrLevel) {
        --m_doc.m_nCurrLevel;
        m_lview->setFocus();
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
        m_lview->setFocus();
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
    saveSettings();
}

void MainWindow::on_actionOpen_triggered()
{
    open("");
    m_lview->setFocus();
    updateMenus();
}

void MainWindow::on_actionSave_triggered()
{
    save();
    m_lview->setFocus();
}

void MainWindow::on_actionSave_as_triggered()
{
    if (saveAs()) {
        save();
    }
    m_lview->setFocus();
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
    m_lview->setFocus();
    updateMenus();
}

void MainWindow::initFileMenu()
{
    // gray out the open recent `nothin' yet`
    ui->action_nothin_yet->setEnabled(false);
    for (int i = 0; i < MaxRecentFiles; i++) {
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
    ui->action_nothin_yet->setVisible(numRecentFiles == 0);
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
        CLevel & level = *m_doc.m_arrLevels[ m_doc.m_nCurrLevel ];
        CDlgEditLevel *dlg = new CDlgEditLevel(this);
        dlg->setGameDB(&m_doc);
        QString s;
        s.sprintf(q2c(tr("Edit level %.3d")), m_doc.m_nCurrLevel + 1);
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
    m_lview->setFocus();
    showLayerName();
    reloadLayerCombo();
    delete d;
}

void MainWindow::paintEvent(QPaintEvent * event) {
    event->accept();
}

void MainWindow::updateMenus()
{
    bool result = !(m_viewMode == VM_GAME);
    QAction * actionsGame[] = {
        ui->actionDebug,
        ui->actionQuit_Game,
        ui->actionPause,
        ui->actionRestart,
        NULL
    };

    for (int i=0; actionsGame[i]; ++i) {
        actionsGame[i]->setEnabled( m_viewMode == VM_GAME );
    }

    for (int j = 0; j < MaxRecentFiles; ++j) {
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
            ui->actionCopy_Object,
            ui->actionDelete_Object,
            ui->actionSend_to_back,
            ui->actionBrint_to_front,
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
            NULL
        };
        for (int i=0; actionsEditor[i]; ++i) {
            actionsEditor[i]->setEnabled(false);
        }
        m_comboEvents->setEnabled(false);
        m_comboLayers->setEnabled(false);
    } else {
        result = m_doc.getSize() != 0;
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
            CLevel & level = * m_doc[m_doc.m_nCurrLevel];
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

        ui->actionRemove_All->setEnabled( enableMulti );
        ui->actionBrint_to_front->setEnabled( enableMulti );
        ui->actionSend_to_back->setEnabled( enableMulti );
        ui->actionCopy_Object->setEnabled( enableMulti );
        ui->actionDelete_Object->setEnabled( enableMulti );
        ui->actionCustomize->setEnabled( enable );
        ui->actionEdit_Object->setEnabled( enable );
        ui->actionEdit_Images->setEnabled( enable );
        ui->actionEdit_Path->setEnabled( enable );
        if (result && m_doc.getSize()) {
            CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
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
    }

    ui->actionScriptWizard->setEnabled(m_viewMode == VM_SPRITE_EVENTS);
    //ui->actionScriptWizard->setEnabled(true);
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
        dlg->save( level );
        m_doc.addLevel( level );
        m_doc.setDirty( true );
        m_doc.m_nCurrLevel = m_doc.getSize() - 1;
        updateMenus();
        showLayerName();        
        reloadLayerCombo();
        emit levelAdded();
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
            delete m_doc[ m_doc.m_nCurrLevel ] ;
            m_doc.removeLevel ( m_doc.m_nCurrLevel );
            if ( m_doc.m_nCurrLevel > m_doc.getSize() - 1) {
                -- m_doc.m_nCurrLevel ;
            }
            if (!m_doc.getSize()) {
                m_doc.m_nCurrLevel = -1;
            }
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
        dlg->setLevel( m_doc.m_nCurrLevel );
        if (dlg->exec() == QDialog::Accepted) {
            m_doc.m_nCurrLevel = dlg->getLevel();
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
        dlg->setLevel( m_doc.m_nCurrLevel );
        int indexFrom = m_doc.m_nCurrLevel;
        if (dlg->exec() == QDialog::Accepted
            && m_doc.m_nCurrLevel != dlg->getLevel()) {
            int level = dlg->getLevel();
            CLevel *levelObj = m_doc [ m_doc.m_nCurrLevel ];
            m_doc.removeLevel( m_doc.m_nCurrLevel );
            m_doc.insertLevel( level, levelObj );
            m_doc.m_nCurrLevel = level;
            m_doc.setDirty( true );
            updateMenus();
            showLayerName();
            reloadLayerCombo();
            emit levelMoved(indexFrom, level);
        }
        delete dlg;
    }
}

void MainWindow::on_actionDelete_Object_triggered()
{
    if (m_doc.getSize()
            && m_viewMode == VM_EDITOR) {
        CLevel *script = m_doc [ m_doc.m_nCurrLevel ];
        CLayer *layer = script->getCurrentLayer();
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
        CLevel *level = m_doc [ m_doc.m_nCurrLevel ];
        CLayer & layer =  * (level->getCurrentLayer());
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
        CLevel & level = * m_doc [ m_doc.m_nCurrLevel ];
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

void MainWindow::on_actionBrint_to_front_triggered()
{
    if (m_doc.getSize()
            && m_viewMode == VM_EDITOR) {
        CLevel & level = * m_doc [ m_doc.m_nCurrLevel ];
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
        CDlgTestLevel *dlg = new CDlgTestLevel(this);
        dlg->setSkill(m_skill);
        dlg->setHP(m_start_hp);
        dlg->setScore(m_score);
        dlg->setLives(m_lives);
        dlg->setContinue(m_bContinue);
        dlg->setExternal(m_runtimeExternal);
        dlg->setRez(m_rez);
        if (dlg->exec()) {
            m_skill = dlg->getSkill();
            m_start_hp = dlg->getHP();
            m_score = dlg->getScore();
            m_lives = dlg->getLives();
            m_bContinue = dlg->getContinue();
            m_rez = dlg->getRez();
            m_rezH = dlg->getHeight();
            m_rezW = dlg->getWidth();
            qDebug("skill: %d", m_skill);
            m_doc.clearKeys();
            m_doc.setVitals(m_start_hp, m_lives, m_score);
            m_doc.setSkill(m_skill);
            m_doc.initLua();
            m_runtimeExternal = dlg->isExternal();
            if (dlg->isExternal()){
            // http://stackoverflow.com/questions/19442400/qt-execute-external-program
                goExternalRuntime();
            } else {
                setViewMode(VM_GAME);
                testLevel(true);
            }
            // set the start level
            m_start_level = m_doc.m_nCurrLevel;
        }
        delete dlg;
    }
    m_lview->setFocus();
}

void MainWindow::testLevel(bool initSound)
{   
    QPixmap pixmap(":/images/0000-1.png");
    QSplashScreen splash(pixmap);
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
            if (!m_bContinue) {
                QMessageBox::information(this, tr(m_appName),
                                     tr("You ran out of time."));
                break;
            }

        case CGame::EVENT_PLAYER_DIED:
            --m_doc.counter("lives");
            if (!m_bContinue) {
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
            // set the start level
            m_doc.m_nCurrLevel = m_start_level;
            emit levelSelected(m_start_level);
            emit focusGL();
            m_doc.removePointsOBL();
        }
    }
}

void MainWindow::viewEvent()
{
    m_lview->setFocus();
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
    d->showGrid(m_bShowGrid);
    d->setGridColor(QString(m_gridColor));
    d->setGridSize(m_gridSize);         
    d->setUpdater(m_bUpdate, m_updateURL);
    d->setFontSize(m_fontSize);
    d->setCurrentTab(tab);
    qDebug("Font Size: %d", m_fontSize);
    QAction **actions = actionShortcuts();
    QStringList listActions;
    QStringList listShortcuts;
    for (uint i=0; actions[i];++i) {
        QAction *a = actions[i];
        QKeySequence s = a->shortcut();
        listShortcuts.append(s.toString());
        listActions.append(a->text());
    }
    d->setSkill(m_skill);
    d->setHP(m_start_hp);
    d->setScore(m_score);
    d->setLives(m_lives);
    d->setRuntime(m_runtime, m_runtimeArgs);
    d->setSkipSplashScreen(m_skipSplash);
    d->init();
    d->load(listActions, listShortcuts, defaultShortcuts());
    if (d->exec() == QDialog::Accepted) {
        d->save(listShortcuts);
        d->getUpdater(m_bUpdate, m_updateURL);
        for (uint i=0; actions[i];++i) {
            QAction *a = actions[i];
            a->setShortcut(QKeySequence(listShortcuts[i]));
        }
        m_bShowGrid = d->isShowGrid();
        emit gridVisible(m_bShowGrid);
        emit gridSizeChanged(d->getGridSize());
        m_gridSize = d->getGridSize();
        emit gridColorChanged(d->getGridColor());
        strcpy(m_gridColor, q2c(d->getGridColor().mid(0,6)));
        ui->action_ShowGrid->setChecked(m_bShowGrid);
        m_lview->repaint();
        m_skill = d->getSkill();
        m_start_hp = d->getHP();
        m_score = d->getScore();
        m_lives = d->getLives();
        m_fontSize = d->getFontSize();
        d->getRuntime(m_runtime, m_runtimeArgs);
        emit fontSizeChanged(m_fontSize);
        m_skipSplash = d->getSkipSplashScreen();
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
        CLevel & level = *m_doc.m_arrLevels[ m_doc.m_nCurrLevel ];
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
            CLevel & level = *m_doc.m_arrLevels[ m_doc.m_nCurrLevel ];
            int layerId = dlg->getLevel();
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
            CLevel & level = *m_doc.m_arrLevels[ m_doc.m_nCurrLevel ];
            int to = dlg->getLevel();
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
        CLevel & level = *m_doc.m_arrLevels[ m_doc.m_nCurrLevel ];
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

        CLevel & level = *m_doc.m_arrLevels[ m_doc.m_nCurrLevel ];
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
        CLevel & level = *m_doc.m_arrLevels[ m_doc.m_nCurrLevel ];
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
        CLevel & level = *m_doc.m_arrLevels[ m_doc.m_nCurrLevel ];
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
        CLevel & level = *m_doc.m_arrLevels[ m_doc.m_nCurrLevel ];
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
    QString fileFilter = tr("Text files (*.txt)");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export..."), "", tr(q2c(fileFilter)));
    if (!fileName.isEmpty()) {
        CFileWrap file;
        if (file.open(q2c(fileName), "wb")) {
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
                QString s;
                s.sprintf("#define OBJECT_%-40s %d \r\n", q2c(name), n);
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
                    QString s;
                    s.sprintf("#define CLASS_%-40s0x%.2x\r\n",  q2c(name), n);
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
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export..."), "", tr(q2c(fileFilter)));
    if (!fileName.isEmpty()) {
        CFileWrap file;
        if (file.open(q2c(fileName), "wb")) {
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
    ui->toolBar->setObjectName("mainToolbar");
    ui->toolBar->setWindowTitle(tr("Main"));
    ui->toolBar->setIconSize( QSize(16,16) );
    ui->toolBar->addAction(ui->actionNew_file);
    ui->toolBar->addAction(ui->actionOpen);
    ui->toolBar->addAction(ui->actionSave);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionCreate_Level);
    ui->toolBar->addAction(ui->actionEdit_Level);
    ui->toolBar->addAction(ui->actionTest_Level);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionPrevious);
    ui->toolBar->addAction(ui->actionNext);
    ui->toolBar->addAction(ui->actionGo_to_level);
    m_comboLayers = new QComboBox(this);
    m_comboLayers->setDisabled(true);
    m_layerToolbar = new QToolBar("Layer", this);
    m_layerToolbar->setIconSize( QSize(16,16) );
    m_layerToolbar->setObjectName("toolbar2");
    m_layerToolbar->addWidget(m_comboLayers);
    m_layerToolbar->addAction(ui->actionCreate_layer);
    m_layerToolbar->addAction(ui->actionEdit_layer);
    m_layerToolbar->addAction(ui->actionDelete_layer);
    addToolBar(m_layerToolbar);
    m_levelToolbar = new QToolBar("Level", this);
    m_levelToolbar->setIconSize( QSize(16,16) );
    m_levelToolbar->setObjectName("toolbar1");
    m_comboEvents = new QComboBox(this);
    reloadEventCombo();
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
    connect(m_comboEvents, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));
    connect(m_comboLayers, SIGNAL(currentIndexChanged(int)), this, SLOT(layerChanged(int)));
    connect(m_comboEvents, SIGNAL(highlighted(int)), this, SLOT(reloadEventCombo()));
    addToolBar(m_levelToolbar);
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
            CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
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
    if ((m_viewMode == VM_GAME)) {
        m_doc.flipPause();
    }
}

void MainWindow::on_actionQuit_Game_triggered()
{
    if ((m_viewMode == VM_GAME)) {
        setViewMode(VM_EDITOR);
        m_doc.removePointsOBL();
        updateMenus();
        m_doc.stopMusic();
        emit scrollbarShown(true);
    }
}

void MainWindow::on_actionDebug_triggered()
{
    //if ((m_viewMode == VM_GAME)) {
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
        CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
        CLayer & layer = * level.getCurrentLayer();
        if (layer.isSingleSelection()) { //.m_currEntry != -1) {
            CLevelEntry entry = layer.getSelection(0);//[layer.m_currEntry];
            const CProto proto = m_doc.m_arrProto[entry.m_nProto];
            CDlgObject *d = new CDlgObject (this);
            d->setGameDB(&m_doc);
            d->load( entry.m_nProto );
            if (d->exec() == QDialog::Accepted) {
                d->save( entry.m_nProto );
                //    if (gf.m_arrProto.getObject( m_proto ) != object) {
                if (proto != m_doc.m_arrProto[ entry.m_nProto ]) {
                    m_doc.setDirty(true);
                }
            }
            delete d;
        }
    }
}

void MainWindow::on_actionCustomize_triggered()
{
    if (m_doc.getSize()) {
        CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
        CLayer & layer = * level.getCurrentLayer();
        if (layer.isSingleSelection()) {
            CDlgEntry *d = new CDlgEntry (this);
            d->setGameDB( & m_doc);

            int triggers[CGameFile::TRIGGER_KEYS + 1];
            memset(&triggers, 0, sizeof(triggers));
            for (int i=0; i < layer.getSize(); ++i) {
                int trigger = layer[i].m_nTriggerKey & CGameFile::TRIGGER_KEYS;
                ++triggers[trigger];
            }
            d->init(triggers);
            d->load(layer.getSelectionIndex(0));
            CLevelEntry entry = layer.getSelection(0);
            if (d->exec() == QDialog::Accepted) {
                d->save(layer.getSelectionIndex(0));
                if (entry != layer.getSelection(0)) {
                    m_doc.setDirty( true );
                }
            }
            delete d;
        }
    }
}

void MainWindow::on_actionImages_triggered()
{
    CWizFrameSet *wiz = new CWizFrameSet( (QWidget*)parent() );
    wiz->init(m_doc.m_arrFrames.getSize());
    if (wiz->exec()) {
        CFrameSet *frameSet = new CFrameSet (wiz->getFrameSet());
        char name[32];
        strcpy(name, wiz->getName());
        frameSet->setName(name);
        m_doc.m_arrFrames.add(frameSet);
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
            CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
            CLayer & layer = * level.getCurrentLayer();
            int x = pos.x() & 0xfff8;
            int y = pos.y() & 0xfff8;
            CGameFile * gf = (CGameFile*) &m_doc;
            int index = gf->whoIs( level, x + level.m_mx, y + level.m_my);
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
                menu.addAction(ui->actionBrint_to_front);
                menu.addSeparator();
                menu.addAction(ui->actionCut);
                menu.addAction(ui->actionDelete);
                menu.addAction(ui->actionRemove_All);
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
        CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
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
        CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
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
        CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
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
    QString url = QString("%1manual.htm?v=%2.%3.%4.%5").arg(WEB_PATH)
            .arg(CGame::getVersion() >> 24 & 0xff)
            .arg(CGame::getVersion() >> 16 & 0xff)
            .arg(CGame::getVersion() >> 8 & 0xff)
            .arg(CGame::getVersion() & 0xff);
    QDesktopServices::openUrl(url);
}

void MainWindow::on_actionTutorials_triggered()
{
    QString url = QString("%1tutorial.htm?v=%2.%3.%4.%5").arg(WEB_PATH)
            .arg(CGame::getVersion() >> 24 & 0xff)
            .arg(CGame::getVersion() >> 16 & 0xff)
            .arg(CGame::getVersion() >> 8 & 0xff)
            .arg(CGame::getVersion() & 0xff);
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
        CLevel *script = m_doc [ m_doc.m_nCurrLevel ];
        CLayer *layer = script->getCurrentLayer();
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
            clipboard->applyDelta(script->m_mx, script->m_my);
      //      qDebug("mx=%d my=%d",script->m_mx, script->m_my);
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
        CLevel *script = m_doc [ m_doc.m_nCurrLevel ];
        CLayer *layer = script->getCurrentLayer();
        if (layer->isMultiSelection()
                || layer->isSingleSelection()) {
            // copy sprites tp clippy
            CSelection *clipboard = m_doc.getClipboard();
            clipboard->clear();
            for (int i=0; i < layer->getSelectionSize(); ++i) {
    //            qDebug("copy select %d", i);
                CLevelEntry & entry = layer->getSelection(i);
                int j = layer->getSelectionIndex(i);
                clipboard->addEntry(entry,j);
            }
            clipboard->applyDelta(script->m_mx, script->m_my);
  //          qDebug("clipboard size: %d", clipboard->getSize());
            updateMenus();
        }
    }
}

void MainWindow::on_actionPaste_triggered()
{
    if (m_doc.getSize() && !(m_viewMode == VM_GAME)) {
        CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
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
        CLevel *script = m_doc [ m_doc.m_nCurrLevel ];
        CLayer *layer = script->getCurrentLayer();
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
        CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
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
        ui->actionBrint_to_front,
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
        NULL
    };
    return actions;
}

void MainWindow::saveSettings()
{
    setViewMode(VM_EDITOR);
    QSettings settings(m_author, m_appName);
    settings.setValue("saveSettings", ui->actionSave_Settings->isChecked());
    bool saveSettings = ui->actionSave_Settings->isChecked();
    if (saveSettings) {
        settings.setValue("showGrid", m_bShowGrid);
        settings.setValue("gridColor", QString(m_gridColor));
        settings.setValue("gridSize", m_gridSize);
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
        settings.setValue("toolBox", m_bShowToolBox);
        settings.endGroup();
        settings.beginGroup("UI_Components");
        settings.setValue("mainWindow:geometry", this->saveGeometry());
        settings.setValue("mainWindow:state", this->saveState());
        settings.setValue("version", UI_VERSION);
       // settings.setValue("mainWindow:pos", this->geometry());
        settings.endGroup();
        settings.beginGroup("TestLevel");
        settings.setValue("skill", m_skill);
        settings.setValue("start_hp", m_start_hp);
        settings.setValue("score", m_score);
        settings.setValue("lives", m_lives);
        settings.setValue("continue", m_bContinue);
        settings.endGroup();
        settings.beginGroup("Updater");
        settings.setValue("updater_check", m_bUpdate);
        settings.setValue("updater_url", m_updateURL);
        settings.setValue("uuid", m_uuid);
        QString ver;
        formatVersion(ver);
        settings.setValue("version", ver);
        settings.endGroup();
        settings.beginGroup("Editor");
        settings.setValue("fontSize", m_fontSize);
        settings.setValue("skipSplash", m_skipSplash);
        settings.endGroup();
        // Runtime
        settings.beginGroup("Runtime");
        settings.setValue("path", m_runtime);
        settings.setValue("args", m_runtimeArgs);
        settings.setValue("external", m_runtimeExternal);
        settings.setValue("rez", m_rez);
        settings.endGroup();
        settings.sync();
    }
}

void MainWindow::formatVersion(QString &ver)
{
    int version = SS_LGCK_VERSION;
    int vv[4]={0,0,0,0};
    for (int i=3; i >= 0; --i) {
        vv[i] = version & 0xff;
        version /= 256;
    }
    ver = QString().sprintf("%.2d.%.2d.%.2d.%.2d", vv[0], vv[1], vv[2], vv[3]);
}

void MainWindow::reloadSettings()
{
    QSettings settings(m_author, m_appName);
    qDebug() << settings.fileName();
    // grid
    m_bShowGrid = settings.value("showGrid", true).toBool();
    emit gridVisible(m_bShowGrid);
    ui->action_ShowGrid->setChecked(m_bShowGrid);
    QString color = settings.value("gridColor", "a0b0c0").toString();
    emit gridColorChanged(color);
    strcpy(m_gridColor, q2c(color.mid(0,6)));
    m_gridSize = settings.value("gridSize", 32).toInt() & 0xf0;
    emit gridSizeChanged(m_gridSize > 0 ? m_gridSize : 32);
    bool saveSettings = settings.value("saveSettings", true).toBool();
    ui->actionSave_Settings->setChecked(saveSettings);
    // TestLevel
    settings.beginGroup("TestLevel");
    m_skill = settings.value("skill", 0).toInt();
    m_lives = settings.value("lives", 5).toInt();
    m_score = settings.value("score", 0).toInt();
    m_start_hp = settings.value("start_hp", 32).toInt();
    m_bContinue = settings.value("continue", true).toBool();
    settings.endGroup();
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
    m_uuid = settings.value("uuid", uuid).toString();
    delete []uuid;
    QString currVersion;
    formatVersion(currVersion);
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

    settings.beginGroup("Editor");
    m_fontSize = settings.value("fontSize", DEFAULT_FONT_SIZE).toInt();
    m_skipSplash = settings.value("skipSplash", false).toBool();
    emit fontSizeChanged(m_fontSize);
    settings.endGroup();
}

void MainWindow::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    if (visible) {
        QSettings settings(m_author, m_appName);
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
        CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
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
    m_bShowGrid = arg1;
    emit gridVisible(m_bShowGrid);
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
        CLevel & level = *m_doc.m_arrLevels[ m_doc.m_nCurrLevel ];
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
        CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
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


const char *getWindowsVersion()
{
    switch(QSysInfo::windowsVersion())
    {
    case QSysInfo::WV_2000: return "Windows 2000";
    case QSysInfo::WV_XP: return "Windows XP";
    case QSysInfo::WV_VISTA: return "Windows Vista";
    case QSysInfo::WV_WINDOWS7: return "Windows 7";
    case QSysInfo::WV_WINDOWS8: return "Windows 8";
    case QSysInfo::WV_WINDOWS8_1:	return "Windows 8.1";
    case QSysInfo::WV_WINDOWS10: return "Windows 10";
    default: return "Windows";
    }
}

void MainWindow::checkVersion()
{
    int version = SS_LGCK_VERSION;
    int vv[4]={0,0,0,0};
    for (int i=3; i >= 0; --i) {
        vv[i] = version & 0xff;
        version /= 256;
    }

    QString os = "";
#ifdef Q_OS_LINUX
    os = "Linux";
#elif defined(Q_OS_WIN32)
    os = getWindowsVersion();
#elif define(Q_OS_UNIX)
    os = "Unix";
#elif define(Q_OS_DARWIN)
    os = "Mac";
#endif
    os.replace(" ", "+");
    QString productVersion = QSysInfo::productVersion();
    QString productType = QSysInfo::productType();
    QString ver = QString().sprintf("%.2d.%.2d.%.2d.%.2d", vv[0], vv[1], vv[2], vv[3]);
    QString driver = QGuiApplication::platformName();
    QString url = QString().sprintf(q2c(m_updateURL),
                                    q2c(ver),
                                    q2c(driver),
                                    q2c(os),
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
    ++ m_fontSize;
    if (m_fontSize > MAX_FONT_SIZE) {
        m_fontSize = MAX_FONT_SIZE;
    }
    emit fontSizeChanged(m_fontSize);
}

void MainWindow::on_actionDecrease_Font_Size_triggered()
{
    -- m_fontSize;
    if (m_fontSize < MIN_FONT_SIZE) {
        m_fontSize = MIN_FONT_SIZE;
    }
    emit fontSizeChanged(m_fontSize);
}

void MainWindow::on_actionReset_Font_Size_triggered()
{
    m_fontSize = DEFAULT_FONT_SIZE;
    emit fontSizeChanged(m_fontSize);
}

void MainWindow::on_actionEdit_Images_triggered()
{
    if (m_doc.getSize()) {
        CLevel & level = * m_doc[ m_doc.m_nCurrLevel ];
        CLayer & layer = * level.getCurrentLayer();
        if (layer.isSingleSelection()) {
            CLevelEntry entry = layer.getSelection(0);
            const CProto proto = m_doc.m_arrProto[entry.m_nProto];
            int fs = proto.m_nFrameSet;
            CDlgFrameSet * d = new CDlgFrameSet (this);
            d->setWindowTitle ( QString(tr("Edit Image Set `%1`")).arg(m_doc.m_arrFrames[fs]->getName()) );
            CFrameSet * frameSet = new CFrameSet (m_doc.m_arrFrames[fs]);
            d->init(frameSet);
            if (d->exec() == QDialog::Accepted) {
                m_doc.setDirty( true );
                // replace frameSet
                delete m_doc.m_arrFrames[ fs ];
                m_doc.m_arrFrames.setAt( fs, frameSet);
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

void MainWindow::on_actionImport_Font_triggered()
{
    CWizFont *wiz = new CWizFont( static_cast<QWidget*>(parent()) );
    if (wiz->exec()) {
        // TODO: implement this
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
    QStringList tmp = m_runtimeArgs.split(" ", QString::SkipEmptyParts);
    QListIterator<QString> itr(tmp);
    while (itr.hasNext()) {
        QString current = itr.next().trimmed();
        if (!current.isEmpty()) {
            current = current.replace("%1", filename)
                    .replace("%2", QString::number(m_doc.m_nCurrLevel))
                    .replace("%3", QString::number(m_skill))
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
        showAppSettings(CDlgAppSettings::TAB_RUNTIME);
    }
}

void MainWindow::on_actionSprite_Editor_triggered()
{
    QString appDir = QCoreApplication::applicationDirPath();
    qDebug() << appDir;
#ifdef Q_OS_WIN32
    QString cmd = "obl5edit.exe";
#else
    QString cmd = "obl5edit";
#endif
    QString runtime = appDir + "/" + cmd;
    bool result = QProcess::startDetached(runtime);
    if (!result) {
        QString errMsg = tr("Running external editor failed");
        QMessageBox msgBox(QMessageBox::Warning, m_appName, errMsg, 0, this);
        msgBox.exec();
    }
}
