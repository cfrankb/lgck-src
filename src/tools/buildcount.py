import uuid
import re
import os
from os.path import join

path = os.path.dirname(os.path.realpath(__file__))
DATA = join(path, 'buildcount.dat')

build_no = 0
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
with open(target, 'wb') as t:
    t.write('#define SS_BUILD "{}"\n'.format(build_no))
    t.write('#define SS_BUILD_STR "{}"'.format(build))
    