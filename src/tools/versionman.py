# -*- coding: utf-8 -*-

import re
import datetime
import json
import uuid
import re

TEMPL = r'''#ifndef VERSION_H
#define VERSION_H

#define VER_FILEVERSION             {version}
#define VER_FILEVERSION_STR         "{version_str}\0"

#define VER_PRODUCTVERSION          {version}
#define VER_PRODUCTVERSION_STR      "{version_str}\0"

#define VER_COMPANYNAME_STR         "Francois Blanchette"
#define VER_FILEDESCRIPTION_STR     "{file_desc}"
#define VER_INTERNALNAME_STR        "{int_file_desc}"
#define VER_LEGALCOPYRIGHT_STR      "Copyright {c} {year} Francois Blanchette"
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved"
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR
#define VER_ORIGINALFILENAME_STR    "{app_exe}"
#define VER_PRODUCTNAME_STR         "LGCK builder"

#define VER_COMPANYDOMAIN_STR       "cfrankb.com"

#endif // VERSION_H'''

with open('../shared/ss_version.h') as s:
    data = s.read()
    
ver = data.replace('#define SS_LGCK_VERSION 0x', '').strip() # 0x00060008

ver = re.findall('..?', ver)


now = datetime.datetime.now()

version = ','.join([str(int(x)) for x in ver])
version_str = version.replace(',','.')

with open("versionman.json") as s:
    data = s.read()
    files = json.loads(data)
for f in files:
    file_path = f['file_path'] #../lgck-builder/version.h"
    app_exe = f['app_exe']#lgck-builder.exe'
    file_desc =  f['file_desc']#'LGCK builder'
    int_file_desc = f['int_file_desc']

    with open(file_path, 'w') as t:
        t.write(TEMPL.format(
            version=version,
            version_str=version_str,
            year=now.year,
            app_exe=app_exe,
            file_desc=file_desc,
            int_file_desc=int_file_desc,
            c='\xa9')
        )



#build_no = 100

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
        line = '<ROW Property="ProductVersion" Value="{version}" Type="32"/>'.format(version='0.6.100')
    elif '<ROW Property="ProductCode" Value="' in line:
        line = '<ROW Property="ProductCode" Value="1033:{{{}}} " Type="16"/>'.format(str(uuid.uuid4()))
    elif '<ROW Property="UpgradeCode" Value="' in line:
        line= '<ROW Property="UpgradeCode" Value="{{{}}}"/>'.format(str(uuid.uuid4()))
    out_data.append(line)
    
with open('../build/test.aip', 'wb') as t:
    t.write('\r\n'.join(out_data))