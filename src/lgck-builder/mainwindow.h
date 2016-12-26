/*
    LGCK Builder GUI
    Copyright (C) 1999, 2012  Francois Blanchette

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

class QComboBox;
class QLabel;
class CWEditEvents;
class QAction;
class CLevelScroll;
//class CLevelView;
class CLevelViewGL;
class CToolBoxDock;
class CThreadUpdater;
class QScrollArea;
class CDlgDistributeGame;

#include <QTimer>
#include <QTime>
#include <QMainWindow>
#include <QGLFunctions>
#include "../shared/stdafx.h"
#include "../shared/Game.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTime & getTime();
    void open(QString fileName);
    void initToolBox();
    CGame *getGame();
    void createEventEditor();
    void makeCurrent();

private:
    static char m_fileFilter[];
    static char m_appName[];
    static char m_appTitle[];
    static char m_author[];
    Ui::MainWindow *ui;
    QComboBox *m_comboEvents;
    QComboBox *m_comboLayers;
    //QScrollArea *m_scroll
    CLevelScroll *m_scroll;
    CLevelViewGL *m_lview;
    CToolBoxDock *m_toolBox;
    CWEditEvents *m_editEvents;
    QToolBar *m_levelToolbar;
    QToolBar *m_layerToolbar;
    CGame m_doc;
    QTimer m_timer;
    QTime m_time;
    int m_nextTick;
    bool m_bShowGrid;
    char m_gridColor[8];
    int m_gridSize;
    int m_skill;
    int m_lives;
    int m_start_hp;
    int m_score;
    int m_start_level;
    bool m_bContinue;
    bool m_bShowToolBox;
    int m_viewMode;
    int m_proto;
    int m_event;    
    int m_pathOrgX;
    int m_pathOrgY;
    int m_pathEntry;
    int m_fontSize;
    bool m_bUpdate;
    QString m_updateURL;
    CThreadUpdater *m_updater;
    QString m_uuid;
    QString m_runtime;
    QString m_runtimeArgs;
    int m_rez;
    int m_rezW;
    int m_rezH;
    bool m_runtimeExternal;
    bool m_skipSplash;
    bool maybeSave();
    bool save();
    bool saveAs();
    void warningMessage(const QString message);
    void initToolBar();
    void showStatus(const QString message);
    void testLevel(bool initSounds);
    bool updateTitle();
    void initFileMenu();
    void reloadRecentFileActions();
    void updateRecentFileActions();
    void reloadSettings();
    void hideView(bool hide);
    void showLayerName();
    void adjustViewPort();
    void commitAll();
    void exportGame();
    bool exportGameCore(CDlgDistributeGame &dlg, QString &outMsg);
    QAction** actionShortcuts();
    QStringList & defaultShortcuts();
    void saveSettings();
    enum {
        MaxRecentFiles      = 4,
        TICK_MAX_RATE       = 200,
        TICK_SCALE          = 1000 / 35,
        TOOLBAR_WIDTH       = 250,
        LABEL0_SIZE         = 500,
        VM_EDITOR           = 0,
        VM_GAME             = 1,
        VM_SPRITE_EVENTS    = 2,
        VM_EDIT_PATH        = 3,
        UI_VERSION          = 0
    };
    QLabel *m_labels[3];
    QAction *m_recentFileActs[MaxRecentFiles];
    virtual void closeEvent(QCloseEvent *event);
    virtual void setVisible ( bool visible );
    virtual void resizeEvent (QResizeEvent *event);
    virtual void focusInEvent ( QFocusEvent * event );
    virtual void paintEvent ( QPaintEvent * event );

    void handleGameEvents();
    void goExternalRuntime();
    bool checkExecutible(const QString exec, QString & errMsg);
    void showAppSettings(int tab);
    void formatVersion(QString &ver);

protected:
    void initializeGL();
    void paintGL();

    QOpenGLContext *m_context;

private slots:
    void on_actionRestart_triggered();
    void on_actionDebug_triggered();
    void on_actionQuit_Game_triggered();
    void on_actionPause_triggered();
    void on_actionRemove_All_triggered();
    void on_actionRuntime_Lua_triggered();
    void on_actionEdit_layer_triggered();
    void on_actionNext_layer_triggered();
    void on_actionPrevious_layer_triggered();
    void on_actionDelete_layer_triggered();
    void on_actionCreate_layer_triggered();
    void on_actionMove_Layer_triggered();
    void on_actionGo_to_layer_triggered();
    void on_actionC_declarations_triggered();
    void on_actionConfigure_LGCK_Builder_triggered();
    void on_actionTest_Level_triggered();
    void on_actionAbout_triggered();
    void on_actionNew_file_triggered();
    void on_actionBrint_to_front_triggered();
    void on_actionSend_to_back_triggered();
    void on_actionDelete_Object_triggered();
    void on_actionMove_Level_triggered();
    void on_actionGo_to_level_triggered();
    void on_actionDelete_Level_triggered();
    void on_actionCreate_Level_triggered();
    void on_actionEdit_Game_Info_triggered();
    void on_actionEdit_Level_triggered();
    void on_actionLast_level_triggered();
    void on_actionFirst_level_triggered();
    void on_actionRevert_triggered();
    void on_actionSave_as_triggered();
    void on_actionSave_triggered();
    void on_actionNext_triggered();
    void on_actionPrevious_triggered();
    void on_actionOpen_triggered();
    void on_actionView_Code_triggered();
    void on_actionCreateSprite_triggered();
    void on_actionDocumentation_triggered();
    void on_actionTutorials_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionDelete_triggered();
    void on_actionSearch_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionEdit_Path_triggered();
    void on_actionExport_Sprite_triggered();
    void on_actionView_Source_triggered();
    void on_actionEdit_Object_triggered();
    void on_actionCustomize_triggered();
    void on_actionImages_triggered();
    void on_actionCopy_Object_triggered();
    void on_actionMainToolbar_toggled(bool arg1);
    void on_actionLevelToolbar_toggled(bool arg1);
    void on_actionStatus_bar_toggled(bool arg1);
    void on_action_ShowGrid_toggled(bool arg1);
    void on_action_ShowToolbox_toggled(bool arg1);
    void on_actionLayer_ToolBar_toggled(bool arg1);
    void on_actionScriptWizard_triggered();
    void openRecentFile();
    void viewEvent();
    void comboChanged(int v);
    void showContextMenu(const QPoint&);
    void selectLevel(int index);
    void editLevel(int index);
    void deleteLevel(int index);
    void addLevel();
    void notifyKeyEvent(int keyCode, int state);
    void updateMenus();
    void setStatus(int i, const QString message);
    void setViewMode(int viewMode);
    void changeViewMode();
    void deleteSprite(int sprite);
    void changeSprite(int sprite);
    void reloadEventCombo();
    void reloadLayerCombo();
    void showEventStatus();
    void showToolBox(bool show);
    void layerChanged(int layerId);
    void closePath();
    void updateEditor(const QString & url, const QString &ver);
    void checkVersion();
    void on_actionIncrease_Font_Size_triggered();
    void on_actionDecrease_Font_Size_triggered();
    void on_actionReset_Font_Size_triggered();
    void on_actionEdit_Images_triggered();
    void on_actionImport_Font_triggered();
    void on_actionSprite_Editor_triggered();
    void on_actionExport_Game_triggered();
    void on_actionDistribution_Package_triggered();

signals:
    void levelDeleted(int index);
    void levelMoved(int fromIndex, int toIndex);
    void levelEdited(int index);
    void levelSelected(int index);
    void levelAdded();    
    void menuSeekingItems(QMenu *, int);
    void protoChanged(int proto, int event);
    void viewModeChanges(int viewMode);
    void eventChanged(int event);
    void eventsCommited();
    void spriteDeleted(int sprite);
    void gameModeEnabled(bool set);
    void eventStatusShown();
    void spriteCreated();
    void gridVisible(bool);
    void gridSizeChanged(int);
    void gridColorChanged(const QString &);
    void pathStarted();
    void pathEnded();
    void updateScene();
    void scrollbarShown(bool);
    void focusGL();
    void textInserted(const char *text);
    void fontSizeChanged(int);
    void frameSetChanged(int fs);
};

#endif // MAINWINDOW_H
