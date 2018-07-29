# -------------------------------------------------
# Project created by QtCreator 2010-02-17T21:49:55
# -------------------------------------------------
RESOURCES += easydoc.qrc
win32:CONFIG        += no_lflags_merge
INCLUDEPATH += ../shared
win32:RC_FILE = easydoc.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
win32:DEFINES += MAKE_WIN32
DEFINES += USE_QFILE MAKE_WIN32
QMAKE_CXXFLAGS_RELEASE += -std=c++0x -O3
QMAKE_CXXFLAGS_DEBUG += -std=c++0x -g3
QMAKE_LFLAGS_WINDOWS += -Wl,--dynamicbase -Wl,--nxcompat,--large-address-aware
win32:LIBS += -lmingw32

TARGET = easydoc
TEMPLATE = app
SOURCES += Class.cpp \
    Classes.cpp \
    Database.cpp \
    FileWrap.cpp \
    Functions.cpp \
    DlgAbout.cpp \
    DlgClass.cpp \
    DlgFunction.cpp \
    DlgParam.cpp \
    DlgSection.cpp \
    main.cpp \
    mainwindow.cpp \
    Params.cpp \
    Sections.cpp \
    tabwidget.cpp \ 
    DlgSource.cpp \
    helper.cpp \
    testcase.cpp \
    DlgStep.cpp \
    pagecond.cpp

HEADERS += Class.h \
    Classes.h \
    Database.h \
    DlgAbout.h \
    DlgClass.h \
    DlgFunction.h \
    DlgParam.h \
    DlgSection.h \
    FileWrap.h \
    Functions.h \
    mainwindow.h \
    Params.h \
    Sections.h \
    tabwidget.h \ 
    DlgSource.h \
    helper.h \
    const.h \
    testcase.h \
    DlgStep.h \
    pagecond.h

FORMS += mainwindow.ui \
    DlgAbout.ui \
    DlgClass.ui \
    DlgFunction.ui \
    DlgParam.ui \
    DlgSection.ui \
    tabwidget.ui \ 
    DlgSource.ui \
    DlgStep.ui \
    pagecond.ui
