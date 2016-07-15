mkdir target\sdl\sdl
mkdir target\sdl\shared
mkdir target\sdl\shared\implementers\sdl
mkdir target\sdl\shared\inputs\sdl
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall sdl/main.cpp -o target/sdl/sdl/main.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Map.cpp -o target/sdl/shared/Map.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/LevelEntry.cpp -o target/sdl/shared/LevelEntry.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Font.cpp -o target/sdl/shared/Font.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Frame.cpp -o target/sdl/shared/Frame.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FrameSet.cpp -o target/sdl/shared/FrameSet.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/SndArray.cpp -o target/sdl/shared/SndArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Attacker.cpp -o target/sdl/shared/Attacker.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Scene.cpp -o target/sdl/shared/Scene.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Display.cpp -o target/sdl/shared/Display.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Settings.cpp -o target/sdl/shared/Settings.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FrameArray.cpp -o target/sdl/shared/FrameArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Inventory.cpp -o target/sdl/shared/Inventory.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/ProtoArray.cpp -o target/sdl/shared/ProtoArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/helper.cpp -o target/sdl/shared/helper.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/DotArray.cpp -o target/sdl/shared/DotArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Selection.cpp -o target/sdl/shared/Selection.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/LuaVM.cpp -o target/sdl/shared/LuaVM.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/DrawList.cpp -o target/sdl/shared/DrawList.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/GameFile.cpp -o target/sdl/shared/GameFile.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Path.cpp -o target/sdl/shared/Path.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Snapshot.cpp -o target/sdl/shared/Snapshot.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/StringTable.cpp -o target/sdl/shared/StringTable.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Const.cpp -o target/sdl/shared/Const.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Drone.cpp -o target/sdl/shared/Drone.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Monster.cpp -o target/sdl/shared/Monster.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/inventoryTable.cpp -o target/sdl/shared/inventoryTable.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Game.cpp -o target/sdl/shared/Game.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Actor.cpp -o target/sdl/shared/Actor.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Object.cpp -o target/sdl/shared/Object.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FileMem.cpp -o target/sdl/shared/FileMem.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/GameLua.cpp -o target/sdl/shared/GameLua.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/PathBlock.cpp -o target/sdl/shared/PathBlock.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/GameEvents.cpp -o target/sdl/shared/GameEvents.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Folders.cpp -o target/sdl/shared/Folders.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Level.cpp -o target/sdl/shared/Level.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Animation.cpp -o target/sdl/shared/Animation.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Layer.cpp -o target/sdl/shared/Layer.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/DisplayManager.cpp -o target/sdl/shared/DisplayManager.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FileWrap.cpp -o target/sdl/shared/FileWrap.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/PngMagic.cpp -o target/sdl/shared/PngMagic.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Tasks.cpp -o target/sdl/shared/Tasks.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sdl/sn_sdl.cpp -o target/sdl/shared/implementers/sdl/sn_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sdl/im_sdl.cpp -o target/sdl/shared/implementers/sdl/im_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sdl/gr_sdl.cpp -o target/sdl/shared/implementers/sdl/gr_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sdl/mu_sdl.cpp -o target/sdl/shared/implementers/sdl/mu_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/inputs/sdl/kt_sdl.cpp -o target/sdl/shared/inputs/sdl/kt_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall shared/lgck_res.cpp -o target/sdl/shared/lgck_res.o
@if %errorlevel% neq 0 goto err
g++ -std=gnu++11 -O3 -Wall  target/sdl/sdl/main.o target/sdl/shared/Map.o target/sdl/shared/LevelEntry.o target/sdl/shared/Font.o target/sdl/shared/Frame.o target/sdl/shared/FrameSet.o target/sdl/shared/SndArray.o target/sdl/shared/Attacker.o target/sdl/shared/Scene.o target/sdl/shared/Display.o target/sdl/shared/Settings.o target/sdl/shared/FrameArray.o target/sdl/shared/Inventory.o target/sdl/shared/ProtoArray.o target/sdl/shared/helper.o target/sdl/shared/DotArray.o target/sdl/shared/Selection.o target/sdl/shared/LuaVM.o target/sdl/shared/DrawList.o target/sdl/shared/GameFile.o target/sdl/shared/Path.o target/sdl/shared/Snapshot.o target/sdl/shared/StringTable.o target/sdl/shared/Const.o target/sdl/shared/Drone.o target/sdl/shared/Monster.o target/sdl/shared/inventoryTable.o target/sdl/shared/Game.o target/sdl/shared/Actor.o target/sdl/shared/Object.o target/sdl/shared/FileMem.o target/sdl/shared/GameLua.o target/sdl/shared/PathBlock.o target/sdl/shared/GameEvents.o target/sdl/shared/Folders.o target/sdl/shared/Level.o target/sdl/shared/Animation.o target/sdl/shared/Layer.o target/sdl/shared/DisplayManager.o target/sdl/shared/FileWrap.o target/sdl/shared/PngMagic.o target/sdl/shared/Tasks.o target/sdl/shared/implementers/sdl/sn_sdl.o target/sdl/shared/implementers/sdl/im_sdl.o target/sdl/shared/implementers/sdl/gr_sdl.o target/sdl/shared/implementers/sdl/mu_sdl.o target/sdl/shared/inputs/sdl/kt_sdl.o target/sdl/shared/lgck_res.o -L../../redist/lib -lz -lm -llua52 -lftgl -lSDL2 -lSDL2_mixer -lopengl32 -lglu32 -lfreeglut -o sdl/lgck-runtime
@if %errorlevel% neq 0 goto err
goto out
:err
@echo fatal error
:out
