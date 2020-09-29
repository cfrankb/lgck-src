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
import argparse
from datetime import date
import os.path
import collections
from lgckutil.license import *

#http://www.dailycoding.com/Posts/enum_coversion_operations_int_to_enum_enum_to_int_string_to_enum_enum_to_string.aspx
#http://www.lua.org/manual/5.2/manual.html#lua_pushnumber
class GenLua():
    def __init__(self):
        pass

    def process(self):
        atypes = self.data['types']['atypes']
        ltypes = self.data['types']['ltypes']
        outt = self.data['types']['outt']
        tf_lua = open(self.data['output']['lua'], 'w')
        print("OUT LUA: {}".format(self.data['output']['lua']))
        #write_GPL(tf_lua, '--[[', ']]--')
        tf_lua.write(license_lua)
        tf_lua.write('-- auto-generated\n')
        tf_lua.write('''
if unpack == nil then
    -- to keep lua 5.2 happy
    unpack = table.unpack;
end
\n''')
        cpp_file = self.data['output']['cpp']
        print("OUT CPP: {}".format(self.data['output']['cpp']))
        src = ''
        if os.path.isfile(cpp_file):
            sf_cpp = open(cpp_file, 'r')
            src = sf_cpp.read()
            sf_cpp.close()
            src = src.split('// auto-generated\n',1)
        tf_cpp = open(cpp_file, 'w')
        if src and len(src)>1:
            tf_cpp.write(src[0])
        else:
            tf_cpp.write(license_cpp)
        tf_cpp.write('// auto-generated\n')
        cl_dict = {}
        for cl, v in self.data['classes'].items():
            cl_dict[cl] = {
                "delete":{'outv':'', 'args': []},
                "use":{'outv':'', 'args': []},
            }
            for f in v:
                xx = f.split('#', 1)
                outv = xx[1] if len(xx)>1 else ''
                vv = xx[0].split(':',1)
                fct = vv[0]
                args = vv[1] if len(vv) > 1 else []
                if fct in cl_dict[cl]:
                    print ('%s already defined for %s' % (fct, cl))
                cl_dict[cl][fct]={ 'args': args.split(',') if args else [], 'outv': outv }
        for ch,pa in self.data['inheritance'].items():
            cl_dict[ch] = { **cl_dict[pa], **cl_dict[ch] }
        for cl,v in sorted(cl_dict.items()):
            if self.args.verbose:
                print (cl)
            tf_lua.write('%s = {};\n' % cl)
            tf_lua.write('%s.__index = %s;\n\n' % (cl, cl))
            ### cpp code
            for fct in sorted(v):
                if self.args.verbose:
                    print ('    %s' % fct)
                if fct == 'use':
                    continue
                outv = cl_dict[cl][fct]['outv']
                if fct not in ('new'):
                    argc = 1
                else:
                    argc = 0
                argc_max = argc
                args = cl_dict[cl][fct]['args']
                if len(args):
                    for i in range(0, len(args)):
                        if '=' not in args[i]:
                            argc = argc + 1
                        argc_max = argc_max +1
                fcta = []
                tf_cpp.write('int %s_%s(lua_State *L)\n{\n' % (cl.lower(), fct))
                tf_cpp.write('    int argc = lua_gettop(L);\n')
                if argc == argc_max:
                    tf_cpp.write('    if (argc != %d) {\n' % argc)
                else:
                    tf_cpp.write('    if (argc < %d and argc > %d) {\n' % (argc, argc_max))
                tf_cpp.write('       error(__func__, %d);\n' % argc)
                tf_cpp.write('    } else {\n')
                if fct != 'new':
                    tf_cpp.write('        int id = lua_tointeger(L, 1);\n')
                    start = 1
                else:
                    start = 0
                for i in range(0, len(args)):
                    pp = args[i].split('=',1)
                    defv = ''
                    atype = pp[0]
                    if len(pp)>1:
                        defv = pp[1]
                    if atype[0]=='^':
                        cast = atype[1:]
                        atype = 'int'
                    else:
                        cast = ''
                    ltype = atype
                    if ltype in ltypes:
                        ltype = ltypes[ltype]
                    if atype in atypes:
                        suff = ''
                        if defv != '':
                            itype = atypes[atype]
                            argcn = i+1+start
                            tf_cpp.write('        %s arg%d = (argc >= %d) ? lua_to%s(L, %d) : %s;\n'
                                % (itype, i, argcn,
                                ltype, argcn,
                                f'static_cast<{atype}>({defv})' if (atype and cast) else defv))
                        else:
                            tf_cpp.write('        %s arg%d = lua_to%s(L, %d);\n' % (atypes[atype], i, ltype, i+1+start))
                        if cast:
                            fcta.append('static_cast<%s>(arg%d%s)' % (cast, i, suff))
                        else:
                            fcta.append('arg%d%s' % (i, suff))
                    else:
                        tf_cpp.write('        int arg%d = lua_to%s(L, %d);\n' % (i, 'integer', i+1+start))
                        fcta.append('static_cast<%s*>(g_controls[arg%d])' % (atype, i))
                if fct == 'new':
                    tf_cpp.write('        return addWidget(new %s(%s));\n' % (cl, ','.join([fa for fa in fcta])))
                elif fct == 'delete':
                    tf_cpp.write('        delete static_cast<%s*>(g_controls[id]);\n' % cl)
                    tf_cpp.write('        freeWidget(id);\n')
                else:
                    tf_cpp.write('        ')
                    if outv in outt :
                        tf_cpp.write('%s tmp = ' % outt[outv][0])
                    tf_cpp.write('static_cast<%s*>(g_controls[id])->%s(%s);\n' % (cl, fct, ('\n            ' if len(fcta)>1 else '') + ',\n            '.join([fa for fa in fcta])))
                    if outv in outt :
                        tf_cpp.write('        lua_push%s(L, tmp%s);\n' % (outt[outv][2], outt[outv][1]))
                        tf_cpp.write('        return 1;\n')                    
                tf_cpp.write('    }\n')
                tf_cpp.write('    return 0;\n')
                tf_cpp.write('}\n\n')
            ### lua code
            for fct in sorted(v):
                if fct in ('new', 'use'):
                    args ={'use': 'id', 'new': '...'}
                    tf_lua.write('function %s:%s(%s)\n' % (cl,fct,args[fct]))
                    tf_lua.write('   local q = {};\n')
                    tf_lua.write('   setmetatable(q, %s);\n' % cl)
                    if fct=='new':
                        tf_lua.write('   if arg ~= nil then\n')
                        tf_lua.write('      q.id = %s_new(unpack(arg))\n' % cl.lower())
                        tf_lua.write('   else\n')
                        tf_lua.write('      q.id = %s_new()\n' % cl.lower())
                        tf_lua.write('   end\n')
                    elif fct=='use':
                        tf_lua.write('   q.id = id;\n')
                    tf_lua.write('   return q\n')
                    tf_lua.write('end\n\n')
                else:
                    fcta = []
                    fctb = ['self.id']
                    args = cl_dict[cl][fct]['args']
                    for i in range(0, len(args)):
                        if '=' in args[i]:
                            fcta.append('...')
                            fctb.append('unpack(arg)')
                            break
                        elif (args[i] in atypes) or (args[i][0]=='^'):
                            fctb.append('arg%d' % i)
                        else:
                            fctb.append('arg%d.id' % i)
                        fcta.append('arg%d' % i)
                    tf_lua.write('function %s:%s(%s)\n' %(cl, fct if fct != 'end' else 'endl', ','.join([fa for fa in fcta])))
                    if 'unpack(arg)' in fctb:
                        tf_lua.write('    if arg == nil then\n')
                        tf_lua.write('        arg = {}\n')
                        tf_lua.write('    end\n')
                    tf_lua.write('    ')
                    if cl_dict[cl][fct]['outv'] in outt:
                        tf_lua.write('return ')
                    tf_lua.write('%s_%s(%s);\n' %(cl.lower(), fct, ','.join([fa for fa in fctb])))
                    if fct=='delete':
                        tf_lua.write('    self.id = 0;')
                    tf_lua.write('end\n\n')
        tf_lua.close()
    
    def main(self):
        parser = argparse.ArgumentParser(description='Utility to generate lua binding for QT')
        parser.add_argument('file', default='conf/genlua.json', help='The json file to parse (genlua.json)')
        parser.add_argument('-v', dest='verbose', action='store_true', help= "verbose")
        self.args = parser.parse_args()        
        try:
            f = open(self.args.file, 'r') 
            raw = f.read() 
            f.close()
        except:
            print ("can't open %s" % args.file)
            exit(-1)
        self.data = json.loads(raw)
        self.process()

p = GenLua()
p.main();
