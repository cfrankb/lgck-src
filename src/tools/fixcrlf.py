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
        if '\r\n' in data:
            b = len(data)
            data = data.replace('\r\n', '\n')
            a = len(data)
            print("{0} {1} {2}".format(f, a, b))
            if update:
                with open(f, 'wb') as t:
                    t.write(data)
                print 'UPDATED'

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