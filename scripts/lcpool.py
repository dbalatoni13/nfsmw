#!/usr/bin/env python3
"""lcpool.py -- el pool `$LC` que emitimos y el objeto del original NO tiene.

Contado en la r31: **los objetos extraidos no tienen NI UN simbolo `$LC`**. Todo
su `.rodata` son `lbl_`, nombres y relleno. Los nuestros llevan ademas el pool de
literales que `cc1plus` emite por su cuenta, porque nuestro codigo escribe la
constante donde el original REFERENCIABA el simbolo del pool.

Y el `lbl_` escrito a mano ya casa byte a byte en todas: no sobra, sobra el pool.

    zDynamics   objetivo $LC 0 / lbl_ 436     nuestro $LC 286 / lbl_ 436   delta +296
    zMiscSmall  objetivo $LC 0 / lbl_ 352     nuestro $LC 583 / lbl_ 352   delta +648

La cura la dejo escrita `und` en la r31: declarar `extern const float
lbl_XXXXXXXX;` y usarlo en el codigo -- **con la definicion AL FINAL de la unidad
de traduccion**, porque con el inicializador visible en el punto de uso GCC
pliega la carga y rompe la funcion.

    python scripts/lcpool.py            # las 33
    python scripts/lcpool.py --todo     # tambien .data/.sdata2
"""
import glob
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SECS = ('.rodata', '.data', '.sdata', '.sdata2')


def rodata(p, secs):
    try:
        f = open(p, 'rb').read()
    except OSError:
        return None
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
    stro = S[sx][4]

    def nm(x, b):
        e = f.index(b'\0', b + x)
        return f[b + x:e].decode()

    names = [nm(s[0], stro) for s in S]
    st = [i for i, s in enumerate(S) if s[1] == 2][0]
    strt = S[S[st][6]][4]
    tam = sum(S[i][5] for i in range(len(S)) if names[i] in secs)
    lc = lbl = otro = 0
    for o in range(S[st][4], S[st][4] + S[st][5], 16):
        nameo, val, size, info, other, shndx = struct.unpack('>IIIBBH', f[o:o + 16])
        n = nm(nameo, strt)
        if not n or shndx >= len(names) or names[shndx] not in secs:
            continue
        if n.startswith('$LC') or (n.startswith('LC') and n[2:3].isdigit()):
            lc += size
        elif n.startswith('lbl_'):
            lbl += size
        elif not n.startswith(('gap_', 'pad_')):
            otro += size
    return tam, lc, lbl, otro


def main():
    secs = SECS if '--todo' in sys.argv else ('.rodata',)
    fuentes = sorted(glob.glob(os.path.join(ROOT, 'src', 'Speed', 'Indep', 'SourceLists', '*.cpp')))
    print('%-20s %8s %8s %8s %8s %8s' % ('unidad', 'delta', 'nuestro$LC', 'obj$LC', 'lbl_ nue', 'lbl_ obj'))
    T = [0, 0, 0]
    n_conlc = 0
    for src in fuentes:
        u = os.path.basename(src)[:-4]
        a = rodata(os.path.join(ROOT, 'build', 'GOWE69', 'obj', 'Speed', 'Indep', 'SourceLists', u + '.o'), secs)
        b = rodata(os.path.join(ROOT, 'build', 'GOWE69', 'src', 'Speed', 'Indep', 'SourceLists', u + '.o'), secs)
        if not a or not b:
            continue
        d = b[0] - a[0]
        T[0] += d
        T[1] += b[1]
        T[2] += a[1]
        if b[1]:
            n_conlc += 1
        print('%-20s %+8d %8d %8d %8d %8d' % (u, d, b[1], a[1], b[2], a[2]))
    print()
    print('TOTAL delta %+d B   nuestro $LC %d B   objetivo $LC %d B   (%d de 33 unidades con pool)'
          % (T[0], T[1], T[2], n_conlc))


if __name__ == '__main__':
    main()
