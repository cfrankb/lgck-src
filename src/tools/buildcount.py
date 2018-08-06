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
target = "../shared/ss_build.h"
with open(target, 'wb') as t:
    t.write("#define SS_BUILD {}".format(build_no))

with open('../shared/ss_version.h') as s:
    data = s.read()
ver = data.replace('#define SS_LGCK_VERSION 0x', '').strip()
ver = re.findall('..?', ver)[0:2] + [build_no]
print ver

version = '.'.join([str(int(x)) for x in ver])

with open('../build/lgck-setup.aip', 'rb') as s:
    in_data = s.read().split('\r\n')

out_data = []
for line in in_data:
    if '<ROW Property="ProductVersion" Value' in line:
        line = '<ROW Property="ProductVersion" Value="{version}" Type="32"/>'.format(version=version)
    elif '<ROW Property="ProductCode" Value="' in line:
        line = '<ROW Property="ProductCode" Value="1033:{{{}}} " Type="16"/>'.format(str(uuid.uuid4()))
    elif '<ROW Property="UpgradeCode" Value="' in line:
        line= '<ROW Property="UpgradeCode" Value="{{{}}}"/>'.format(str(uuid.uuid4()))
    out_data.append(line)

with open('../build/build.aip', 'wb') as t:
    t.write('\r\n'.join(out_data))
