#!/usr/bin/env python3
"""diffunidad.py <VERSION> <unidad> [<unidad>...] -- el diff completo de objdiff (JSON, instruccion a
instruccion) de una o varias unidades, para las herramientas de patrones (patrones.py, tabla_op.py,
desplazamientos.py). Compara build/<V>/obj/.../<unidad>.o (el original troceado) con build/<V>/src/...
(el nuestro), asi que hay que haber compilado antes esa version (scripts/pctall.py <V>).

    python scripts/diffunidad.py SLES-53558-A124 zAI zFe2 zWorld
    -> build/diffs/SLES-53558-A124/zAI.json, ...

Cada diff pesa decenas de MB (zAI: 42 MB): son de trabajo, no se versionan."""
import os
import subprocess
import sys

if len(sys.argv) < 3 or sys.argv[1] in ('-h', '--help'):
    print(__doc__)
    sys.exit(0)

ROOT = os.getcwd()
V = sys.argv[1]
CLI = os.path.normpath(os.path.join(ROOT, 'build', 'tools', 'objdiff-cli.exe'))
DEST = os.path.join(ROOT, 'build', 'diffs', V)
os.makedirs(DEST, exist_ok=True)


def busca(raiz, nombre):
    for r, _, ff in os.walk(raiz):
        if nombre + '.o' in ff:
            return os.path.join(r, nombre + '.o')
    return None


for u in sys.argv[2:]:
    t = busca(os.path.join(ROOT, 'build', V, 'obj'), u)
    b = busca(os.path.join(ROOT, 'build', V, 'src'), u)
    if not t or not b:
        print('  %-24s falta el objeto %s' % (u, 'ORIGINAL' if not t else 'NUESTRO (compila antes)'))
        continue
    out = os.path.join(DEST, u + '.json')
    r = subprocess.run([CLI, 'diff', '-1', t, '-2', b, '-c', 'functionRelocDiffs=none',
                        '--format', 'json', '-o', out], capture_output=True, text=True)
    if r.returncode or not os.path.exists(out):
        print('  %-24s objdiff falla' % u)
    else:
        print('  %-24s %s (%d MB)' % (u, os.path.relpath(out, ROOT), os.path.getsize(out) // 1048576))
