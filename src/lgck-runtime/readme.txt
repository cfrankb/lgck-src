1- Creating the makefile

  LINUX:
    ./setup.sh sdl/ --res
    ./setup.sh sfml/ --res
    ./setup.sh glut/ --res

  WINDOWS:
    ./setup sdl/ --res
    ./setup sfml/ --res
    ./setup glut/ --res
  

2- Building the runtime
    make clean
    make

3- Running the application
    sdl/lgckbuilder <<path_of_lgckdb>>
    sfml/lgckbuilder <<path_of_lgckdb>>
    glut/lgckbuilder <<path_of_lgckdb>>    

