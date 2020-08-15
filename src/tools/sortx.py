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

lines = {
}

groups = ['SOURCES', 'HEADERS', 'FORMS']

preface = []

in_group = None
with open('../lgck-builder/lgck-builder.pro') as s:
    for line in s:
        line = line.strip('\r\n')
        found = False
        for g in groups:
            if line[0: len(g)] == g:
                print (f'found {g}')
                in_group = g
                found = True
                break
        
        if found: 
            continue
        elif in_group:
            line = line.strip().strip('\\ ')
            if not line or line[0] == '#':
                continue
            if not in_group in lines:
                lines[in_group] = []
            lines[in_group].append(line)
        else:
            preface.append(line)

print(lines)

with open('../lgck-builder/lgck-builder.pro', 'w') as t:
    t.write('\n'.join([x for x in preface]))

    for g in groups:
        t.write(f'\n{g} += \\\n')
        v = sorted(lines[g], key=str.casefold)
        t.write(' \\\n'.join([f'    {x}' for x in v]) + '\n')
