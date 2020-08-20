'''
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
'''

from datetime import date
from textwrap import wrap
import argparse
from lgckutil.license import *

def parseCpp(sname, tname):
    tfile = open(tname, 'w')
    tfile.write(license_cpp)

    tfile.write("""
#include "LuaVM.h"

typedef struct  {
    const char *fnName;
    lua_CFunction fnAddr;
} FUNCTION;

#define fn( fnName ) { #fnName, fnName },
#define alias( aliasName, fnName ) { aliasName, fnName },
#define luaFn( __fn__ ) int __fn__ (lua_State *);\n\n""")
    
    with open(sname) as infile:
        fct = []
        alias = {}
        last_fct = ''
        for line in infile:
            if line[:4]=='int ' and 'lua_State' in line:
                t = line[4:]
                i = t.find('(')
                if i != -1:
                    name = t[0:i].strip()
                    fct.append(name)
                    last_fct = name
            elif line[:3]== '//#':
                t = line[3:].strip()
                alias[t]= last_fct
                
    fct = sorted(fct, key=str.lower)
    for f in fct:
        tfile.write('luaFn ( %s )\n' % f)
    tfile.write('\nstatic FUNCTION exports [] = {\n')
    for f in fct:
        tfile.write('    fn ( %s )\n' % f)
    srt = list(alias.items())
    srt.sort()
    for k,v in srt:
        tfile.write('    alias ( "%s", %s )\n' % (k,v))

    tfile.write('    { nullptr, 0 }\n')
    tfile.write('};\n')
    tfile.close()

def wrapstr(s):
    return '\n'.join(['\n'.join(wrap(line)) for line in s.splitlines()])

def main():
    parser = argparse.ArgumentParser(description='EDOC helper for LGCK Builder')
    parser.add_argument('-o', '--output', required=False, default='../shared/GameLua.h')
    parser.add_argument('file', help='The cpp file to parse (../shared/GameLua.cpp)')
    args = parser.parse_args()
    parseCpp(args.file, args.output)

main()
