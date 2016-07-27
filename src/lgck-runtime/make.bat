mkdir target\sfml\sfml
mkdir target\sfml\shared
mkdir target\sfml\shared\implementers\sdl
mkdir target\sfml\shared\implementers\sfml
mkdir target\sfml\shared\inputs\sfml
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall sfml/main.cpp -o target/sfml/sfml/main.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Map.cpp -o target/sfml/shared/Map.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/LevelEntry.cpp -o target/sfml/shared/LevelEntry.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Font.cpp -o target/sfml/shared/Font.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Frame.cpp -o target/sfml/shared/Frame.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FrameSet.cpp -o target/sfml/shared/FrameSet.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/SndArray.cpp -o target/sfml/shared/SndArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Attacker.cpp -o target/sfml/shared/Attacker.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Scene.cpp -o target/sfml/shared/Scene.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Display.cpp -o target/sfml/shared/Display.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Settings.cpp -o target/sfml/shared/Settings.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FrameArray.cpp -o target/sfml/shared/FrameArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Inventory.cpp -o target/sfml/shared/Inventory.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/ProtoArray.cpp -o target/sfml/shared/ProtoArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/helper.cpp -o target/sfml/shared/helper.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/DotArray.cpp -o target/sfml/shared/DotArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Selection.cpp -o target/sfml/shared/Selection.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/LuaVM.cpp -o target/sfml/shared/LuaVM.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/DrawList.cpp -o target/sfml/shared/DrawList.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/GameFile.cpp -o target/sfml/shared/GameFile.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Path.cpp -o target/sfml/shared/Path.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Snapshot.cpp -o target/sfml/shared/Snapshot.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/StringTable.cpp -o target/sfml/shared/StringTable.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Const.cpp -o target/sfml/shared/Const.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Drone.cpp -o target/sfml/shared/Drone.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Monster.cpp -o target/sfml/shared/Monster.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/inventoryTable.cpp -o target/sfml/shared/inventoryTable.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Game.cpp -o target/sfml/shared/Game.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Actor.cpp -o target/sfml/shared/Actor.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Object.cpp -o target/sfml/shared/Object.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FileMem.cpp -o target/sfml/shared/FileMem.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/GameLua.cpp -o target/sfml/shared/GameLua.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/PathBlock.cpp -o target/sfml/shared/PathBlock.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/GameEvents.cpp -o target/sfml/shared/GameEvents.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Folders.cpp -o target/sfml/shared/Folders.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Level.cpp -o target/sfml/shared/Level.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Animation.cpp -o target/sfml/shared/Animation.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Layer.cpp -o target/sfml/shared/Layer.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/DisplayManager.cpp -o target/sfml/shared/DisplayManager.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FileWrap.cpp -o target/sfml/shared/FileWrap.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/PngMagic.cpp -o target/sfml/shared/PngMagic.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Tasks.cpp -o target/sfml/shared/Tasks.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sdl/sn_sdl.cpp -o target/sfml/shared/implementers/sdl/sn_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sdl/im_sdl.cpp -o target/sfml/shared/implementers/sdl/im_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sdl/gr_sdl.cpp -o target/sfml/shared/implementers/sdl/gr_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sdl/mu_sdl.cpp -o target/sfml/shared/implementers/sdl/mu_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sfml/gr_sfml.cpp -o target/sfml/shared/implementers/sfml/gr_sfml.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sfml/im_sfml.cpp -o target/sfml/shared/implementers/sfml/im_sfml.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/inputs/sfml/kt_sfml.cpp -o target/sfml/shared/inputs/sfml/kt_sfml.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall shared/lgck_res.cpp -o target/sfml/shared/lgck_res.o
@if %errorlevel% neq 0 goto err
g++ -std=gnu++11 -O3 -Wall  target/sfml/sfml/main.o target/sfml/shared/Map.o target/sfml/shared/LevelEntry.o target/sfml/shared/Font.o target/sfml/shared/Frame.o target/sfml/shared/FrameSet.o target/sfml/shared/SndArray.o target/sfml/shared/Attacker.o target/sfml/shared/Scene.o target/sfml/shared/Display.o target/sfml/shared/Settings.o target/sfml/shared/FrameArray.o target/sfml/shared/Inventory.o target/sfml/shared/ProtoArray.o target/sfml/shared/helper.o target/sfml/shared/DotArray.o target/sfml/shared/Selection.o target/sfml/shared/LuaVM.o target/sfml/shared/DrawList.o target/sfml/shared/GameFile.o target/sfml/shared/Path.o target/sfml/shared/Snapshot.o target/sfml/shared/StringTable.o target/sfml/shared/Const.o target/sfml/shared/Drone.o target/sfml/shared/Monster.o target/sfml/shared/inventoryTable.o target/sfml/shared/Game.o target/sfml/shared/Actor.o target/sfml/shared/Object.o target/sfml/shared/FileMem.o target/sfml/shared/GameLua.o target/sfml/shared/PathBlock.o target/sfml/shared/GameEvents.o target/sfml/shared/Folders.o target/sfml/shared/Level.o target/sfml/shared/Animation.o target/sfml/shared/Layer.o target/sfml/shared/DisplayManager.o target/sfml/shared/FileWrap.o target/sfml/shared/PngMagic.o target/sfml/shared/Tasks.o target/sfml/shared/implementers/sdl/sn_sdl.o target/sfml/shared/implementers/sdl/im_sdl.o target/sfml/shared/implementers/sdl/gr_sdl.o target/sfml/shared/implementers/sdl/mu_sdl.o target/sfml/shared/implementers/sfml/gr_sfml.o target/sfml/shared/implementers/sfml/im_sfml.o target/sfml/shared/inputs/sfml/kt_sfml.o target/sfml/shared/lgck_res.o -L../../redist/lib -lz -lm -llua52 -lftgl -lSDL2 -lSDL2_mixer -lopengl32 -lglu32 -lfreeglut -o sfml/lgck-runtime
@if %errorlevel% neq 0 goto err
goto out
:err
@echo fatal error
:out
