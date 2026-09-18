#!/usr/bin/env python3
"""mn_repro.py <unidad> <src.cpp relativo> [flags extra...]
Compila UN .cpp con los cflags exactos de la unidad, al scratchpad, y deja el .o.
"""
import os
import subprocess
import sys

ROOT = 'C:/Users/jferr/Desktop/nfsdecompiled'
SCR = os.environ.get('SCRATCH')
BS = chr(92)
unit = sys.argv[1]
src = sys.argv[2]
extra = sys.argv[3:]
tag = os.environ.get('TAG', '')

lines = open(os.path.join(ROOT, 'build.ninja'), encoding='utf-8', errors='replace').read().split('\n')
joined = []
i = 0
while i < len(lines):
    cur = lines[i]
    while cur.endswith('$') and i + 1 < len(lines):
        i += 1
        cur = cur[:-1] + lines[i].strip()
    joined.append(cur)
    i += 1
lines = joined

tgt = BS.join(['build', 'GOWE69', 'src', 'Speed', 'Indep', 'SourceLists', unit + '.o'])
cflags = None
tv = None
for k, l in enumerate(lines):
    if l.startswith('build ' + tgt + ':'):
        for m in lines[k + 1:k + 14]:
            m = m.strip()
            if m.startswith('cflags = '):
                cflags = m[len('cflags = '):]
            if m.startswith('toolchain_version = '):
                tv = m[len('toolchain_version = '):]
            if m.startswith('build '):
                break
        break
assert cflags, 'no cflags for ' + unit

name = os.path.basename(src).replace('.cpp', '')
stub = os.path.join(SCR, 'stub_%s_%s%s.cpp' % (unit, name, tag))
open(stub, 'w').write(os.environ.get('PRE', '').replace('|', '\n') + '\n#include "%s"\n' % src)
out = os.path.join(SCR, 'r_%s_%s%s.o' % (unit, name, tag))
if os.path.exists(out):
    os.remove(out)
rootw = ROOT.replace('/', BS)
comp = BS.join([rootw, 'build', 'compilers', tv, 'ngccc.exe'])
env = dict(os.environ)
env['SN_NGC_PATH'] = BS.join([rootw, 'build', 'compilers', tv])
cmd = '"%s" %s %s -c -o "%s" "%s"' % (comp, cflags, ' '.join(extra), out, stub)
r = subprocess.run(cmd, shell=True, cwd=ROOT, capture_output=True, text=True, env=env)
if r.returncode != 0:
    sys.stderr.write(r.stdout[-4000:])
    sys.stderr.write(r.stderr[-4000:])
    sys.exit(1)
print(out)
