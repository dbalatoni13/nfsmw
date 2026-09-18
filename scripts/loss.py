"""Ranking por BYTES PERDIDOS, no por instrucciones malas.

    python scripts/fndiff.py <unit> zz   # genera el d_<unit>.json
    python scripts/loss.py <unit>        # ranking
    python scripts/loss.py <unit> miss   # solo simbolos que no existen en el .o nuestro

cheap.py filtra a 60-100% y por eso NUNCA ensena las funciones al 0-25%,
que es donde suele estar el 90% de la ganancia (stubs, dtors sin definir,
globals extern sin definicion que se comen sus ctors del static-init).
"""
import json, os, sys

SCR = os.environ.get(
    'SCRATCH',
    'C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad')

unit = sys.argv[1]
mode = sys.argv[2] if len(sys.argv) > 2 else 'loss'
d = json.load(open('%s/d_%s.json' % (SCR, unit)))
R = {s['name']: s for s in d['right']['symbols']}
L = [s for s in d['left']['symbols'] if s.get('kind') == 'SYMBOL_FUNCTION']

if mode == 'miss':
    rows = []
    for s in L:
        r = R.get(s['name'])
        if r is None or r.get('size') in (None, 0, '0'):
            rows.append((int(s['size']), s['name']))
    rows.sort(reverse=True)
    print('# %s: %d simbolos ausentes, %d bytes' % (unit, len(rows), sum(r[0] for r in rows)))
    for sz, n in rows:
        print('%7d  %s' % (sz, n))
    sys.exit(0)

if mode == 'stub':
    # definidas pero mucho mas cortas que el original: cabecera que miente / sin implementar
    rows = []
    for s in L:
        r = R.get(s['name'])
        if r is None or r.get('size') is None:
            continue
        ls, rs = int(s['size']), int(r['size'])
        if ls - rs >= 32:
            rows.append((ls - rs, ls, rs, s.get('match_percent') or 0, s['name']))
    rows.sort(reverse=True)
    for dd, ls, rs, mp, n in rows[:40]:
        print('-%6d  orig %6d  ours %6d  %6.2f%%  %s' % (dd, ls, rs, mp, n))
    sys.exit(0)

rows = []
tot = 0
for s in L:
    mp = s.get('match_percent') or 0
    sz = int(s['size'])
    tot += sz
    rows.append((sz * (100 - mp) / 100.0, sz, mp, s['name']))
rows.sort(reverse=True)
print('# %s  total_code=%d  perdidos=%d' % (unit, tot, sum(r[0] for r in rows)))
print('%9s %7s %7s  %s' % ('lossB', 'size', 'pct', 'name'))
for l, sz, mp, n in rows[:40]:
    print('%9.0f %7d %7.2f  %s' % (l, sz, mp, n))
