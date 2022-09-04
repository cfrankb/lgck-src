CXX=g++
CXXFLAGS=-g -O3 -DQT_NOT_WANTED
LIBS=-lm -lz 
main: main.o
		$(CXX) $(CXXFLAGS) $(LIBS) main.cpp ../shared/FileWrap.cpp ../shared/Frame.cpp ../shared/FrameSet.cpp ../shared/PngMagic.cpp ../shared/DotArray.cpp -o obl5t 
main.o:	main.cpp makefile ../shared/FileWrap.cpp ../shared/Frame.cpp ../shared/FrameSet.cpp ../shared/PngMagic.cpp ../shared/DotArray.cpp
