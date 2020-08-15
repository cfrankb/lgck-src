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
import json
import glob
import argparse

class CMakeU():
    def __init__(self):
        self.data = {}
        self.lines = []

    def process_element(self,el):
        if type(self.data[el]) is list:
            for i in self.data[el]:
                self.lines.append(el + '(' + i + ')')
            self.lines.append('')
        elif type(self.data[el]) is dict:
            for i in self.data[el]:
                self.lines.append('')
                self.lines.append(el + '(' + i + ' ')
                w = self.data[el][i]
                p = w if type(w) is list else [w]
                for j in p:
                    if type(j) is not str:
                        self.lines.append('    ' + str(j))
                    elif '*' in j:
                        if ':' in j:
                            v = j.split(':',2)
                        else:
                            v = [j]
                        l = glob.glob(v[0])
                        l.sort();
                        for k in l:
                            if ':' in j:
                                try:
                                    f = open(k) 
                                    text = f.read() 
                                    f.close()
                                    if v[1] in text:
                                        self.lines.append('    ' + k)
                                except:
                                    print ("fail to read {k}".format(k=k))
                            else:
                                self.lines.append('    ' + k)
                    else:
                        self.lines.append('    ' + j)
                self.lines.append(')')            
        else:
            self.lines.append(el + '(' + self.data[el] + ')')

    def main(self):
        parser = argparse.ArgumentParser(description='CMakeFile utility for LGCK Builder')
        parser.add_argument('file', default='cmakeu.json', help='The json file to parse (cmakeu.json)')
        parser.add_argument('-o', '--output', required=False, default='CMakeLists.txt')
        args = parser.parse_args()
        try:
            with open(args.file, 'r') as f:
                raw = f.read()
            raw = raw.replace('\r\n', '\n')
            a = raw.split('\n')
            raw = '\n'.join([x for x in a if not x.strip() or x.strip()[0] != '#'])
        except:
            print ("can't open %s" % args.file)
            exit(-1)
        self.data = json.loads(raw)
        for el in self.data['__elements']:
            if el in self.data:
                self.process_element(el)
            else:
                print ('missing element:' + el)
        try:
            with open(args.output, 'w') as f:
                f.write('\n'.join([e for e in self.lines]))
            print ("completed")
        except:
            print ("can't write target {f}".format(f=args.output))
            exit(-1)

cmakeu = CMakeU()
cmakeu.main();
