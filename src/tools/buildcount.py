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
import uuid
import re
import os
from os.path import join, isfile

path = os.path.dirname(os.path.realpath(__file__))
DATA = join(path, 'buildcount.dat')

build_no = 0
if isfile(DATA):
    with open(DATA, 'r') as s:
        build_no = s.read().strip()

if not build_no:
    build_no = 0

build_no = int(build_no) + 1

with open(DATA, 'w') as t:
    t.write(str(build_no))

os.chdir(path)

print(build_no)

with open('../shared/ss_version.h') as s:
    data = s.read()
ver = data.replace('#define SS_LGCK_VERSION 0x', '').strip()
ver = re.findall('..?', ver)

version = '.'.join([str(int(x, 16)) for x in ver])
build = '{version} - build {build_no}'.format(version=version, build_no=build_no)
print (build)

target = "../shared/ss_build.h"
with open(target, 'w') as t:
    t.write('#define SS_BUILD "{}"\n'.format(build_no))
    t.write('#define SS_BUILD_STR "{}"'.format(build))

