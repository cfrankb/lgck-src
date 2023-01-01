#-------------------------------------------------
#
# Project created by QtCreator 2010-08-04T19:56:47
#
#-------------------------------------------------

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:CONFIG        += no_lflags_merge
win32:INCLUDEPATH   += ../headers
INCLUDEPATH         += ../shared
win32:RC_FILE       = lgck-builder.rc
unix:DEFINES        += MAKE_LINUX=1
win32:DEFINES       += MAKE_WIN32
DEFINES             += USE_QFILE=1
DEFINES             += LGCK_QT=1
DEFINES            += LGCK_GAMEPAD=1
QT                  += core gui opengl network gamepad
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET              = lgck-builder
TEMPLATE            = app
QMAKE_CXXFLAGS_RELEASE += -std=c++0x -O3
QMAKE_CXXFLAGS_DEBUG += -std=c++0x -g3
QMAKE_LFLAGS_WINDOWS += -Wl,--dynamicbase -Wl,--nxcompat
RESOURCES           += lgck-builder.qrc

win32:QMAKE_LIBDIR  += ../libs

CONFIG( debug, debug|release ) {
    # debug
    win32:DEFINES   += LGCK_OPENGL_DEBUG=1
} else {
    # release
}

unix:LIBS += \
    -llua5.2 \
    -lSDL2_mixer \
    -lSDL2-2.0 \
    -lz

win32:LIBS += \
    -llua \
    #-lmingw32 \
    -lSDL2_mixer \
    -lSDL2 \
    -lzlib \
    -lopengl32

SOURCES += \
    ../shared/Actor.cpp \
    ../shared/Animation.cpp \
    ../shared/Attacker.cpp \
    ../shared/Countdown.cpp \
    ../shared/compat/asprintf.cpp \
    ../shared/Display.cpp \
    ../shared/displayconfig.cpp \
    ../shared/DisplayManager.cpp \
    ../shared/DotArray.cpp \
    ../shared/Drone.cpp \
    ../shared/FileMem.cpp \
    ../shared/FileWrap.cpp \
    ../shared/Folders.cpp \
    ../shared/Font.cpp \
    ../shared/fontmanager.cpp \
    ../shared/Frame.cpp \
    ../shared/FrameArray.cpp \
    ../shared/FrameSet.cpp \
    ../shared/Game.cpp \
    ../shared/GameEvents.cpp \
    ../shared/GameFile.cpp \
    ../shared/GameLua.cpp \
    ../shared/helper.cpp \
    ../shared/implementers/opengl/gr_opengl.cpp \
    ../shared/implementers/opengl/im_opengl.cpp \
    ../shared/implementers/sdl/gr_sdl.cpp \
    ../shared/implementers/sdl/im_sdl.cpp \
    ../shared/implementers/sdl/mu_sdl.cpp \
    ../shared/implementers/sdl/sn_sdl.cpp \
    ../shared/inputs/qt/kt_qt.cpp \
    ../shared/interfaces/IGraphics.cpp \
    ../shared/Inventory.cpp \
    ../shared/inventoryTable.cpp \
    ../shared/Layer.cpp \
    ../shared/Level.cpp \
    ../shared/LevelEntry.cpp \
    ../shared/LuaVM.cpp \
    ../shared/Map.cpp \
    ../shared/Monster.cpp \
    ../shared/Object.cpp \
    ../shared/Path.cpp \
    ../shared/PathBlock.cpp \
    ../shared/PngMagic.cpp \
    ../shared/Proto.cpp \
    ../shared/ProtoArray.cpp \
    ../shared/ProtoIndex.cpp \
    ../shared/qtgui/qfilewrap.cpp \
    ../shared/qtgui/qthelper.cpp \
    ../shared/qtgui/qtlua.cpp \
    ../shared/Scene.cpp \
    ../shared/Selection.cpp \
    ../shared/Settings.cpp \
    ../shared/Snapshot.cpp \
    ../shared/SndArray.cpp \
    ../shared/StringTable.cpp \
    ../shared/Tasks.cpp \
    ../shared/microtime.cpp \
    DlgAbout.cpp \
    DlgAnimation.cpp \
    DlgAppSettings.cpp \
    dlgdisplay.cpp \
    DlgDistributeGame.cpp \
    DlgEditLevel.cpp \
    DlgEntry.cpp \
    DlgExportSprite.cpp \
    DlgFrameSet.cpp \
    DlgGame.cpp \
    DlgGotoLevel.cpp \
    dlgindicator.cpp \
    DlgLayer.cpp \
    DlgObject.cpp \
    DlgPath.cpp \
    DlgPathDir.cpp \
    DlgSelect.cpp \
    DlgSkill.cpp \
    DlgSource.cpp \
    DlgTestLevel.cpp \
    exportgame.cpp \
    gamefixer.cpp \
    infodock.cpp \
    launcher.cpp \
    levelscroll.cpp \
    levelviewgl.cpp \
    luahighlighter.cpp \
    main.cpp \
    mainwindow.cpp \
    OBL5File.cpp \
    optiongroup.cpp \
    options.cpp \
    Pixel.cpp \
    thread_updater.cpp \
    ToolBoxDock.cpp \
    WAnimation.cpp \
    wbtncolor.cpp \
    WEdit.cpp \
    WEditEvents.cpp \
    WFileDialog.cpp \
    WFrameSet.cpp \
    WGetKey.cpp \
    WHotKey.cpp \
    WizFont.cpp \
    WizFrameSet.cpp \
    WizGame.cpp \
    wizhead.cpp \
    WizPageEasy.cpp \
    WizPageGeneral.cpp \
    WizPageImages.cpp \
    WizPageImport.cpp \
    WizScript.cpp \
    WizSprite.cpp \
    wslider.cpp \
    WSpriteList.cpp

HEADERS += \
    ../shared/Actor.h \
    ../shared/Animation.h \
    ../shared/Attacker.h \
    ../shared/compat/asprintf.h \
    ../shared/Countdown.h \
    ../shared/Credits.h \
    ../shared/Display.h \
    ../shared/displayconfig.h \
    ../shared/DisplayManager.h \
    ../shared/DotArray.h \
    ../shared/Drone.h \
    ../shared/Extra.h \
    ../shared/FileMem.h \
    ../shared/FileWrap.h \
    ../shared/Folders.h \
    ../shared/Font.h \
    ../shared/fontmanager.h \
    ../shared/Frame.h \
    ../shared/FrameArray.h \
    ../shared/FrameSet.h \
    ../shared/Game.h \
    ../shared/GameEvents.h \
    ../shared/GameFile.h \
    ../shared/GameLua.h \
    ../shared/helper.h \
    ../shared/IFile.h \
    ../shared/implementers/opengl/glhelper.h \
    ../shared/implementers/opengl/gr_opengl.h \
    ../shared/implementers/opengl/im_opengl.h \
    ../shared/implementers/sdl/gr_sdl.h \
    ../shared/implementers/sdl/im_sdl.h \
    ../shared/implementers/sdl/mu_sdl.h \
    ../shared/implementers/sdl/sn_sdl.h \
    ../shared/inputs/qt/kt_qt.h \
    ../shared/interfaces/IDisplayManager.h \
    ../shared/interfaces/IGraphics.h \
    ../shared/interfaces/IImageManager.h \
    ../shared/interfaces/IMusic.h \
    ../shared/interfaces/ISound.h \
    ../shared/Inventory.h \
    ../shared/inventoryTable.h \
    ../shared/ISerial.h \
    ../shared/Layer.h \
    ../shared/Level.h \
    ../shared/LevelEntry.h \
    ../shared/LuaVM.h \
    ../shared/Map.h \
    ../shared/Monster.h \
    ../shared/Object.h \
    ../shared/Path.h \
    ../shared/PathBlock.h \
    ../shared/PngMagic.h \
    ../shared/Proto.h \
    ../shared/ProtoArray.h \
    ../shared/ProtoIndex.h \
    ../shared/qtgui/qfilewrap.h \
    ../shared/qtgui/qthelper.h \
    ../shared/qtgui/qtlua.h \
    ../shared/Scene.h \
    ../shared/Selection.h \
    ../shared/Settings.h \
    ../shared/Snapshot.h \
    ../shared/Snd.h \
    ../shared/SndArray.h \
    ../shared/ss_version.h \
    ../shared/StringTable.h \
    ../shared/Tasks.h \
    ../shared/vlamits3.h \
    ../shared/microtime.h \
    DlgAbout.h \
    DlgAnimation.h \
    DlgAppSettings.h \
    dlgdisplay.h \
    DlgDistributeGame.h \
    DlgEditLevel.h \
    DlgEntry.h \
    DlgExportSprite.h \
    DlgFrameSet.h \
    DlgGame.h \
    DlgGotoLevel.h \
    dlgindicator.h \
    DlgLayer.h \
    DlgObject.h \
    DlgPath.h \
    DlgPathDir.h \
    DlgSelect.h \
    DlgSkill.h \
    DlgSource.h \
    DlgTestLevel.h \
    exportgame.h \
    gamefixer.h \
    infodock.h \
    launcher.h \
    levelscroll.h \
    levelviewgl.h \
    luahighlighter.h \
    mainwindow.h \
    OBL5File.h \
    optiongroup.h \
    options.h \
    thread_updater.h \
    ToolBoxDock.h \
    WAnimation.h \
    wbtncolor.h \
    WEdit.h \
    WEditEvents.h \
    WFileDialog.h \
    WFrameSet.h \
    WGetKey.h \
    WHotKey.h \
    WizFont.h \
    WizFrameSet.h \
    WizGame.h \
    WizPageEasy.h \
    WizPageGeneral.h \
    WizPageImages.h \
    WizPageImport.h \
    WizScript.h \
    WizSprite.h \
    wslider.h \
    WSpriteList.h

FORMS += \
    DlgAbout.ui \
    DlgAnimation.ui \
    DlgAppSettings.ui \
    dlgdisplay.ui \
    DlgDistributeGame.ui \
    DlgEditLevel.ui \
    DlgEntry.ui \
    DlgExportSprite.ui \
    DlgFrameSet.ui \
    DlgGame.ui \
    DlgGotoLevel.ui \
    dlgindicator.ui \
    DlgLayer.ui \
    DlgObject.ui \
    DlgPath.ui \
    DlgPathDir.ui \
    DlgSelect.ui \
    DlgSkill.ui \
    DlgSource.ui \
    DlgTestLevel.ui \
    infodock.ui \
    mainwindow.ui \
    ToolBoxDock.ui \
    WEditEvents.ui \
    WizFont.ui \
    WizFrameSet.ui \
    WizGame.ui \
    WizScript.ui \
    WizSprite.ui
