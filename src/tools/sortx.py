
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

with open('../lgck-builder/lgck-builder-clean.pro', 'w') as t:
    t.write('\n'.join([x for x in preface]))

    for g in groups:
        t.write(f'\n{g} += \\\n')
        v = sorted(lines[g], key=str.casefold)
        t.write(' \\\n'.join([f'    {x}' for x in v]) + '\n')
