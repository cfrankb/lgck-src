1- Creating the makefile

There are three runtime options: sdl sfml and glut

  LINUX:
    ./setup.sh sdl --res
    ./setup.sh sfml --res
    ./setup.sh glut --res

  WINDOWS:
    ./setup sdl --res
    ./setup sfml --res
    ./setup glut --res
  
2- Building the runtime
    make clean
    make

3- Running the application (linux)
    build/lgck-runtime-sdl <<path_of_lgckdb>>
    build/lgck-runtime-sfml <<path_of_lgckdb>>
    build/lgck-runtime-glut <<path_of_lgckdb>>

    Running on windows
    build\lgck-runtime-sdl <<path_of_lgckdb>>
    build\lgck-runtime-sfml <<path_of_lgckdb>>
    build\lgck-runtime-glut <<path_of_lgckdb>>
    
