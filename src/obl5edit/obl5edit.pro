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
win32:INCLUDEPATH += ../../redist/headers
RESOURCES +=  obl5edit.qrc
win32:RC_FILE = obl5edit.rc
OTHER_FILES +=  TODO
DEFINES += USE_QFILE=1
win32:DEFINES += MAKE_WIN32
win32:LIBS += -L../../redist/lib
LIBS += -lz

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
    FrameView.cpp \
    Pixel.cpp \
    DlgGoto.cpp \
    WizFrameSet.cpp \
    WFrameSet.cpp \
    WizPageImport.cpp \
    DlgImageSize.cpp \
    WPreview.cpp \
    WFileSave.cpp \
    DlgShortcuts.cpp \
    WHotKey.cpp \
    DlgAlpha.cpp \
    ../shared/PngMagic.cpp

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
    FrameView.h \
    Pixel.h \
    DlgGoto.h \
    WizFrameSet.h \
    WFrameSet.h \
    WizPageImport.h \
    DlgImageSize.h \
    WPreview.h \
    WFileSave.h \
    DlgShortcuts.h \
    WHotKey.h \
    DlgAlpha.h \
    ../shared/PngMagic.h

FORMS    += mainwindow.ui \
    DlgAbout.ui \
    PixelBoxDock.ui \
    WPreviewDock.ui \
    DlgGoto.ui \
    WizFrameSet.ui \
    DlgImageSize.ui \
    DlgShortcuts.ui \
    DlgAlpha.ui
