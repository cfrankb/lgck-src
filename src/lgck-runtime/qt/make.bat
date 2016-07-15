mkdir obj
mkdir obj\shared
mkdir obj\shared\implementers
mkdir obj\shared\inputs\glut
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall main.cpp -o obj/main.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Map.cpp -o obj/shared/Map.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/LevelEntry.cpp -o obj/shared/LevelEntry.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Frame.cpp -o obj/shared/Frame.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FrameSet.cpp -o obj/shared/FrameSet.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/SndArray.cpp -o obj/shared/SndArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Attacker.cpp -o obj/shared/Attacker.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Scene.cpp -o obj/shared/Scene.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Display.cpp -o obj/shared/Display.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Settings.cpp -o obj/shared/Settings.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FrameArray.cpp -o obj/shared/FrameArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Inventory.cpp -o obj/shared/Inventory.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/ProtoArray.cpp -o obj/shared/ProtoArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/helper.cpp -o obj/shared/helper.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/DotArray.cpp -o obj/shared/DotArray.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Selection.cpp -o obj/shared/Selection.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/LuaVM.cpp -o obj/shared/LuaVM.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/DrawList.cpp -o obj/shared/DrawList.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/GameFile.cpp -o obj/shared/GameFile.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Path.cpp -o obj/shared/Path.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Snapshot.cpp -o obj/shared/Snapshot.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/StringTable.cpp -o obj/shared/StringTable.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Const.cpp -o obj/shared/Const.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Drone.cpp -o obj/shared/Drone.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Monster.cpp -o obj/shared/Monster.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/inventoryTable.cpp -o obj/shared/inventoryTable.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Game.cpp -o obj/shared/Game.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Actor.cpp -o obj/shared/Actor.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Object.cpp -o obj/shared/Object.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FileMem.cpp -o obj/shared/FileMem.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/GameLua.cpp -o obj/shared/GameLua.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/PathBlock.cpp -o obj/shared/PathBlock.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/GameEvents.cpp -o obj/shared/GameEvents.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Folders.cpp -o obj/shared/Folders.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Level.cpp -o obj/shared/Level.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Animation.cpp -o obj/shared/Animation.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Layer.cpp -o obj/shared/Layer.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/FileWrap.cpp -o obj/shared/FileWrap.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/PngMagic.cpp -o obj/shared/PngMagic.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/Tasks.cpp -o obj/shared/Tasks.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/sn_sdl.cpp -o obj/shared/implementers/sn_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/dm_opengl.cpp -o obj/shared/implementers/dm_opengl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/gr_opengl.cpp -o obj/shared/implementers/gr_opengl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/mu_sdl.cpp -o obj/shared/implementers/mu_sdl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/implementers/im_opengl.cpp -o obj/shared/implementers/im_opengl.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall ../shared/inputs/glut/kt_glut.cpp -o obj/shared/inputs/glut/kt_glut.o
@if %errorlevel% neq 0 goto err
g++ -c -I../shared -I../../redist/headers -I../../redist/headers/freetype2 -std=gnu++11 -O3 -Wall lgck_res.cpp -o obj/lgck_res.o
@if %errorlevel% neq 0 goto err
g++ -std=gnu++11 -O3 -Wall  obj/main.o obj/shared/Map.o obj/shared/LevelEntry.o obj/shared/Frame.o obj/shared/FrameSet.o obj/shared/SndArray.o obj/shared/Attacker.o obj/shared/Scene.o obj/shared/Display.o obj/shared/Settings.o obj/shared/FrameArray.o obj/shared/Inventory.o obj/shared/ProtoArray.o obj/shared/helper.o obj/shared/DotArray.o obj/shared/Selection.o obj/shared/LuaVM.o obj/shared/DrawList.o obj/shared/GameFile.o obj/shared/Path.o obj/shared/Snapshot.o obj/shared/StringTable.o obj/shared/Const.o obj/shared/Drone.o obj/shared/Monster.o obj/shared/inventoryTable.o obj/shared/Game.o obj/shared/Actor.o obj/shared/Object.o obj/shared/FileMem.o obj/shared/GameLua.o obj/shared/PathBlock.o obj/shared/GameEvents.o obj/shared/Folders.o obj/shared/Level.o obj/shared/Animation.o obj/shared/Layer.o obj/shared/FileWrap.o obj/shared/PngMagic.o obj/shared/Tasks.o obj/shared/implementers/sn_sdl.o obj/shared/implementers/dm_opengl.o obj/shared/implementers/gr_opengl.o obj/shared/implementers/mu_sdl.o obj/shared/implementers/im_opengl.o obj/shared/inputs/glut/kt_glut.o obj/lgck_res.o -L../../redist/lib -lz -lm -llua52 -lftgl -lSDL2 -lSDL2_mixer -lopengl32 -lglu32 -lfreeglut -o lgck-runtime
@if %errorlevel% neq 0 goto err
goto out
:err
@echo fatal error
:out
