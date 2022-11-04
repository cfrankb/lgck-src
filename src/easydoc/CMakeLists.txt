cmake_minimum_required(VERSION 3.10)
project(easydoc)
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
    Class.cpp
    Classes.cpp
    Database.cpp
    DlgAbout.cpp
    DlgClass.cpp
    DlgFunction.cpp
    DlgParam.cpp
    DlgSection.cpp
    DlgSource.cpp
    DlgStep.cpp
    FileWrap.cpp
    Functions.cpp
    Params.cpp
    Sections.cpp
    helper.cpp
    main.cpp
    mainwindow.cpp
    pagecond.cpp
    tabwidget.cpp
    testcase.cpp
)

set(lgckb_ui_files 
    DlgAbout.ui
    DlgClass.ui
    DlgFunction.ui
    DlgParam.ui
    DlgSection.ui
    DlgSource.ui
    DlgStep.ui
    mainwindow.ui
    pagecond.ui
    tabwidget.ui
)

set(lgckb_MOC_HEADERS 
    DlgAbout.h
    DlgClass.h
    DlgFunction.h
    DlgParam.h
    DlgSection.h
    DlgSource.h
    DlgStep.h
    mainwindow.h
    pagecond.h
    tabwidget.h
)

set(lgckb_RCS 
    easydoc.qrc
)
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Widgets)


include_directories(${QT_INCLUDES})
include_directories(./)
include_directories(../shared)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DUSE_QFILE=1)
add_definitions(-DLGCK_QT=1)

QT5_WRAP_UI(lgckb_SRCS ${lgckb_ui_files})
QT5_ADD_RESOURCES(lgckb_RC_SRCS ${lgckb_RCS})
add_executable(easydoc ${lgckb_SRCS} ${lgckb_MOC_OUTFILES} ${lgckb_MOC_HEADERS} ${lgckb_RC_SRCS})
target_link_libraries(easydoc Qt5::Core Qt5::Widgets Qt5::Gui z lua5.2)