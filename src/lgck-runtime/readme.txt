1- Creating the makefile

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

3- Running the application
    build/lgck-runtime-sdl <<path_of_lgckdb>>
    build/lgck-runtime-sfml <<path_of_lgckdb>>
    build/lgck-runtime-glut <<path_of_lgckdb>>

