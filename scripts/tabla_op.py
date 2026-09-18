#!/usr/bin/env python3
"""tabla_op.py [--prefijo P] <diff.json> [...] -- la tabla "lo que emitimos -> lo que emite el original" de
las instrucciones que difieren SOLO en el codigo de operacion (DIFF_OP_MISMATCH, a igual posicion), con su
consistencia. Los diffs salen de scripts/diffunidad.py.

Si una forma nuestra va SIEMPRE a la misma del original, es una correccion directa, y no hay que deducirla
del nombre de la funcion: la da el binario. Asi salieron en la r81 las mascaras de campo del VU0 de PS2:

    vmul.xyzw   -> vmul.xyz    238   99 %
    vmulax.xyzw -> vmulax.x    179   99 %
    vsub.xyzw   -> vsub.xyz    133  100 %

y el 1 % que no cuadraba no era ruido, sino otra rutina (la distancia en el plano xz, con vmul.xz).

Por que OP_MISMATCH y no REPLACE/INSERT/DELETE: esos tres son el resultado de ALINEAR dos secuencias de
distinta longitud, y cuando el cuerpo difiere la cola queda desplazada -- son sintoma, no causa. Un
OP_MISMATCH es la misma posicion con otro codigo de operacion: no depende de la alineacion.

    python scripts/tabla_op.py build/diffs/SLES-53558-A124/*.json
    python scripts/tabla_op.py --prefijo v build/diffs/SLES-53558-A124/zAI.json   # solo VU0"""
import collections
import json
import sys

args = sys.argv[1:]
if not args or args[0] in ('-h', '--help'):
    print(__doc__)
    sys.exit(0)
PREF = ''
if args[0] == '--prefijo':
    PREF, args = args[1], args[2:]

por_nuestra = collections.defaultdict(collections.Counter)


def fmt(i):
    return ((i or {}).get('instruction') or {}).get('formatted') or ''


for p in args:
    d = json.load(open(p, encoding='utf-8'))
    R = {s['name']: s for s in d['right']['symbols']}
    for s in d['left']['symbols']:
        ins = s.get('instructions') or []
        ri = (R.get(s['name']) or {}).get('instructions') or []
        for i, x in enumerate(ins):
            if x.get('diff_kind') != 'DIFF_OP_MISMATCH' or i >= len(ri):
                continue
            a, b = fmt(x), fmt(ri[i])
            if not (a and b and a.startswith(PREF) and b.startswith(PREF)):
                continue
            por_nuestra[b.split(' ')[0]][a.split(' ')[0]] += 1

print('%-22s -> %-22s %6s   %s' % ('NUESTRO', 'ORIGINAL', 'veces', 'consistencia'))
for ob in sorted(por_nuestra, key=lambda k: -sum(por_nuestra[k].values()))[:40]:
    tot = sum(por_nuestra[ob].values())
    for oa, n in por_nuestra[ob].most_common():
        print('%-22s -> %-22s %6d   %3.0f %%' % (ob, oa, n, 100.0 * n / tot))
