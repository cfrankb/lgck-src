# -*- coding: utf-8 -*-
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

import re
import datetime
import json
import uuid
import re
from lgckutil.license import *

VERSIONMAN_CONF = "conf/versionman.json"
SS_VERSION_H = '../shared/ss_version.h'

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
#define VER_LEGALTRADEMARKS1_STR    ""
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR
#define VER_ORIGINALFILENAME_STR    "{app_exe}"
#define VER_PRODUCTNAME_STR         "LGCK builder"

#define VER_COMPANYDOMAIN_STR       "cfrankb.com"

#endif // VERSION_H'''

with open(SS_VERSION_H) as s:
    data = s.read()
    
a = data.split('*/', 1)
data = a[0] if len(a) == 1 else a[1]

ver = data.replace('#define SS_LGCK_VERSION 0x', '').strip() # 0x00060008
ver = re.findall('..?', ver)
now = datetime.datetime.now()
version = ','.join([str(int(x, 16)) for x in ver])
version_str = version.replace(',','.')

with open(VERSIONMAN_CONF) as s:
    data = s.read()
    files = json.loads(data)
for f in files:
    file_path = f['file_path'] #../lgck-builder/version.h"
    app_exe = f['app_exe']#lgck-builder.exe'
    file_desc =  f['file_desc']#'LGCK builder'
    int_file_desc = f['int_file_desc']

    with open(file_path, 'w') as t:
        t.write(license_cpp + TEMPL.format(
            version=version,
            version_str=version_str,
            year=now.year,
            app_exe=app_exe,
            file_desc=file_desc,
            int_file_desc=int_file_desc,
            c='\xa9'))
