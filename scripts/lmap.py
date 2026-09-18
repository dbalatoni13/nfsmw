#!/usr/bin/env python3
"""lmap.py <unidad> <simbolo>  -- asm del OBJETIVO con la linea de fuente al lado.

Funde el .s del troceador (que trae la direccion real de cada instruccion) con
symbols/debug_lines.txt. El orden y los huecos de las lineas delatan el orden de
las sentencias del original, que es lo que decide el reparto de registros.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__))).replace(os.sep, '/')
ASMDIR = ROOT + '/build/GOWE69/asm'


def asm_de(unit):
    """Encuentra el .s del troceador acepte lo que acepte el usuario.

    Antes esto estaba CABLEADO a SourceLists, asi que la herramienta mas
    productiva del proyecto no servia en el middleware --que es justo donde
    esta ahora el trabajo--. Prueba, por este orden: la ruta tal cual, bajo
    SourceLists, y por ultimo el nombre suelto en cualquier subdirectorio.
    """
    for c in (ASMDIR + '/%s.s' % unit,
              ASMDIR + '/Speed/Indep/SourceLists/%s.s' % unit):
        if os.path.exists(c):
            return c
    base = os.path.basename(unit) + '.s'
    for dp, _, fs in os.walk(ASMDIR):
        if base in fs:
            return os.path.join(dp, base)
    raise SystemExit('no encuentro el asm de %s bajo %s' % (unit, ASMDIR))

unit, sym = sys.argv[1], sys.argv[2]
only = sys.argv[3] if len(sys.argv) > 3 else None

lo = hi = None
rows = []
inside = False
RE = re.compile(r'^/\* ([0-9A-F]{8}) [0-9A-F]{8} +([0-9A-F ]+)\*/\t(.*)$')
for ln in open(asm_de(unit), encoding='utf-8', errors='replace'):
    t = ln.rstrip()
    if t.startswith('.fn ') and t.split()[1].rstrip(',') == sym:
        inside = True
        continue
    if inside and t.startswith('.endfn'):
        break
    if inside:
        m = RE.match(t)
        if m:
            a = int(m.group(1), 16)
            lo = a if lo is None else lo
            hi = a
            rows.append((a, m.group(3)))
        elif t.strip():
            rows.append((None, t.strip()))

lines = {}
for ln in open(ROOT + '/symbols/debug_lines.txt', encoding='utf-8', errors='replace'):
    m = re.match(r'^0x([0-9A-Fa-f]+): (.*) \(line (\d+)\)', ln.strip())
    if not m:
        continue
    a = int(m.group(1), 16)
    if lo is not None and lo <= a <= hi:
        f = m.group(2).replace('\\', '/').split('/')[-1]
        if only and only not in f:
            continue
        lines.setdefault(a, []).append('%s:%s' % (f, m.group(3)))

print('# %s  %08X..%08X  (%d B)' % (sym, lo, hi + 4, hi + 4 - lo))
for a, txt in rows:
    if a is None:
        print('          %s' % txt)
    else:
        print('%08X  %-44s %s' % (a, txt, '  '.join(lines.get(a, []))))
