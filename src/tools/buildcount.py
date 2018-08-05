import os
from os.path import join

path = os.path.dirname(os.path.realpath(__file__))
DATA = join(path, 'buildcount.dat')

with open(DATA, 'r') as s:
    build_no = s.read().strip()
    
if not build_no:
    build_no = 0

build_no = int(build_no) + 1

with open(DATA, 'w') as t:
    t.write(str(build_no))

os.chdir(path)

print(build_no)
target = "../shared/ss_build.h"
with open(target, 'wb') as t:
    t.write("#define SS_BUILD {}".format(build_no))
