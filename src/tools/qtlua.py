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

def outputGPL(tfile, start, end):
    tfile.write('''%s
    LGCK Builder Runtime
    Copyright (C) 1999, %d  Francois Blanchette

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
%s\n''' % (start, date.today().year, end))

def parseLua():
    edoc = parseEDOC()
    #print edoc['class']
    tfile = open('functions1.lua', 'w')
    with open("functions.lua") as infile:
        for line in infile:
            if '= {};' in line and 'local' not in line and line[0] != ' ':
                i = line.find(' ')
                if i != -1:
                    name = line[0:i].strip()
                    if edoc['class'].has_key(name):
                        tfile.write('--[[ //////////////////////////////////////////////////////////////\n' + edoc['class'][name] + '\n////////////////////////////////////////////////////////////// --]]\n\n')
            elif line[:9]=='function ':
                t = line[9:]
                i = t.find('(')
                if i != -1:
                    name = t[0:i].strip()
                    print name
                    if edoc['function'].has_key(name):
                        tfile.write('--[[ //////////////////////////////////////////////////////////////\n' + edoc['function'][name] + '\n////////////////////////////////////////////////////////////// --]]\n\n')
            tfile.write(line)
    tfile.close()

def parseCpp(sname, tname):
    tfile = open(tname, 'w')
    outputGPL(tfile , '/*', '*/')

    tfile.write("""
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
    srt = alias.items()
    srt.sort()
    for k,v in srt:
        tfile.write('    alias ( "%s", %s )\n' % (k,v))

    tfile.write('    { NULL, 0 }\n')
    tfile.write('};\n')
    tfile.close()

def wrapstr(s):
    return '\n'.join(['\n'.join(wrap(line)) for line in s.splitlines()])

def parseEDOC():
    with open("GameLua.edoc") as infile:
        fct = {'function':{}, 'class':{}}
        key = ""
        body = ""
        ntype = ''
        ctype = ''
        for line in infile:
            if line[:11]=='@@function ':
                ntype = 'function'
            elif line[:8]=='@@class ':
                ntype = 'class'
            else:
                ntype = ''
            if ntype:
                if key:
                    #print key
                    fct[ctype][key]=wrapstr(body.strip())
                ctype = ntype
                body = line
                if ntype=='function':
                    line = line[11:]
                else:
                    line = line[8:]
                i = line.find(' ')
                if i != -1:
                    line = line[0:i]
                i = line.find(' ')
                if i != -1:
                   line = line[0:i]
                key = line.strip()                
            else:
                if '//////////////////////////////' not in line and '*/' not in line:
                    body += line                    
        if key:
            fct[ctype][key]=wrapstr(body.strip())
    return fct

def main():
    parser = argparse.ArgumentParser(description='EDOC helper for LGCK Builder')
    parser.add_argument('-o', '--output', required=False, default='GameLua.h')
    parser.add_argument('file', help='The cpp file to parse (GameLua.cpp)')
    args = parser.parse_args()
    parseCpp(args.file, args.output)

main()
