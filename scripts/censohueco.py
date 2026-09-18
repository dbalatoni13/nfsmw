#!/usr/bin/env python3
"""censohueco.py [minimo] -- donde esta el hueco recuperable de PS2, por unidad. Lee los informes que deja
scripts/pctall.py en build/versiones/.

Cuenta los bytes de funciones CASI casadas (near-miss, por defecto 80-100 %), que valen su tamano integro
porque matched_code es todo-o-nada. Y dos columnas que son las que encuentran CAUSAS COMUNES:

  GC al 100   de esas funciones, cuantas estan al 100 % en GameCube. Si son casi todas, la fuente es buena
              y lo que falla es algo de PS2 (compilador, layout, flags), no la forma del codigo.
  en ambas    cuantas estan near-miss en las DOS versiones de PS2 a la vez: causa compartida.

Asi salio en la r81 que de 259 near-miss de zAI en PS2, 243 estaban al 100 % en GameCube: era algo de PS2
(las mascaras de campo del VU0), no 259 funciones mal escritas.

    python scripts/censohueco.py          # 80-100 %
    python scripts/censohueco.py 95       # solo los muy cercanos"""
import collections
import json
import os
import sys

LO = float(sys.argv[1]) if len(sys.argv) > 1 and sys.argv[1] not in ('-h', '--help') else 80.0
if len(sys.argv) > 1 and sys.argv[1] in ('-h', '--help'):
    print(__doc__)
    sys.exit(0)
V = {'GC': 'build/versiones/GOWE69.json', 'EU': 'build/versiones/SLES-53558-A124.json',
     'US': 'build/versiones/SLUS-21351.json'}


def carga(p):
    out = {}
    if not os.path.exists(p):
        return out
    for u in json.load(open(p, encoding='utf-8')).get('units', []):
        un = u.get('name', '').split('/')[-1]
        for f in u.get('functions') or []:
            pct = f.get('fuzzy_match_percent')
            out[f.get('name')] = (un, int(f.get('size') or 0), float(pct) if pct is not None else 0.0)
    return out


R = {k: carga(p) for k, p in V.items()}
if not R['EU'] and not R['US']:
    sys.exit('no hay informes de PS2 en build/versiones/: corre antes `python scripts/pctall.py`')

for k in ('EU', 'US'):
    por_u = collections.defaultdict(lambda: [0, 0, 0, 0])
    otra = 'US' if k == 'EU' else 'EU'
    for n, (un, sz, pct) in R[k].items():
        if LO <= pct < 100.0:
            e = por_u[un]
            e[0] += sz
            e[1] += 1
            g = R['GC'].get(n)
            if g and g[2] >= 100.0:
                e[2] += 1
            o = R[otra].get(n)
            if o and LO <= o[2] < 100.0:
                e[3] += 1
    tot = sum(v[0] for v in por_u.values())
    print('\n== %s: near-miss %.0f-100 %%, %d bytes en total' % ({'EU': 'SLES', 'US': 'SLUS'}[k], LO, tot))
    print('   %-26s %9s %6s %10s %9s' % ('unidad', 'bytes', 'fn', 'GC al 100', 'en ambas'))
    for un, (b, n, g, a) in sorted(por_u.items(), key=lambda x: -x[1][0])[:16]:
        print('   %-26s %9d %6d %10d %9d' % (un, b, n, g, a))
