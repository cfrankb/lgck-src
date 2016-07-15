INCLUDEPATH += ../shared

DESTDIR = ../../bin
MOC_DIR = ../../build/obl5t/moc
RCC_DIR = ../../build/obl5t/rcc
UI_DIR = ../../build/obl5t/ui
unix:OBJECTS_DIR = ../../build/obl5t/o/unix
win32:OBJECTS_DIR = ../../build/obl5t/o/win32
macx:OBJECTS_DIR = ../../build/obl5t/o/mac

CONFIG            += console
QMAKE_CXXFLAGS      += -std=c++0x -O3
TARGET = obl5t
LIBS += -lz

SOURCES += main.cpp ../shared/FileWrap.cpp ../shared/Frame.cpp ../shared/FrameSet.cpp ../shared/PngMagic.cpp ../shared/DotArray.cpp
