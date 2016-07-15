#-------------------------------------------------
#
# Project created by QtCreator 2014-06-24T14:37:26
#
#-------------------------------------------------

QT       += core gui

win32:INCLUDEPATH   += ../../redist/headers
win32:DEFINES       += MAKE_WIN32
win32:LIBS          += -L../../redist/lib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = qci-test
TEMPLATE = app

SOURCES  += main.cpp\
            mainwindow.cpp \
            DlgSource.cpp \
            WEdit.cpp \
            Lexer.cpp

HEADERS  += mainwindow.h \
            DlgSource.h \
            WEdit.h \
            Lexer.h

FORMS    += mainwindow.ui \
            DlgSource.ui

LIBS     += -lqscintilla2

