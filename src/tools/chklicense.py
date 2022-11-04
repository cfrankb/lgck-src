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

import glob
import os
from datetime import date
from os.path import join
from lgckutil.license import *

specs = [
    '../shared/',
    '../lgck-builder/',
    '../lgck-runtime/',
    '../obl5edit/',
    '../easydoc/',
]

def scan_folder(folder):
    folders = [x[0] for x in os.walk(folder) if '/build/' not in x[0]]
    for x in folders[1:]:
        scan_folder(x)
    
    for ext in ['*.h', '*.cpp']:
        for x in glob.glob(f'{folder}/{ext}'):
            if 'version.h' in x or 'ss_' in x:
                continue
            with open(x, 'r') as s:
                data = s.read()
            if data[0:2] != '/*':
                print('*', x)
                with open(x, 'w') as t:
                    t.write(license_cpp + data)

for x in specs:
    for y in glob.glob(x):
        scan_folder(y)
