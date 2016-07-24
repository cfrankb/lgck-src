#-------------------------------------------------
#
# Project created by QtCreator 2010-08-04T19:56:47
#
#-------------------------------------------------

win32:CONFIG        += static
win32:INCLUDEPATH   += ../../redist/headers
win32:INCLUDEPATH   += ../../redist/headers/freetype2
INCLUDEPATH         += ../shared
unix:INCLUDEPATH    += /usr/include/freetype2
win32:RC_FILE       = lgck-builder.rc
unix:DEFINES        += MAKE_LINUX=1
QT                  += core gui opengl network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET              = lgck-builder
TEMPLATE            = app
QMAKE_CXXFLAGS      += -std=c++0x -O3
#QMAKE_CXXFLAGS += -std=c++0x -g3
RESOURCES           += lgck-builder.qrc
DEFINES             += USE_QFILE=1

unix:LIBS += -lqt5scintilla2 \
    -llua5.2 \
    -lSDL2_mixer-2.0 \
    -lSDL2-2.0 \
    -lz

win32:LIBS += -L../../redist/lib \
    -lqscintilla2 \
    -llua52 \
    -lSDL2 \
    -lSDL2_mixer \
    -lz

SOURCES +=  mainwindow.cpp \
    main.cpp \
    LevelView.cpp \
    DlgSource.cpp \
    DlgSkill.cpp \
    DlgGotoLevel.cpp \
    DlgEditLevel.cpp \
    DlgGame.cpp \
    DlgAppSettings.cpp \
    DlgAbout.cpp \
    DlgObject.cpp \
    DlgLayer.cpp \
    DlgFrameSet.cpp \
    DlgEntry.cpp \
    DlgAnimation.cpp \
    DlgPath.cpp \
    DlgPathDir.cpp \
    ToolBoxDock.cpp \
    WSpriteList.cpp \
    WizSprite.cpp \
    WFrameSet.cpp \
    WEdit.cpp \
    Lexer.cpp \
    WEditEvents.cpp \
    DlgTestLevel.cpp \
    WizPageEasy.cpp \
    WizPageGeneral.cpp \
    WizPageImages.cpp \
    WizFrameSet.cpp \
    WizPageImport.cpp \
    DlgSelect.cpp \
    WizGame.cpp \
    WAnimation.cpp \
    WHotKey.cpp \
    Pixel.cpp \
    WFileSave.cpp \
    ../shared/SndArray.cpp \
    ../shared/Settings.cpp \
    ../shared/ProtoArray.cpp \
    ../shared/FrameArray.cpp \
    ../shared/LuaVM.cpp \
    ../shared/LevelEntry.cpp \
    ../shared/Level.cpp \
    ../shared/Layer.cpp \
    ../shared/Game.cpp \
    ../shared/Folders.cpp \
    ../shared/FileWrap.cpp \
    ../shared/Animation.cpp \
    ../shared/Object.cpp \
    ../shared/Path.cpp \
    ../shared/GameFile.cpp \
    ../shared/GameLua.cpp \
    ../shared/GameEvents.cpp \
    ../shared/Scene.cpp \
    ../shared/Actor.cpp \
    ../shared/Map.cpp \
    ../shared/FrameSet.cpp \
    ../shared/Frame.cpp \
    ../shared/Inventory.cpp \
    ../shared/Monster.cpp \
    ../shared/Drone.cpp \
    ../shared/Selection.cpp \
    ../shared/PngMagic.cpp \
    ../shared/StringTable.cpp \
    ../shared/Tasks.cpp \
    ../shared/DotArray.cpp \
    ../shared/Attacker.cpp \
    ../shared/PathBlock.cpp \
    ../shared/inventoryTable.cpp \
    ../shared/Display.cpp \
    ../shared/DisplayManager.cpp \
    ../shared/Font.cpp \
    ../shared/implementers/opengl/im_opengl.cpp \
    ../shared/implementers/opengl/gr_opengl.cpp \
    ../shared/implementers/sdl/mu_sdl.cpp \
    ../shared/implementers/sdl/sn_sdl.cpp \
    ../shared/implementers/sdl/im_sdl.cpp \
    ../shared/implementers/sdl/gr_sdl.cpp \
    ../shared/inputs/qt/kt_qt.cpp \
    ../shared/qtgui/qtlua.cpp \
    ../shared/helper.cpp \
    WizScript.cpp \
    wizhead.cpp \
    wbtncolor.cpp \
    thread_updater.cpp \
    ../shared/Snapshot.cpp \
    ../shared/FileMem.cpp \
    OBL5File.cpp \
    WizFont.cpp \
    DlgExportSprite.cpp

HEADERS  +=  mainwindow.h \
    LevelView.h \
    DlgSource.h \
    DlgSkill.h \
    DlgObject.h \
    DlgGotoLevel.h \
    DlgAppSettings.h \
    DlgGame.h \
    DlgAbout.h \
    DlgLayer.h \
    DlgFrameSet.h \
    DlgEntry.h \
    DlgEditLevel.h \
    DlgAnimation.h \
    DlgPath.h \
    DlgPathDir.h \
    ToolBoxDock.h \
    WSpriteList.h \
    WizSprite.h \
    WFrameSet.h \
    WEdit.h \
    Lexer.h \
    WEditEvents.h \
    DlgTestLevel.h \
    WizPageEasy.h \
    WizPageGeneral.h \
    WizPageImages.h \
    WizFrameSet.h \
    WizPageImport.h \
    DlgSelect.h \
    WizGame.h \
    WAnimation.h \
    WHotKey.h \
    ../shared/SndArray.h \
    ../shared/Snd.h \
    ../shared/Settings.h \
    ../shared/ProtoArray.h \
    ../shared/Proto.h \
    ../shared/FrameArray.h \
    ../shared/LuaVM.h \
    ../shared/vlamits3.h \
    ../shared/LevelEntry.h \
    ../shared/Level.h \
    ../shared/Layer.h \    
    ../shared/Game.h \
    ../shared/Folders.h \
    ../shared/FileWrap.h \
    ../shared/Animation.h \
    ../shared/Object.h \
    ../shared/Path.h \
    ../shared/GameFile.h \
    ../shared/GameLua.h \
    ../shared/GameEvents.h \
    ../shared/Scene.h \
    ../shared/Actor.h \
    ../shared/Map.h \
    ../shared/Credits.h \
    ../shared/FrameSet.h \
    ../shared/Frame.h \
    ../shared/Inventory.h \
    ../shared/Monster.h \
    ../shared/Drone.h \
    ../shared/Tasks.h \
    ../shared/DotArray.h \
    ../shared/Attacker.h \
    ../shared/PathBlock.h \
    ../shared/Selection.h \
    ../shared/PngMagic.h \
    ../shared/StringTable.h \
    ../shared/ss_version.h \
    ../shared/inventoryTable.h \
    ../shared/interfaces/IImageManager.h \
    ../shared/interfaces/IGraphics.h \
#    ../shared/implementers/opengl/dm_opengl.h \
    ../shared/implementers/opengl/im_opengl.h \
    ../shared/implementers/opengl/gr_opengl.h \
    ../shared/implementers/sdl/mu_sdl.h \
    ../shared/implementers/sdl/sn_sdl.h \
    ../shared/implementers/sdl/im_sdl.h \
    ../shared/implementers/sdl/gr_sdl.h \
    ../shared/implementers/sdl/dm_sdl.h \
    ../shared/interfaces/IMusic.h \
    ../shared/Display.h \
    ../shared/DisplayManager.h \
    ../shared/interfaces/IDisplayManager.h \
    ../shared/interfaces/ISound.h \
    ../shared/inputs/qt/kt_qt.h \
    ../shared/qtgui/qtlua.h \
    ../shared/helper.h \
    WizScript.h \
    wbtncolor.h \
    thread_updater.h \
    ../shared/Snapshot.h \
    ../shared/Extra.h \
    ../shared/FileMem.h \
    ../shared/IFile.h \
    ../shared/Font.h \
    ../shared/implementers/opengl/glhelper.h \
    WFileSave.h \
    OBL5File.h \
    WizFont.h \
    DlgExportSprite.h

FORMS  += mainwindow.ui \
    DlgSource.ui \
    DlgSkill.ui \
    DlgObject.ui \
    DlgGotoLevel.ui \
    DlgAppSettings.ui \
    DlgGame.ui \
    DlgFrameSet.ui \
    DlgAbout.ui \
    DlgLayer.ui \
    DlgEntry.ui \
    DlgEditLevel.ui \
    DlgAnimation.ui \
    DlgPath.ui \
    DlgPathDir.ui \
    DlgSelect.ui \
    DlgTestLevel.ui \
    ToolBoxDock.ui \
    WizSprite.ui \
    WEditEvents.ui \
    WizFrameSet.ui \
    WizGame.ui \
    WizScript.ui \
    WizFont.ui \
    DlgExportSprite.ui

