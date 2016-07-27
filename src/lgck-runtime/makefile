TARGET=sfml/lgck-runtime
CC=g++
ODIR=target/sfml
CFLAGS=-I. -I../shared -I/usr/include -I/usr/include/freetype2 -m64 -std=c++0x -O3 -fPIE -Wall 
OBJ=$(ODIR)/sfml/main.o $(ODIR)/shared/Map.o $(ODIR)/shared/LevelEntry.o $(ODIR)/shared/Font.o $(ODIR)/shared/Frame.o $(ODIR)/shared/FrameSet.o $(ODIR)/shared/SndArray.o $(ODIR)/shared/Attacker.o $(ODIR)/shared/Scene.o $(ODIR)/shared/Display.o $(ODIR)/shared/Settings.o $(ODIR)/shared/FrameArray.o $(ODIR)/shared/Inventory.o $(ODIR)/shared/ProtoArray.o $(ODIR)/shared/helper.o $(ODIR)/shared/DotArray.o $(ODIR)/shared/Selection.o $(ODIR)/shared/LuaVM.o $(ODIR)/shared/DrawList.o $(ODIR)/shared/GameFile.o $(ODIR)/shared/Path.o $(ODIR)/shared/Snapshot.o $(ODIR)/shared/StringTable.o $(ODIR)/shared/Const.o $(ODIR)/shared/Drone.o $(ODIR)/shared/Monster.o $(ODIR)/shared/inventoryTable.o $(ODIR)/shared/Game.o $(ODIR)/shared/Actor.o $(ODIR)/shared/Object.o $(ODIR)/shared/FileMem.o $(ODIR)/shared/GameLua.o $(ODIR)/shared/PathBlock.o $(ODIR)/shared/GameEvents.o $(ODIR)/shared/Folders.o $(ODIR)/shared/Level.o $(ODIR)/shared/Animation.o $(ODIR)/shared/Layer.o $(ODIR)/shared/DisplayManager.o $(ODIR)/shared/FileWrap.o $(ODIR)/shared/PngMagic.o $(ODIR)/shared/Tasks.o $(ODIR)/shared/implementers/sdl/sn_sdl.o $(ODIR)/shared/implementers/sdl/im_sdl.o $(ODIR)/shared/implementers/sdl/gr_sdl.o $(ODIR)/shared/implementers/sdl/mu_sdl.o $(ODIR)/shared/implementers/sfml/gr_sfml.o $(ODIR)/shared/implementers/sfml/im_sfml.o $(ODIR)/shared/inputs/sfml/kt_sfml.o $(ODIR)/shared/lgck_res.o
LIBS=-lz -lm -lpthread -llua5.2 -lftgl -lSDL2 -lSDL2_mixer -lsfml-graphics -lsfml-window -lsfml-system -lX11

all: $(TARGET)

$(ODIR)/sfml/main.o: sfml/main.cpp ../shared/Game.cpp ../shared/FileWrap.cpp ../shared/Level.cpp ../shared/implementers/sfml/gr_sfml.cpp ../shared/implementers/sfml/im_sfml.cpp ../shared/implementers/sdl/mu_sdl.cpp ../shared/implementers/sdl/sn_sdl.cpp ../shared/inputs/sfml/kt_sfml.cpp ../shared/GameEvents.cpp ../shared/Snapshot.cpp ../shared/DisplayManager.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ sfml/main.cpp $(CFLAGS)

$(ODIR)/shared/Actor.o: ../shared/Actor.cpp ../shared/LevelEntry.cpp ../shared/Animation.cpp ../shared/Attacker.cpp ../shared/Drone.cpp ../shared/Frame.cpp ../shared/Game.cpp ../shared/Inventory.cpp ../shared/Map.cpp ../shared/Monster.cpp ../shared/Path.cpp ../shared/PathBlock.cpp ../shared/Scene.cpp ../shared/Tasks.cpp ../shared/Level.cpp ../shared/GameEvents.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Actor.cpp $(CFLAGS)

$(ODIR)/shared/Animation.o: ../shared/Animation.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Animation.cpp $(CFLAGS)

$(ODIR)/shared/Attacker.o: ../shared/Attacker.cpp ../shared/Monster.cpp ../shared/Game.cpp ../shared/Frame.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Attacker.cpp $(CFLAGS)

$(ODIR)/shared/Const.o: ../shared/Const.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Const.cpp $(CFLAGS)

$(ODIR)/shared/Display.o: ../shared/Display.cpp ../shared/FileWrap.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Display.cpp $(CFLAGS)

$(ODIR)/shared/DisplayManager.o: ../shared/DisplayManager.cpp ../shared/Display.cpp ../shared/Game.cpp ../shared/Frame.cpp ../shared/FileWrap.cpp ../shared/Font.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/DisplayManager.cpp $(CFLAGS)

$(ODIR)/shared/DotArray.o: ../shared/DotArray.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/DotArray.cpp $(CFLAGS)

$(ODIR)/shared/DrawList.o: ../shared/DrawList.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/DrawList.cpp $(CFLAGS)

$(ODIR)/shared/Drone.o: ../shared/Drone.cpp ../shared/Monster.cpp ../shared/Game.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Drone.cpp $(CFLAGS)

$(ODIR)/shared/FileMem.o: ../shared/FileMem.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/FileMem.cpp $(CFLAGS)

$(ODIR)/shared/FileWrap.o: ../shared/FileWrap.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/FileWrap.cpp $(CFLAGS)

$(ODIR)/shared/Folders.o: ../shared/Folders.cpp ../shared/FileWrap.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Folders.cpp $(CFLAGS)

$(ODIR)/shared/Font.o: ../shared/Font.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Font.cpp $(CFLAGS)

$(ODIR)/shared/Frame.o: ../shared/Frame.cpp ../shared/FrameSet.cpp ../shared/DotArray.cpp ../shared/FileWrap.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Frame.cpp $(CFLAGS)

$(ODIR)/shared/FrameArray.o: ../shared/FrameArray.cpp ../shared/FrameSet.cpp ../shared/LevelEntry.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/FrameArray.cpp $(CFLAGS)

$(ODIR)/shared/FrameSet.o: ../shared/FrameSet.cpp ../shared/Frame.cpp ../shared/PngMagic.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/FrameSet.cpp $(CFLAGS)

$(ODIR)/shared/Game.o: ../shared/Game.cpp ../shared/GameFile.cpp ../shared/LuaVM.cpp ../shared/FileWrap.cpp ../shared/Folders.cpp ../shared/Inventory.cpp ../shared/inventoryTable.cpp ../shared/Map.cpp ../shared/Scene.cpp ../shared/Selection.cpp ../shared/Frame.cpp ../shared/Level.cpp ../shared/Layer.cpp ../shared/Display.cpp ../shared/Tasks.cpp ../shared/GameEvents.cpp ../shared/Const.cpp ../shared/helper.cpp ../shared/Snapshot.cpp ../shared/Font.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Game.cpp $(CFLAGS)

$(ODIR)/shared/GameEvents.o: ../shared/GameEvents.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/GameEvents.cpp $(CFLAGS)

$(ODIR)/shared/GameFile.o: ../shared/GameFile.cpp ../shared/FrameArray.cpp ../shared/ProtoArray.cpp ../shared/SndArray.cpp ../shared/Settings.cpp ../shared/FileWrap.cpp ../shared/Folders.cpp ../shared/Selection.cpp ../shared/Frame.cpp ../shared/StringTable.cpp ../shared/PathBlock.cpp ../shared/Level.cpp ../shared/Tasks.cpp ../shared/Layer.cpp ../shared/GameEvents.cpp ../shared/helper.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/GameFile.cpp $(CFLAGS)

$(ODIR)/shared/GameLua.o: ../shared/GameLua.cpp ../shared/Game.cpp ../shared/FileWrap.cpp ../shared/Folders.cpp ../shared/Inventory.cpp ../shared/Map.cpp ../shared/Scene.cpp ../shared/Frame.cpp ../shared/StringTable.cpp ../shared/Level.cpp ../shared/Layer.cpp ../shared/GameEvents.cpp ../shared/Display.cpp ../shared/Snapshot.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/GameLua.cpp $(CFLAGS)

$(ODIR)/shared/Inventory.o: ../shared/Inventory.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Inventory.cpp $(CFLAGS)

$(ODIR)/shared/Layer.o: ../shared/Layer.cpp ../shared/LevelEntry.cpp ../shared/Selection.cpp ../shared/Level.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Layer.cpp $(CFLAGS)

$(ODIR)/shared/Level.o: ../shared/Level.cpp ../shared/Settings.cpp ../shared/LevelEntry.cpp ../shared/Layer.cpp ../shared/FileWrap.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Level.cpp $(CFLAGS)

$(ODIR)/shared/LevelEntry.o: ../shared/LevelEntry.cpp ../shared/Level.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/LevelEntry.cpp $(CFLAGS)

$(ODIR)/shared/LuaVM.o: ../shared/LuaVM.cpp ../shared/FileWrap.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/LuaVM.cpp $(CFLAGS)

$(ODIR)/shared/Map.o: ../shared/Map.cpp ../shared/Frame.cpp ../shared/FileWrap.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Map.cpp $(CFLAGS)

$(ODIR)/shared/Monster.o: ../shared/Monster.cpp ../shared/Actor.cpp ../shared/Game.cpp ../shared/Map.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Monster.cpp $(CFLAGS)

$(ODIR)/shared/Object.o: ../shared/Object.cpp ../shared/ProtoArray.cpp ../shared/Path.cpp ../shared/Animation.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Object.cpp $(CFLAGS)

$(ODIR)/shared/Path.o: ../shared/Path.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Path.cpp $(CFLAGS)

$(ODIR)/shared/PathBlock.o: ../shared/PathBlock.cpp ../shared/Path.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/PathBlock.cpp $(CFLAGS)

$(ODIR)/shared/PngMagic.o: ../shared/PngMagic.cpp ../shared/FileWrap.cpp ../shared/Frame.cpp ../shared/FrameSet.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/PngMagic.cpp $(CFLAGS)

$(ODIR)/shared/ProtoArray.o: ../shared/ProtoArray.cpp ../shared/LevelEntry.cpp ../shared/Object.cpp ../shared/Game.cpp ../shared/FileWrap.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/ProtoArray.cpp $(CFLAGS)

$(ODIR)/shared/Scene.o: ../shared/Scene.cpp ../shared/Actor.cpp ../shared/Game.cpp ../shared/Map.cpp ../shared/Frame.cpp ../shared/Layer.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Scene.cpp $(CFLAGS)

$(ODIR)/shared/Selection.o: ../shared/Selection.cpp ../shared/LevelEntry.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Selection.cpp $(CFLAGS)

$(ODIR)/shared/Settings.o: ../shared/Settings.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Settings.cpp $(CFLAGS)

$(ODIR)/shared/Snapshot.o: ../shared/Snapshot.cpp ../shared/Scene.cpp ../shared/Game.cpp ../shared/Level.cpp ../shared/Map.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Snapshot.cpp $(CFLAGS)

$(ODIR)/shared/SndArray.o: ../shared/SndArray.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/SndArray.cpp $(CFLAGS)

$(ODIR)/shared/StringTable.o: ../shared/StringTable.cpp ../shared/LevelEntry.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/StringTable.cpp $(CFLAGS)

$(ODIR)/shared/Tasks.o: ../shared/Tasks.cpp ../shared/Actor.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/Tasks.cpp $(CFLAGS)

$(ODIR)/shared/helper.o: ../shared/helper.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/helper.cpp $(CFLAGS)

$(ODIR)/shared/implementers/sdl/gr_sdl.o: ../shared/implementers/sdl/gr_sdl.cpp ../shared/Frame.cpp ../shared/FrameSet.cpp ../shared/implementers/sdl/im_sdl.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/implementers/sdl/gr_sdl.cpp $(CFLAGS)

$(ODIR)/shared/implementers/sdl/im_sdl.o: ../shared/implementers/sdl/im_sdl.cpp ../shared/Frame.cpp ../shared/FrameSet.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/implementers/sdl/im_sdl.cpp $(CFLAGS)

$(ODIR)/shared/implementers/sdl/mu_sdl.o: ../shared/implementers/sdl/mu_sdl.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/implementers/sdl/mu_sdl.cpp $(CFLAGS)

$(ODIR)/shared/implementers/sdl/sn_sdl.o: ../shared/implementers/sdl/sn_sdl.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/implementers/sdl/sn_sdl.cpp $(CFLAGS)

$(ODIR)/shared/implementers/sfml/gr_sfml.o: ../shared/implementers/sfml/gr_sfml.cpp ../shared/Frame.cpp ../shared/FrameSet.cpp ../shared/implementers/sfml/im_sfml.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/implementers/sfml/gr_sfml.cpp $(CFLAGS)

$(ODIR)/shared/implementers/sfml/im_sfml.o: ../shared/implementers/sfml/im_sfml.cpp ../shared/Frame.cpp ../shared/FrameSet.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/implementers/sfml/im_sfml.cpp $(CFLAGS)

$(ODIR)/shared/inputs/sfml/kt_sfml.o: ../shared/inputs/sfml/kt_sfml.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/inputs/sfml/kt_sfml.cpp $(CFLAGS)

$(ODIR)/shared/inventoryTable.o: ../shared/inventoryTable.cpp ../shared/Inventory.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ ../shared/inventoryTable.cpp $(CFLAGS)

$(ODIR)/shared/lgck_res.o: shared/lgck_res.cpp
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -c -o $@ shared/lgck_res.cpp $(CFLAGS)

$(TARGET): $(OBJ)
	mkdir -p $(ODIR)/sfml $(ODIR)/shared $(ODIR)/shared/implementers/sdl $(ODIR)/shared/implementers/sfml $(ODIR)/shared/inputs/sfml
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -rf $(ODIR)