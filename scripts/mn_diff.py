#!/usr/bin/env python3
"""mn_diff.py <unidad> <sym> <ours.o> [lim]  -- diff del OBJETIVO de <unidad> contra un .o cualquiera."""
import json
import os
import subprocess
import sys

ROOT = 'C:/Users/jferr/Desktop/nfsdecompiled'
SCR = os.environ.get('SCRATCH')
unit, sym, ours = sys.argv[1], sys.argv[2], sys.argv[3]
lim = int(sys.argv[4]) if len(sys.argv) > 4 else 3000
out = os.path.join(SCR, 'mnd_%s.json' % os.path.basename(ours))
r = subprocess.run([ROOT + '/objdiff-cli-windows-x86_64.exe', 'diff',
                    '-1', '%s/build/GOWE69/obj/Speed/Indep/SourceLists/%s.o' % (ROOT, unit),
                    '-2', ours,
                    '-c', 'function_reloc_diffs=none', '-c', 'ppc.calculatePoolRelocations=false',
                    '-o', out, '--format', 'json', sym], capture_output=True, text=True)
if r.returncode != 0:
    print(r.stdout[-3000:])
    print(r.stderr[-3000:])
    sys.exit(1)
d = json.load(open(out))


def get(side):
    for s in d[side]['symbols']:
        if s.get('name') == sym:
            return s
    return None


A, B = get('left'), get('right')
if A is None or B is None:
    print('not found', A is None, B is None)
    sys.exit(1)
LI, RI = A.get('instructions', []), B.get('instructions', [])
# TRAMPA (ronda 17): contra un .o suelto objdiff devuelve a veces match_percent
# = None y el diff sale VACIO, que se lee igual que un acierto. Grita en vez de
# imprimir columnas mudas: sin porcentaje la medida NO vale.
if A.get('match_percent') is None or B.get('match_percent') is None:
    print('!!! SIN PORCENTAJE: objdiff devuelve match_percent=None '
          '(target=%s ours=%s). El diff de abajo NO es medida: no lo leas como '
          'que casa. Cierra contra el .o de la unidad.'
          % (A.get('match_percent'), B.get('match_percent')))
print('# %s target=%s%% ours=%s%% size=%s/%s' % (sym, A.get('match_percent'), B.get('match_percent'), A.get('size'), B.get('size')))


def fmt(e):
    if e is None:
        return ('', '')
    i = e.get('instruction') or {}
    return (i.get('formatted', '').strip(), e.get('diff_kind', '') or '')


only = os.environ.get('ONLY') == '1'
n = max(len(LI), len(RI))
for k in range(min(n, lim)):
    a = LI[k] if k < len(LI) else None
    b = RI[k] if k < len(RI) else None
    ta, ka = fmt(a)
    tb, kb = fmt(b)
    kk = ''
    if ka not in ('', 'DIFF_NONE'):
        kk += ka.replace('DIFF_', '')
    if kb not in ('', 'DIFF_NONE'):
        kk += '/' + kb.replace('DIFF_', '')
    if only and not kk:
        continue
    print('%s %4d %-46s | %-46s %s' % ('>>>' if kk else '   ', k, ta[:46], tb[:46], kk))
if n > lim:
    print('...truncated', n)
