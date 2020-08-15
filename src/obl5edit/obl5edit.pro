#-------------------------------------------------
#
# Project created by QtCreator 2010-08-05T03:10:42
#
#-------------------------------------------------

win32:CONFIG += static
QT       += core gui opengl

TARGET = obl5edit
TEMPLATE = app
INCLUDEPATH += ../shared/qtgui/ColorPicker
INCLUDEPATH += ../shared
win32:INCLUDEPATH += ../../../redist/include
RESOURCES +=  obl5edit.qrc
win32:RC_FILE = obl5edit.rc
OTHER_FILES +=  TODO
DEFINES += USE_QFILE=1
win32:DEFINES += MAKE_WIN32
win32:LIBS += -L../../../redist/lib
LIBS += -lz -llua5.2
QMAKE_CXXFLAGS_RELEASE += -std=c++0x -O3
QMAKE_CXXFLAGS_DEBUG += -std=c++0x -g3
QMAKE_LFLAGS_WINDOWS += -Wl,--dynamicbase -Wl,--nxcompat

SOURCES += mainwindow.cpp \
    main.cpp\
    OBL5File.cpp \
    DlgAbout.cpp \
    ../shared/FileWrap.cpp \
    ../shared/Frame.cpp \
    ../shared/FrameSet.cpp \
    PixelBoxDock.cpp \
    WPalette.cpp \
    ../shared/DotArray.cpp \
    WPreviewDock.cpp \
    ../shared/qtgui/ColorPicker/screen.cpp \
    ../shared/qtgui/ColorPicker/qtcolortriangle.cpp \
    ../shared/qtgui/ColorPicker/colorviewer.cpp \
    ../shared/qtgui/ColorPicker/colorpickerwidget.cpp \
    Pixel.cpp \
    DlgGoto.cpp \
    WizFrameSet.cpp \
    WFrameSet.cpp \
    WizPageImport.cpp \
    DlgImageSize.cpp \
    WFileSave.cpp \
    DlgShortcuts.cpp \
    WHotKey.cpp \
    DlgAlpha.cpp \
    ../shared/PngMagic.cpp \
    framescroll.cpp \
    framewidget.cpp \
    previewwidget.cpp \
    previewscroll.cpp \
    ../shared/helper.cpp \
    ../shared/LuaVM.cpp

HEADERS  += mainwindow.h \
    OBL5File.h \
    DlgAbout.h \
    ../shared/FileWrap.h \
    ../shared/Frame.h \
    ../shared/FrameSet.h \
    PixelBoxDock.h \
    WPalette.h \
    ../shared/DotArray.h \
    ../shared/Tools.h \
    WPreviewDock.h \
    ../shared/qtgui/ColorPicker/screen.h \
    ../shared/qtgui/ColorPicker/qtcolortriangle.h \
    ../shared/qtgui/ColorPicker/colorviewer.h \
    ../shared/qtgui/ColorPicker/colorpickerwidget.h \
    Pixel.h \
    DlgGoto.h \
    WizFrameSet.h \
    WFrameSet.h \
    WizPageImport.h \
    DlgImageSize.h \
    WFileSave.h \
    DlgShortcuts.h \
    WHotKey.h \
    DlgAlpha.h \
    ../shared/PngMagic.h \
    framescroll.h \
    framewidget.h \
    previewwidget.h \
    previewscroll.h \
    ../shared/LuaVM.h

FORMS    += mainwindow.ui \
    DlgAbout.ui \
    PixelBoxDock.ui \
    WPreviewDock.ui \
    DlgGoto.ui \
    WizFrameSet.ui \
    DlgImageSize.ui \
    DlgShortcuts.ui \
    DlgAlpha.ui
