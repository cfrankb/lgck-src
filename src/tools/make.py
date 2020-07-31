#!/usr/bin/python
'''
    LGCK Builder Runtime
    Copyright (C) 1999, 2016  Francois Blanchette

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

# http://stackoverflow.com/questions/2568067/using-gcc-mingw-to-compile-opengl-on-windows

import json
import glob
import argparse
import os
import binascii
import subprocess
import platform

EXIT_SUCCESS = 0
EXIT_FAILURE = 1
def chunks(l, n):
    for i in range(0, len(l), n):
        yield l[i:i+n]

class CMakeU():
    def __init__(self):
        self.data = {}
        self.lines = []

    def scanfile(self,fpath, deps):
        print (fpath)
        sfile = open(fpath, 'r')
        for line in sfile:
            if line[:10] == '#include "':
                fname = line.split('"')[1]
                base = os.path.splitext(fname)[0]
                src = base + '.cpp'
                if not os.path.exists(src):
                    src = os.path.join(os.path.dirname(fpath), os.path.basename(src))
                if not os.path.exists(src):
                    if self.args.verbose:
                        print("warning: can't find {}".format(src))
                    continue
                if os.path.basename(base) not in self.data['exclude'] and src not in deps:
                    deps.append(src)
        sfile.close()

    def write_batchfile(self, build):
        objs = []
        objd = []
        src = self.get_source()
        cmds = []
        cmds.append('if %1.==link. goto link')
        cmds.append('if %1.==clean. goto clean')
        cmds.append('windres lgck.rc -O coff -o build\lgck.res')
        cflags = ' '.join(["-I{flag}".format(flag=flag) for flag in self.data[build]['paths']]) + \
            ' ' + ' '.join(["{flag}".format(flag=flag) for flag in self.data[build]['flags']])
        for fname in src:
            base = os.path.splitext(fname)[0]
            objn = base + '.o';
            if objn[:3] == '../':
                objn = objn[3:]
            objn = self.data['obj_dir'] + '/' + objn
            objp = os.path.dirname(objn)
            if not objp in objd:
                objd.append(objp)
            objs.append(objn)
            cmds.append(r'g++ -c %s %s -o %s' % (cflags, fname, objn))
            cmds.append(r'@if %errorlevel% neq 0 goto err')
        tfile = open(self.data[build]['output'], 'w')
        for objp in objd:
            tfile.write('mkdir %s\n' % objp.replace('/', '\\'))
        for cmd in cmds:
            tfile.write(cmd + '\n')
        cflags = ' '.join(["{flag}".format(flag=flag) for flag in self.data[build]['flags']]) + \
            ' ' + ' '.join(["-D{flag}".format(flag=flag) for flag in self.data['declare']]) 
        libs = ' '.join(['{lib}'.format(lib=lib) for lib in self.data[build]['libs']])
        objs = ' '.join([obj for obj in objs])
        tfile.write(':link\n')
        tfile.write('g++ {cflags} {objs} build\lgck.res {libs} -o {target}\n'.format(
            cflags=cflags,
            objs=objs,
            libs=libs,
            target=self.data['target']
        ))
        tfile.write(r'@if %errorlevel% neq 0 goto err')
        tfile.write('\n')
        tfile.write('goto out\n')
        tfile.write(':clean\n')
        for objp in objd:
            tfile.write('rmdir /s /q %s\n' % objp.replace('/', '\\'))
        tfile.write('goto out\n')
        tfile.write(':err\n')
        tfile.write('@echo fatal error\n')
        tfile.write(':out\n')
        tfile.close()

    def get_source(self):
        src = []
        for fname in self.data['sources']:
            if '*' in fname:
                for fname in glob.glob(fname):
                    src.append(fname)
            else:
                src.append(fname)
        return src

    def write_makefile(self):
        deps = []
        self.tdeps = {}
        objs = []
        src = self.get_source()
        if self.args.verbose:
            print(' \\\n'.join('{x}'.format(x=x) for x in src))
        objd = []
        for fname in src:
            deps = [fname]
            base = os.path.splitext(fname)[0]
            hname = base + '.h'
            if os.path.exists(hname):
                self.scanfile(hname,deps)
            if os.path.exists(fname):
                self.scanfile(fname,deps)
            else:
                print('*',fname)
                exit(EXIT_FAILURE)
            objn = base + '.o';
            if objn[:3] == '../':
                objn = objn[3:]
            objn = '$(ODIR)/' + objn
            objp = os.path.dirname(objn)
            if not objp in objd:
                objd.append(objp)
            self.tdeps[objn] = (deps,fname)
            objs.append(objn)
        cflags = ' '.join(["-I{flag}".format(flag=flag) for flag in self.data['linux']['paths']]) + \
            ' ' + ' '.join(["{flag}".format(flag=flag) for flag in self.data['linux']['flags']]) + \
            ' ' + ' '.join(["-D{flag}".format(flag=flag) for flag in self.data['declare']])
        tfile = open(self.data['linux']['output'], 'w')
        tfile.write("TARGET=%s\n" % self.data['target'])
        tfile.write('CC=%s\n' % self.data['cxx'])
        tfile.write('ODIR=%s\n' % self.data['obj_dir'])
        tfile.write('CFLAGS=%s\n' % cflags)
        tfile.write('OBJ=%s\n' % ' '.join([obj for obj in objs]))
        tfile.write('LIBS=%s\n'% ' '.join(['-l{lib}'.format(lib=lib) for lib in self.data['linux']['libs']]))
        tfile.write('\nall: $(TARGET)\n\n');
        objs = self.tdeps.items()
        objs = sorted(objs)
        for k, v in objs:
            tfile.write('%s: %s\n' % (k, ' '.join([fname for fname in v[0]])))
            tfile.write('\tmkdir -p ' + ' '.join([c for c in objd]) + '\n')
            tfile.write('\t$(CC) -c -o $@ %s $(CFLAGS)\n\n' % v[1]) # $<
        tfile.write('$(TARGET): $(OBJ)\n')
        tfile.write('\tmkdir -p ' + ' '.join([c for c in objd]) + '\n')
        tfile.write('\t$(CC) -o $@ $^ $(CFLAGS) $(LIBS)\n\n')
        tfile.write('clean:\n')
        tfile.write('\trm -rf $(ODIR)')
        tfile.close()

    def write_res(self):
        with open('res.json') as s:
            raw = s.read()
        res = json.loads(raw)
        print(res)
        CHUNK_SIZE = 32
        out_file = res['out']
        odir = os.path.dirname(out_file)
        print(odir)
        if not os.path.exists(odir):
            if platform.system()=='Windows':
                subprocess.check_call(["mkdir", odir.replace('/', '\\')])
            else:
                subprocess.check_call(["mkdir", "-p", odir])
        base = res['base']
        tfile = open(out_file, 'w')
        tfile.write('#include "FileWrap.h"\n\n');
        i = 0
        for fname in res['file_list']:
            sfile = open(base + fname, 'rb')
            data = sfile.read()
            sfile.close()
            tfile.write('const unsigned char file_%d[]={\n' % i)
            for line in chunks(binascii.b2a_hex(data), CHUNK_SIZE):
                tfile.write('    ' + ','.join(['0x'+ch.decode("utf-8") for ch in chunks(line, 2)]))
                if len(line) == CHUNK_SIZE:
                    tfile.write(',')
                tfile.write('\n')
            tfile.write('};\n\n')
            i = i + 1
        tfile.write('void {0}()\n{{\n'.format(res['name']));
        i = 0
        for fname in res['file_list']:
            tfile.write('    CFileWrap::addFile(":/%s", (const char*)file_%d, sizeof(file_%d));\n' % (fname, i, i))
            i = i + 1
        tfile.write('}\n')
        tfile.close()

    def main(self):
        parser = argparse.ArgumentParser(description='MakeFile utility for LGCK Builder')
        parser.add_argument('file', default='make.json', help='The json file to parse (make.json)')
        parser.add_argument('--res', dest='res', action='store_true', help= "rebuild res file")
        parser.add_argument('-v', dest='verbose', action='store_true', help= "verbose")
        self.args = parser.parse_args()
        try:
            f = open(self.args.file, 'r') 
            raw = f.read() 
            f.close()
        except:
            print("can't open {}".format(self.args.file))
            exit(-1)
        self.data = json.loads(raw)
        if self.args.res:
            self.write_res()
        if 'linux' in self.data:
            self.write_makefile()
        if 'win32' in self.data:
            self.write_batchfile('win32')
        if 'win32-s' in self.data:
            self.write_batchfile('win32-s')

cmakeu = CMakeU()
cmakeu.main();
