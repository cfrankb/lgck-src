cmake_minimum_required(VERSION 3.10)
project(lgck_builder)
include(CMakePackageConfigHelpers)


set(QT_MIN_VERSION 
    5.15.0
)

set(CMAKE_CXX_STANDARD 
    17
)

set(CMAKE_CXX_STANDARD_REQUIRED 
    ON
)

set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -std=c++0x -O3 -fPIE"
)

set(CMAKE_INCLUDE_CURRENT_DIR 
    ON
)

set(CMAKE_AUTOMOC 
    ON
)

set(CMAKE_BUILD_TYPE 
    Release
)

set(lgckb_SRCS 
    DlgAbout.cpp
    DlgAnimation.cpp
    DlgAppSettings.cpp
    DlgDistributeGame.cpp
    DlgEditFrames.cpp
    DlgEditLevel.cpp
    DlgEntry.cpp
    DlgExportSprite.cpp
    DlgFrameSet.cpp
    DlgGame.cpp
    DlgGotoLevel.cpp
    DlgLayer.cpp
    DlgObject.cpp
    DlgPath.cpp
    DlgPathDir.cpp
    DlgSelect.cpp
    DlgSkill.cpp
    DlgSource.cpp
    DlgTestLevel.cpp
    OBL5File.cpp
    Pixel.cpp
    ToolBoxDock.cpp
    WAnimation.cpp
    WEdit.cpp
    WEditEvents.cpp
    WFileDialog.cpp
    WFrameSet.cpp
    WGetKey.cpp
    WHotKey.cpp
    WSpriteList.cpp
    WizFont.cpp
    WizFrameSet.cpp
    WizGame.cpp
    WizPageEasy.cpp
    WizPageGeneral.cpp
    WizPageImages.cpp
    WizPageImport.cpp
    WizScript.cpp
    WizSprite.cpp
    dlgdisplay.cpp
    dlgindicator.cpp
    exportgame.cpp
    gamefixer.cpp
    infodock.cpp
    launcher.cpp
    levelscroll.cpp
    levelviewgl.cpp
    luahighlighter.cpp
    main.cpp
    mainwindow.cpp
    optiongroup.cpp
    options.cpp
    thread_updater.cpp
    wbtncolor.cpp
    wizhead.cpp
    wslider.cpp
    ../shared/Actor.cpp
    ../shared/Animation.cpp
    ../shared/Attacker.cpp
    ../shared/Const.cpp
    ../shared/Display.cpp
    ../shared/DisplayManager.cpp
    ../shared/DotArray.cpp
    ../shared/DrawList.cpp
    ../shared/Drone.cpp
    ../shared/FileMem.cpp
    ../shared/FileWrap.cpp
    ../shared/Folders.cpp
    ../shared/Font.cpp
    ../shared/Frame.cpp
    ../shared/FrameArray.cpp
    ../shared/FrameSet.cpp
    ../shared/Game.cpp
    ../shared/GameEvents.cpp
    ../shared/GameFile.cpp
    ../shared/GameLua.cpp
    ../shared/Inventory.cpp
    ../shared/Layer.cpp
    ../shared/Level.cpp
    ../shared/LevelEntry.cpp
    ../shared/LuaVM.cpp
    ../shared/Map.cpp
    ../shared/Monster.cpp
    ../shared/Object.cpp
    ../shared/Path.cpp
    ../shared/PathBlock.cpp
    ../shared/PngMagic.cpp
    ../shared/Proto.cpp
    ../shared/ProtoArray.cpp
    ../shared/ProtoIndex.cpp
    ../shared/Scene.cpp
    ../shared/Selection.cpp
    ../shared/Settings.cpp
    ../shared/Snapshot.cpp
    ../shared/SndArray.cpp
    ../shared/StringTable.cpp
    ../shared/Tasks.cpp
    ../shared/displayconfig.cpp
    ../shared/fontmanager.cpp
    ../shared/helper.cpp
    ../shared/inventoryTable.cpp
    ../shared/runtime.cpp
    ../shared/ss_build.cpp
    ../shared/implementers/opengl/gr_opengl.cpp
    ../shared/implementers/opengl/im_opengl.cpp
    #../shared/implementers/sdl/gr_sdl.cpp
    #../shared/implementers/sdl/im_sdl.cpp
    ../shared/implementers/sdl/mu_sdl.cpp
    ../shared/implementers/sdl/sn_sdl.cpp
    ../shared/interfaces/IGraphics.cpp
    ../shared/inputs/qt/kt_qt.cpp
    ../shared/qtgui/qtlua.cpp
)

set(lgckb_ui_files 
    DlgAbout.ui
    DlgAnimation.ui
    DlgAppSettings.ui
    DlgDistributeGame.ui
    DlgEditFrames.ui
    DlgEditLevel.ui
    DlgEntry.ui
    DlgExportSprite.ui
    DlgFrameSet.ui
    DlgGame.ui
    DlgGotoLevel.ui
    DlgLayer.ui
    DlgObject.ui
    DlgPath.ui
    DlgPathDir.ui
    DlgSelect.ui
    DlgSkill.ui
    DlgSource.ui
    DlgTestLevel.ui
    ToolBoxDock.ui
    WEditEvents.ui
    WizFont.ui
    WizFrameSet.ui
    WizGame.ui
    WizScript.ui
    WizSprite.ui
    dlgdisplay.ui
    dlgindicator.ui
    infodock.ui
    mainwindow.ui
)

set(lgckb_MOC_HEADERS 
    DlgAbout.h
    DlgAnimation.h
    DlgAppSettings.h
    DlgDistributeGame.h
    DlgEditFrames.h
    DlgEditLevel.h
    DlgEntry.h
    DlgExportSprite.h
    DlgFrameSet.h
    DlgGame.h
    DlgGotoLevel.h
    DlgLayer.h
    DlgObject.h
    DlgPath.h
    DlgPathDir.h
    DlgSelect.h
    DlgSkill.h
    DlgSource.h
    DlgTestLevel.h
    ToolBoxDock.h
    WAnimation.h
    WEdit.h
    WEditEvents.h
    WFileDialog.h
    WFrameSet.h
    WGetKey.h
    WHotKey.h
    WSpriteList.h
    WizFont.h
    WizFrameSet.h
    WizGame.h
    WizPageEasy.h
    WizPageGeneral.h
    WizPageImages.h
    WizPageImport.h
    WizScript.h
    WizSprite.h
    dlgdisplay.h
    dlgindicator.h
    exportgame.h
    infodock.h
    levelscroll.h
    levelviewgl.h
    luahighlighter.h
    mainwindow.h
    thread_updater.h
    wbtncolor.h
    wslider.h
)

set(lgckb_RCS 
    lgck-builder.qrc
)
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui OpenGL Widgets Network)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

find_library(SDL_MIXER_LIBRARY NAMES SDL2_mixer HINTS ENV SDLMIXERDIR ENV SDLDIR PATH_SUFFIXES lib)

include_directories(${QT_INCLUDES})
include_directories(../shared)
include_directories(./)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DUSE_QFILE=1)
add_definitions(-DLGCK_QT=1)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

QT5_WRAP_UI(lgckb_SRCS ${lgckb_ui_files})
QT5_ADD_RESOURCES(lgckb_RC_SRCS ${lgckb_RCS})
add_executable(lgck-builder ${lgckb_SRCS} ${lgckb_MOC_OUTFILES} ${lgckb_MOC_HEADERS} ${lgckb_RC_SRCS})
target_link_libraries(lgck-builder Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Network Qt5::Gui ${OPENGL_LIBRARIES} z SDL2 ${SDL2mixer_LIBRARIES} lua5.2)
