#!/usr/bin/env python3
"""datacmp.py -- cuanto DATO tenemos ya bien, comparando BYTES y no nombres.

El porcentaje `data` de objdiff no sirve para guiar el trabajo: empareja los
simbolos de datos por NOMBRE, y los del objetivo son `lbl_<direccion>` puestos
por dtk mientras los nuestros son `$LC*` y estaticos manglados. No casa
practicamente ninguno, asi que informa 0,0 % en unidades cuyo dato ya esta bien:

    zTrack     92,9 % de bytes identicos   ->  objdiff dice 0,0 %
    zMain      80,0 %                      ->  objdiff dice 0,0 %
    zPlatform  64,3 %                      ->  objdiff dice 0,0 %

Esto compara el CONTENIDO de cada seccion con datos contra el objeto extraido,
byte a byte y en orden, y da lo que de verdad falta. `.bss`/`.sbss` no tienen
contenido (NOBITS): de esas solo se compara el TAMANO.

    python scripts/datacmp.py              # todas, ordenadas por bytes distintos
    python scripts/datacmp.py --sl         # solo SourceLists
    python scripts/datacmp.py zTrack       # detalle por seccion
"""
import glob
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CON = ('.rodata', '.data', '.sdata', '.sdata2')
SIN = ('.bss', '.sbss')


def secs(p):
    try:
        f = open(p, 'rb').read()
    except OSError:
        return None
    if f[:4] != b'\x7fELF':
        return None
    try:
        shoff = struct.unpack('>I', f[0x20:0x24])[0]
        se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
        S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
        stro = S[sx][4]
    except (struct.error, IndexError):
        return None
    out = {}
    for s in S:
        e = f.index(b'\0', stro + s[0])
        n = f[stro + s[0]:e].decode()
        if n in CON and s[5]:
            out[n] = f[s[4]:s[4] + s[5]]
        elif n in SIN and s[5]:
            out[n] = ('bss', s[5])
    return out


def compara(u):
    a = secs(os.path.join(ROOT, 'build', 'GOWE69', 'obj', *u.split('/')) + '.o')
    b = secs(os.path.join(ROOT, 'build', 'GOWE69', 'src', *u.split('/')) + '.o')
    if a is None or b is None:
        return None
    filas = []
    for k in sorted(set(a) | set(b)):
        x, y = a.get(k), b.get(k)
        if isinstance(x, tuple) or isinstance(y, tuple):
            ta = x[1] if isinstance(x, tuple) else 0
            tb = y[1] if isinstance(y, tuple) else 0
            filas.append((k, ta, tb, None, None))
            continue
        x, y = x or b'', y or b''
        n = min(len(x), len(y))
        ig = sum(1 for i in range(n) if x[i] == y[i])
        pri = next((i for i in range(n) if x[i] != y[i]), n if len(x) == len(y) else n)
        filas.append((k, len(x), len(y), ig, pri))
    return filas


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    solo_sl = '--sl' in sys.argv
    base = os.path.join(ROOT, 'build', 'GOWE69', 'obj')
    unidades = []
    for p in glob.glob(os.path.join(base, '**', '*.o'), recursive=True):
        u = os.path.relpath(p, base).replace(os.sep, '/')[:-2]
        if u.startswith('auto_'):
            continue
        if solo_sl and '/SourceLists/' not in u:
            continue
        if args and not any(a in u for a in args):
            continue
        unidades.append(u)

    if args:
        for u in unidades:
            f = compara(u)
            if not f:
                continue
            print('== %s' % u)
            for k, ta, tb, ig, pri in f:
                if ig is None:
                    print('   %-9s objetivo %7d  nuestro %7d   (NOBITS: solo tamano)  %s'
                          % (k, ta, tb, 'IGUAL' if ta == tb else '%+d' % (tb - ta)))
                else:
                    print('   %-9s objetivo %7d  nuestro %7d   iguales %7d (%5.1f%%)  1a diferencia +0x%X'
                          % (k, ta, tb, ig, 100.0 * ig / max(ta, 1), pri))
        return

    filas = []
    for u in unidades:
        f = compara(u)
        if not f:
            continue
        tot = ig = 0
        for k, ta, tb, i, pri in f:
            if i is None:
                tot += max(ta, tb)
                ig += min(ta, tb)
            else:
                tot += max(ta, tb)
                ig += i
        if tot:
            filas.append((tot - ig, tot, ig, u))
    filas.sort(reverse=True)
    print('%d unidades con datos; %d B distintos de %d (%.1f%% ya igual)'
          % (len(filas), sum(f[0] for f in filas), sum(f[1] for f in filas),
             100.0 * sum(f[2] for f in filas) / max(sum(f[1] for f in filas), 1)))
    print()
    print('%10s %10s %7s  %s' % ('distintos', 'total', 'ya bien', 'unidad'))
    for d, tot, ig, u in filas[:30]:
        print('%10d %10d %6.1f%%  %s' % (d, tot, 100.0 * ig / tot, u))


if __name__ == '__main__':
    main()
