#!/usr/bin/python
import glob
import ntpath
import os

EXIT_SUCCESS = 0
EXIT_FAILURE =1

def prepare_deps(deps, fname):
    lines = []
    basename = ntpath.basename(fname)
    name = basename.replace('.cpp', '')
    fname_h =fname.replace('.cpp', '.h')
    ref = fname
    if os.path.isfile(fname_h):
        ref += f' {fname_h}'
    lines.append(f'$(BPATH)/{name}$(EXT): {ref}')
    lines.append(f'\t$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@')
    deps.append(f'{name}')
    return '\n'.join(lines)


def get_deps_blocks():
    paths = [
        "sdl/main.cpp",
        "../shared/*.cpp",
        "../shared/implementers/sdl/*.cpp",
        "../shared/interfaces/*.cpp",
        "../shared/inputs/*.cpp",
        "../shared/inputs/sdl/*.cpp",
        "build/lgck_res.cpp",
    ]
    deps_blocks = ["all: $(TARGET)"]
    deps = []

    for pattern in paths:
        files = [f for f in glob.glob(pattern)] if '*' in pattern else [pattern]
        for f in files:
            deps_blocks.append(prepare_deps(deps, f))

    objs = ' '.join(f'$(BPATH)/{x}$(EXT)' for x in deps)
    lines = []
    lines.append(f'$(TARGET): $(DEPS)')
    lines.append(f'\t$(CXX) $(CXXFLAGS) $(DEPS) $(LIBS) $(PARGS) -o $@')
    deps_blocks.append('\n'.join(lines))
    lines = []
    lines.append('clean:')
    lines.append("\trm -f $(BPATH)/*")
    deps_blocks.append('\n'.join(lines))
    return deps_blocks, objs

def main():
    vars = [
        'CXX=em++',
        'INC=-I../shared',
        'LIBS=-L../shared/extern/lua5.4 -llua',
        'CXXFLAGS=-sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sUSE_ZLIB=1 -DEMS',
        'PARGS=--preload-file data --emrun -DWASM -O2 -s WASM=1 -sASSERTIONS -sALLOW_MEMORY_GROWTH',
        'BPATH=build/emsdl', 'BNAME=lgck.html', 'TARGET=$(BPATH)/$(BNAME)'
        ]
    print("type `emmake make` to generare binary.")
    ext = '.o'

    deps_blocks, objs = get_deps_blocks()
    vars.append(f'DEPS={objs}')
    vars.append(f'EXT={ext}')
    with open('makefile', 'w') as tfile:
        tfile.write('\n'.join(vars) + "\n\n")
        tfile.write('\n\n'.join(deps_blocks))

    return EXIT_SUCCESS

exit(main())