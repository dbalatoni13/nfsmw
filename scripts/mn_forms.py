#!/usr/bin/env python3
"""mn_forms.py <unidad> <src.cpp rel> <sym> <formas.json>

formas.json: {"file": "ruta/al/fichero.cpp", "anchor": "texto exacto a sustituir",
              "forms": [["nombre", "texto nuevo"], ...]}
Parchea, compila el .cpp solo, mide, restaura SIEMPRE.
"""
import json
import os
import subprocess
import sys

ROOT = 'C:/Users/jferr/Desktop/nfsdecompiled'
SCR = os.environ.get('SCRATCH')
unit, src, sym, spec = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
S = json.load(open(spec))
path = os.path.join(ROOT, S['file'])
import re
orig = open(path, 'rb').read()
# los finales de linea son mixtos POR LINEA: se busca con \r?\n y se restituye
# el mismo terminador que llevaba cada linea del ancla.
pat = re.compile(b'\r?\n'.join(re.escape(l.encode()) for l in S['anchor'].split('\n')))
ms = list(pat.finditer(orig))
assert len(ms) == 1, 'anchor aparece %d veces' % len(ms)
M0, M1 = ms[0].start(), ms[0].end()
eols = re.findall(b'\r?\n', orig[M0:M1])


def render(txt):
    parts = txt.split('\n')
    out = parts[0].encode()
    for i, p in enumerate(parts[1:]):
        out += (eols[i] if i < len(eols) else b'\r\n') + p.encode()
    return out


def measure(tag):
    env = dict(os.environ)
    env['TAG'] = tag
    r = subprocess.run([sys.executable, os.path.join(SCR, 'mn_repro.py'), unit, src],
                       capture_output=True, text=True, env=env, cwd=ROOT)
    if r.returncode != 0:
        return ('BUILD-FAIL', None, None, r.stderr[-300:])
    o = r.stdout.strip()
    out = os.path.join(SCR, 'fm_%s.json' % tag)
    d = subprocess.run([ROOT + '/objdiff-cli-windows-x86_64.exe', 'diff',
                        '-1', '%s/build/GOWE69/obj/Speed/Indep/SourceLists/%s.o' % (ROOT, unit),
                        '-2', o, '-c', 'function_reloc_diffs=none',
                        '-c', 'ppc.calculatePoolRelocations=false', '-o', out, '--format', 'json', sym],
                       capture_output=True, text=True)
    if d.returncode != 0:
        return ('DIFF-FAIL', None, None, d.stderr[-300:])
    j = json.load(open(out))
    B = None
    for s in j['right']['symbols']:
        if s.get('name') == sym:
            B = s
    if B is None:
        return ('NO-SYM', None, None, '')
    ins = B.get('instructions', [])
    nd = sum(1 for e in ins if (e.get('diff_kind') or 'DIFF_NONE') != 'DIFF_NONE')
    # TRAMPA (ronda 17): match_percent=None con diffs=0 se lee como un 100%.
    # Sin porcentaje no hay medida: se marca aparte y no se compara.
    if B.get('match_percent') is None:
        return ('SIN-PCT (objdiff no da match_percent: NO es un acierto)', None, B.get('size'), nd)
    return ('OK', B.get('match_percent'), B.get('size'), nd)


try:
    for i, (name, txt) in enumerate(S['forms']):
        open(path, 'wb').write(orig[:M0] + render(txt) + orig[M1:])
        st, pc, sz, nd = measure('_v%d' % i)
        if st != 'OK':
            print('%-42s %s %s' % (name[:42], st, nd))
        else:
            print('%-42s %9.5f%%  size=%-8s diffs=%s' % (name[:42], pc, sz, nd))
        sys.stdout.flush()
finally:
    open(path, 'wb').write(orig)
    print('-- restaurado')
