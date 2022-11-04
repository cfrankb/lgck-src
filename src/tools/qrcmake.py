import glob
import os
from os.path import join

header = '''<RCC>
    <qresource prefix="/">\n'''

footer = '''    </qresource>
</RCC>'''

files = []
files0 = []

excluded = ['CMakeLists.txt', 'lgck-builder.ico']

with open('../lgck-builder/lgck-builder.qrc') as s:
    for line in s:
        if '<file>' not in line:
            continue
        line = line.replace('<file>', '').replace('</file>', '').strip()
        files0.append(line)

def scan_folder(folder):
    print('folder:', folder)
    for fname in glob.glob(join('../lgck-builder', folder, '*')):
        basename = os.path.basename(fname)
        if os.path.isdir(fname) and basename[0] != '.':
            scan_folder(join(folder, basename))
        elif basename in excluded:
            continue
        elif os.path.isfile(fname):
            files.append(join(folder, basename))

def add_text_files():
    pages = []
    for fname in glob.glob('/home/cfrankb/websites/htdocs/cfrankb/lgck/data/pages/*.txt'):
        basename = os.path.basename(fname)
        pages.append(basename)
        with open(fname, 'r') as s:
            data = s.read()
        with open(f'../lgck-builder/res/help/{basename}', 'w') as t:
            t.write(data)
    with open(f'../lgck-builder/res/help/files.dat', 'w') as t:
        t.write('\n'.join(pages))

def create_file(files):
    with open('../lgck-builder/lgck-builder1.qrc', 'w') as t:
        t.write(header)
        t.write('\n'.join([f'        <file>{x}</file>' for x in files]))
        t.write('\n' + footer)

folders = ['images', 'res', 'scripts']
for folder in folders:
    scan_folder(folder)

files = list(set(files))
files.sort()
print(set(files) - set(files0))
