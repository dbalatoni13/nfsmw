#!/usr/bin/env python3
"""gapchk.py -- huecos escritos a mano que `-strip-unused-data` se lleva.

Un bloque `asm()` de datos con directiva `.size` es un simbolo con tamano, y si
nadie lo referencia el enlazador se lo lleva ENTERO -- aunque mida 4 B, donde la
aritmetica `size & ~7` diria que no pierde nada. Cuando eso pasa, todo lo que va
detras en la seccion se desplaza y el DOL rompe SIN que `linkdelta` ni `measure`
se muevan: la unidad mide bien en las nueve secciones.

Medido en zBWare: cuatro `gap_*_data` sin entrada en `keep.lst` hacian caer
`bDefaultSeed` 4 B antes y con el toda la `.data` posterior de la imagen.

    python scripts/gapchk.py            # todas las SourceLists compiladas
    python scripts/gapchk.py zWorld     # una
"""
import glob
import io
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PREF = ('gap_', 'pad_', 'lbl_')


def referenciados(d, S, sn):
    """Indices de simbolo a los que apunta alguna reubicacion."""
    r = set()
    for s in S:
        if s[1] in (4, 9):                      # RELA / REL
            paso = 12 if s[1] == 4 else 8
            for o in range(s[4], s[4] + s[5], paso):
                info, = struct.unpack('>I', d[o + 4:o + 8])
                r.add(info >> 8)
    return r


def simbolos(p):
    d = open(p, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, n, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = [struct.unpack('>10I', d[shoff + i * se:shoff + i * se + 40]) for i in range(n)]
    stro = S[sx][4]
    sn = {i: d[stro + s[0]:d.index(b'\0', stro + s[0])].decode() for i, s in enumerate(S)}
    st = [s for s in S if s[1] == 2]
    if not st:
        return []
    st = st[0]
    strt = S[st[6]][4]
    out = []
    for q in range(st[4], st[4] + st[5], 16):
        nm, val, sz, info, other, shn = struct.unpack('>IIIBBH', d[q:q + 16])
        name = d[strt + nm:d.index(b'\0', strt + nm)].decode('latin1')
        if name.startswith(PREF) and sz:
            out.append((name, sn.get(shn, '?'), val, sz))
    return out


def main():
    os.chdir(ROOT)
    keep = set(l.strip() for l in io.open('config/GOWE69/keep.lst', encoding='utf-8'))
    pedidos = sys.argv[1:]
    objs = sorted(glob.glob('build/GOWE69/src/Speed/Indep/SourceLists/*.o'))
    total = 0
    for p in objs:
        u = os.path.basename(p)[:-2]
        if pedidos and u not in pedidos:
            continue
        falta = [(n, s, v, z) for n, s, v, z in simbolos(p)
                 if ('%s.o:%s' % (u, n)) not in keep]
        if not falta:
            continue
        print('%-14s %3d muerto(s) sin keep.lst, %5d B en juego' % (u, len(falta), sum(z for _,_,_,z in falta)))
        if pedidos:
            for n, s, v, z in sorted(falta, key=lambda x: (x[1], x[2])):
                print('    %-34s %-9s +%04X %5d B' % (n, s, v, z))
        total += len(falta)
    print('total %d' % total)


main()
