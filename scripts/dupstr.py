#!/usr/bin/env python3
"""dupstr.py -- cadenas que emitimos MAS VECES que el objetivo, por CONTENIDO.

`dupdata.py` busca el duplicado por (seccion, tamano, bytes) entre un simbolo que
define un `asm()` y otro del compilador. Eso se le escapa cuando la copia a mano
va DENTRO de un bloque grande sin simbolo propio --que es como se escriben los
prefijos de bWare/STL de este proyecto--. Esto cuenta apariciones de cada cadena
terminada en NUL en la `.rodata` de los dos objetos y marca las que salen de mas.

La columna que importa es `FORZADA POR keep.lst`: una copia sobrante que ademas
esta nombrada en `keep.lst` **no se estripa**, o sea que son bytes de mas en el
DOL, no una cola inofensiva.

Aciertos de su primera pasada (r50):

    zAI / zPhysics / zPhysicsBehaviors   "GAMECUBE" x2 (objetivo x1)
        escriben a mano el prefijo de bWare pero no definen BWARE_PREFIX_GAMECUBE
        (ver bWare.hpp:164), asi que cc1plus interna una segunda copia y las dos
        estan en keep.lst.
    zSim   "FEngHUD" x2 (el de la r49) y ademas "SimModel" x2, que nadie habia visto.
    zMain  "SuspensionParams" ($LC523) y "RBComplexParams" ($LC521), las dos en keep.lst.

Ojo: la mayoria de las filas son `x1 (objetivo x0)` --cadenas que el objetivo no
tiene en esa unidad--. Esas estan MUERTAS y el enlazador se lleva `size & ~7`; no
son el problema salvo que keep.lst las nombre.

    python scripts/dupstr.py            # todas las SourceLists compiladas
    python scripts/dupstr.py zSim zAI   # solo esas
"""
import collections
import glob
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
KEEP = set(l.strip() for l in open('config/GOWE69/keep.lst', encoding='utf-8', errors='replace')
           if l.strip() and not l.startswith('#'))


def rodata(p):
    d = open(p, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, sn, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = [struct.unpack('>10I', d[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
    stro = S[sx][4]
    nom = lambda o: d[stro + o:d.index(b'\0', stro + o)].decode('latin1')
    idx = [i for i, s in enumerate(S) if nom(s[0]) == '.rodata']
    if not idx:
        return b'', {}
    i = idx[0]
    blob = d[S[i][4]:S[i][4] + S[i][5]]
    st = [s for s in S if s[1] == 2]
    sy = {}
    if st:
        st = st[0]
        strt = S[st[6]][4]
        for o in range(st[4], st[4] + st[5], 16):
            nm, val, sz, info, other, shn = struct.unpack('>IIIBBH', d[o:o + 16])
            if shn == i and sz:
                sy[val] = d[strt + nm:d.index(b'\0', strt + nm)].decode('latin1')
    return blob, sy


def cuenta(blob):
    """Cadenas ASCII >=3 terminadas en NUL que ARRANCAN detras de un NUL: sin ese
    filtro un puntero con bytes imprimibles se cuela como prefijo de la de al lado."""
    c = collections.Counter()
    i, n = 0, len(blob)
    while i < n:
        if 32 <= blob[i] < 127 and (i == 0 or blob[i - 1] == 0):
            j = i
            while j < n and 32 <= blob[j] < 127:
                j += 1
            if j < n and blob[j] == 0 and j - i >= 3:
                c[blob[i:j]] += 1
            i = j + 1
        else:
            i += 1
    return c


def main():
    uni = sys.argv[1:] or sorted(os.path.basename(x)[:-2] for x in
                                 glob.glob('build/GOWE69/src/Speed/Indep/SourceLists/*.o'))
    tot = 0
    for u in uni:
        o = 'build/GOWE69/obj/Speed/Indep/SourceLists/%s.o' % u
        n = 'build/GOWE69/src/Speed/Indep/SourceLists/%s.o' % u
        if not (os.path.exists(o) and os.path.exists(n)):
            continue
        W, _ = rodata(o)
        V, sy = rodata(n)
        if not V:
            continue
        cw, cv = cuenta(W), cuenta(V)
        filas = []
        for s, k in cv.items():
            if k <= cw.get(s, 0):
                continue
            pos, i = [], 0
            while True:
                i = V.find(s + b'\0', i)
                if i < 0:
                    break
                pos.append(i)
                i += 1
            nom = [sy.get(p, '-') for p in pos]
            forz = [x for x in nom if '%s.o:%s' % (u, x) in KEEP]
            filas.append(((len(s) + 1) * (k - cw.get(s, 0)), len(s) + 1, s, k, cw.get(s, 0), nom, forz))
        if not filas:
            continue
        b = sum(f[0] for f in filas)
        tot += b
        print('%-18s %3d cadenas de mas, %d B' % (u, len(filas), b))
        for _, sz, s, k, kw, nom, forz in sorted(filas, key=lambda x: -x[0])[:10]:
            print('    %3d B  x%d (objetivo x%d)  %-30r  %s%s'
                  % (sz, k, kw, s.decode('latin1')[:30], nom[:4],
                     '  FORZADA POR keep.lst: ' + ','.join(forz) if forz else ''))
    print('TOTAL %d B' % tot)
    return 0


if __name__ == '__main__':
    sys.exit(main())
