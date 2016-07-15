INCLUDEPATH         += ../shared
unix:INCLUDEPATH    += /usr/include/freetype2
CONFIG            += console
QMAKE_CXXFLAGS      += -std=c++0x -O3
TARGET = lgck-runtime
LIBS += -lz 
unix:LIBS += -llua5.2 \
    -lftgl \
    -lSDL2_mixer-2.0 \
    -lSDL2-2.0 \
    -lz \
    -lglut

DESTDIR = ../../bin
MOC_DIR = ../../build/lgck-runtime/moc
RCC_DIR = ../../build/lgck-runtime/rcc
UI_DIR = ../../build/lgck-runtime/ui
unix:OBJECTS_DIR = ../../build/lgck-runtime/o/unix
win32:OBJECTS_DIR = ../../build/lgck-runtime/o/win32
macx:OBJECTS_DIR = ../../build/lgck-runtime/o/mac

SOURCES += main.cpp \
    lgck_res.cpp \
    ../shared/Map.cpp \
    ../shared/LevelEntry.cpp \
    ../shared/Frame.cpp \
    ../shared/FrameSet.cpp \
    ../shared/SndArray.cpp \
    ../shared/Attacker.cpp \
    ../shared/Scene.cpp \
    ../shared/Display.cpp \
    ../shared/Settings.cpp \
    ../shared/FrameArray.cpp \
    ../shared/Inventory.cpp \
    ../shared/ProtoArray.cpp \
    ../shared/helper.cpp \
    ../shared/DotArray.cpp \
    ../shared/Selection.cpp \
    ../shared/LuaVM.cpp \
    ../shared/DrawList.cpp \
    ../shared/GameFile.cpp \
    ../shared/Path.cpp \
    ../shared/StringTable.cpp \
    ../shared/Const.cpp \
    ../shared/Drone.cpp \
    ../shared/Monster.cpp \
    ../shared/inventoryTable.cpp \
    ../shared/Game.cpp \
    ../shared/Actor.cpp \
    ../shared/Object.cpp \
    ../shared/GameLua.cpp \
    ../shared/PathBlock.cpp \
    ../shared/GameEvents.cpp \
    ../shared/Folders.cpp \
    ../shared/Level.cpp \
    ../shared/Animation.cpp \
    ../shared/Layer.cpp \
    ../shared/FileWrap.cpp \
    ../shared/PngMagic.cpp \
    ../shared/Tasks.cpp \
    ../shared/implementers/sn_sdl.cpp \
    ../shared/implementers/dm_opengl.cpp \
    ../shared/implementers/gr_opengl.cpp \
    ../shared/implementers/mu_sdl.cpp \
    ../shared/implementers/im_opengl.cpp \
    ../shared/inputs/glut/kt_glut.cpp 

