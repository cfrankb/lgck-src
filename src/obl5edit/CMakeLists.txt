cmake_minimum_required(VERSION 3.10)
project(obl5edit)
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

set(CMAKE_LIBRARY_PATH 
    ${CMAKE_LIBRARY_PATH}
    /opt/local/lib
)

set(lgckb_SRCS 
    DlgAbout.cpp
    DlgAlpha.cpp
    DlgGoto.cpp
    DlgImageSize.cpp
    DlgShortcuts.cpp
    OBL5File.cpp
    Pixel.cpp
    PixelBoxDock.cpp
    WFileSave.cpp
    WFrameSet.cpp
    WHotKey.cpp
    WPalette.cpp
    WPreviewDock.cpp
    WizFrameSet.cpp
    WizPageImport.cpp
    framescroll.cpp
    framewidget.cpp
    main.cpp
    mainwindow.cpp
    previewscroll.cpp
    previewwidget.cpp
    ../shared/FileWrap.cpp
    ../shared/Frame.cpp
    ../shared/FrameSet.cpp
    ../shared/DotArray.cpp
    ../shared/qtgui/ColorPicker/colorpickerwidget.cpp
    ../shared/qtgui/ColorPicker/colorviewer.cpp
    ../shared/qtgui/ColorPicker/qtcolortriangle.cpp
    ../shared/qtgui/ColorPicker/screen.cpp
    ../shared/PngMagic.cpp
    ../shared/helper.cpp
    ../shared/LuaVM.cpp
)

set(lgckb_ui_files 
    DlgAbout.ui
    DlgAlpha.ui
    DlgGoto.ui
    DlgImageSize.ui
    DlgShortcuts.ui
    PixelBoxDock.ui
    WPreviewDock.ui
    WizFrameSet.ui
    mainwindow.ui
)

set(lgckb_MOC_HEADERS 
    DlgAbout.h
    DlgAlpha.h
    DlgGoto.h
    DlgImageSize.h
    DlgShortcuts.h
    PixelBoxDock.h
    WFileSave.h
    WFrameSet.h
    WHotKey.h
    WPalette.h
    WPreviewDock.h
    WizFrameSet.h
    WizPageImport.h
    framescroll.h
    framewidget.h
    mainwindow.h
    previewscroll.h
    previewwidget.h
    ../shared/qtgui/ColorPicker/colorpickerwidget.h
    ../shared/qtgui/ColorPicker/colorviewer.h
    ../shared/qtgui/ColorPicker/qtcolortriangle.h
    ../shared/qtgui/ColorPicker/screen.h
)

set(lgckb_RCS 
    obl5edit.qrc
)
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui OpenGL Widgets)
find_package(OpenGL REQUIRED)


include_directories(${QT_INCLUDES})
include_directories(../shared)
include_directories(../shared/qtgui/ColorPicker/)
include_directories(../shared/qtgui)
include_directories(./)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DUSE_QFILE=1)
add_definitions(-DLGCK_QT=1)

QT5_WRAP_UI(lgckb_SRCS ${lgckb_ui_files})
QT5_ADD_RESOURCES(lgckb_RC_SRCS ${lgckb_RCS})
add_executable(obl5edit ${lgckb_SRCS} ${lgckb_MOC_OUTFILES} ${lgckb_MOC_HEADERS} ${lgckb_RC_SRCS})
target_link_libraries(obl5edit Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Gui ${OPENGL_LIBRARIES} z lua5.2)