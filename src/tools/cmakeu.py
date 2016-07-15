#!/usr/bin/python
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
                for j in self.data[el][i]:
                    if '*' in j:
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
                                    print "fail to read {k}".format(k=k)
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
        except:
            print "can't open %s" % args.file
            exit(-1)
        self.data = json.loads(raw)
        for el in self.data['__elements']:
            if self.data.has_key(el):
                self.process_element(el)
            else:
                print 'missing element:' + el
        try:
            with open(args.output, 'w') as f:
                f.write('\n'.join([e for e in self.lines]))
            print "completed"
        except:
            print "can't write target {f}".format(f=args.output)
            exit(-1)

cmakeu = CMakeU()
cmakeu.main();
