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
import argparse
import glob
from os.path import join
import os

def make_list():
    paths = ['../lgck-builder', '../obl5edit', '../easydoc', '../tools', '../shared']
    specs = ['*.cpp', '*.h', '*.pro', '*.xml', '*.py', '*.json', '*.rc']
    files = []

    for path in paths:
        for spec in specs:
            for f in glob.glob(join(path, spec)):
                if os.path.isfile(f):
                    files.append(f)
    return files

def process_files(files, update):
    for f in files:
        if not os.path.isfile(f):
            print('not a file: {}'.format(f))
            break
        with open(f, 'rb') as s:
            data = s.read()
        if b'\r\n' in data:
            b = len(data)
            data = data.replace(b'\r\n', b'\n')
            a = len(data)
            print("{0} {1} {2}".format(f, a, b))
            if update:
                with open(f, 'wb') as t:
                    t.write(data)
                print('UPDATED')

def main():
    parser = argparse.ArgumentParser(description='utility')
    parser.add_argument('file', default='', nargs='?', help='file to patse')
    parser.add_argument('-u', dest='update', action='store_true', help= "update")
    args = parser.parse_args()
    
    if args.file:
        files = glob.glob(args.file)
    else:
        files = make_list()
    
    process_files(files, args.update)

main()
