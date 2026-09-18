"""Detector mecanico de CODIGO QUE FALTA, ordenado por bytes.

Busca funciones donde el objetivo emite `bl X` y nuestra version no llama a X
ni una sola vez. Eso no es ruido de planificacion ni de reparto de registros:
es una sentencia que no esta escrita, o una cabecera que inlinea algo que el
original llamaba fuera de linea. Es la clase de hallazgo mas barata que hay.

  python scripts/missingcalls.py                # todas las unidades
  python scripts/missingcalls.py zAI zPhysics   # solo esas
  python scripts/missingcalls.py --dtors        # solo destructores (_._X)

Interpretacion rapida de la salida:
  - pct muy bajo (<5%) y varios `bl` ausentes  -> la funcion esta sin escribir.
  - pct alto (>95%) y UN solo `bl` ausente     -> falta una sentencia concreta;
    suele cerrarse en minutos. Ej. `__builtin_vec_new` ausente = falta un `new[]`.
  - `_._X` ausente -> o el destructor de X esta definido DENTRO de la clase
    (en GCC 2.9 eso lo inlinea y no emite simbolo; sacarlo fuera lo arregla),
    o el llamante no construye el objeto local que el original si construye.
"""
import collections
import glob
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCR = os.environ.get(
    'SCRATCH',
    'C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad')
CLI = os.path.join(ROOT, 'objdiff-cli-windows-x86_64.exe')
OBJ = 'build/GOWE69/obj/Speed/Indep/SourceLists/%s.o'
OURS = 'build/GOWE69/src/Speed/Indep/SourceLists/%s.o'
RE_BL = re.compile(r'bl ([\w.$]+)')


def calls(sym):
    c = collections.Counter()
    for e in sym.get('instructions', []):
        t = ((e.get('instruction') or {}).get('formatted', '')).strip()
        m = RE_BL.match(t)
        if m:
            c[m.group(1)] += 1
    return c


def scan(unit, only_dtors):
    ours = os.path.join(ROOT, OURS % unit)
    if not os.path.exists(ours):
        return []
    out = os.path.join(SCR, 'mc_%s.json' % unit)
    r = subprocess.run([CLI, 'diff', '-1', OBJ % unit, '-2', OURS % unit,
                        '-c', 'function_reloc_diffs=none',
                        '-c', 'ppc.calculatePoolRelocations=false', '-o', out, '--format', 'json'],
                       cwd=ROOT, capture_output=True, text=True)
    if r.returncode != 0:
        return []
    try:
        d = json.load(open(out))
    finally:
        if os.path.exists(out):
            os.remove(out)
    right = {s['name']: s for s in d['right']['symbols']}
    rows = []
    for s in d['left']['symbols']:
        if s.get('kind') != 'SYMBOL_FUNCTION':
            continue
        pct = s.get('match_percent') or 0
        if not (0 < pct < 100):
            continue
        r2 = right.get(s['name'])
        if r2 is None:
            continue
        theirs, mine = calls(s), calls(r2)
        gone = [k for k in theirs if k not in mine]
        if only_dtors:
            gone = [k for k in gone if k.startswith('_._')]
        if gone:
            rows.append((int(s['size']), pct, unit, s['name'], gone))
    return rows


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    only_dtors = '--dtors' in sys.argv
    units = args or sorted(os.path.basename(p)[:-2]
                           for p in glob.glob(os.path.join(ROOT, OBJ % '*')))
    rows = []
    for u in units:
        rows.extend(scan(u, only_dtors))
    rows.sort(reverse=True)
    print('%8s %7s %-14s %-52s %s' % ('bytes', 'pct', 'unidad', 'funcion', 'bl ausentes'))
    for sz, pct, u, name, gone in rows:
        print('%8d %6.2f%% %-14s %-52s %s' % (sz, pct, u, name[:52],
                                              ','.join(x[:34] for x in gone[:3])))
    print('TOTAL: %d funciones, %d B' % (len(rows), sum(r[0] for r in rows)))


if __name__ == '__main__':
    main()
