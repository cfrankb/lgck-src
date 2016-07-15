#!/usr/bin/python
from datetime import date
import json
import os

def write_GPL(tfile, start, end):
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

def makesafe(code):
    return code.strip().replace('"', '\\"').replace('\n', '\\n')

def write_file(tfile):
    write_GPL(tfile, '/*', '*/')
    tfile.write('''
#include "stdafx.h"
#include "WizScript.h"

WIZACTION CWizScript::m_actions[]=
{\n''')
    with open('wizscript.json') as sfile:
        raw = sfile.read()
    data = json.loads(raw)
    for k,v in sorted(data.items()):
        name = k
        print name
        t = v['type']
        fname = v['fname']
        descr = v['descr']
        sampleName = 'wizscript/%s.sample.lua' % fname
        if not os.path.isfile(sampleName):
            print 'creating: %s' % sampleName
            tfileTmp = open(sampleName, 'a')
            tfileTmp.close()
        sfile = open(sampleName)
        sample = sfile.read().strip()
        sfile.close()
        code = ''
        loader = ''
        script_file = 'wizscript/%s.lua' % fname
        if os.path.isfile(script_file):
            with open(script_file) as sfile:
                loader = sfile.read().strip()
        else:
            print('   not defined: %s' % ('wizscript/%s.lua' % fname))
        tfile.write('    {"%s", "%s", "%s", "%s", "%s", "%s", "%s"},\n' % (name, fname, makesafe(descr), t, makesafe(sample), makesafe(code), makesafe(loader)))
        
    tfile.write('    { NULL, NULL, NULL, NULL, NULL, NULL, NULL }\n')
    tfile.write('};\n')

def main():
    with open('../lgck-builder/wizhead.cpp', 'w') as tfile:
        write_file(tfile)

main()
