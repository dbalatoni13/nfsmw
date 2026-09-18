#!/usr/bin/env python3
"""vtdup.py -- vtables (y demas datos con nombre) que emitimos en VARIAS unidades.

El original emite cada vtable UNA sola vez, en el TU que define su *key method*
--la primera virtual no inline--. Si una clase no tiene key method (todas sus
virtuales puras o inline, que es el caso de las INTERFACES), GCC 2.9 la emite en
**cada** unidad que la usa, y el enlace se queda con todas: bytes de mas que
corren las direcciones y bloquean la promocion.

Caso testigo (r32): `EA::Allocator::IAllocator` declara `virtual ~IAllocator() {}`
inline, asi que no hay key method. El original la tiene una sola vez, en zBWare
(0x803D18F0); nosotros la emitimos tambien en `rcmp_vp6_codec`.

    python scripts/vtdup.py            # las que emitimos mas de una vez
    python scripts/vtdup.py --todas    # todas, con su duenyo segun splits.txt

La columna DUENYO sale de cruzar la direccion de `symbols.txt` con los rangos de
`splits.txt`: es la unidad que DEBE emitirla. Las demas sobran.
"""
import glob
import io
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SECS = ('.rodata', '.data', '.sdata', '.sdata2')
RE_RANGO = re.compile(
    r'\s+(\.\w+)\s+start:(0x[0-9A-Fa-f]+)\s+end:(0x[0-9A-Fa-f]+)(?:.*?rename:(\.\w+))?')


def syms(p):
    try:
        f = open(p, 'rb').read()
    except OSError:
        return []
    if f[:4] != b'\x7fELF':
        return []
    try:
        shoff = struct.unpack('>I', f[0x20:0x24])[0]
        se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
        S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
        stro = S[sx][4]
    except (struct.error, IndexError):
        return []                     # objeto a medio escribir por otro agente

    def nm(x, b):
        e = f.index(b'\0', b + x)
        return f[b + x:e].decode()

    names = [nm(s[0], stro) for s in S]
    st = [i for i, s in enumerate(S) if s[1] == 2]
    if not st:
        return []
    st = st[0]
    strt = S[S[st][6]][4]
    out = []
    try:
        rango = range(S[st][4], S[st][4] + S[st][5], 16)
    except (struct.error, IndexError):
        return []
    for o in rango:
        nameo, val, size, info, other, shndx = struct.unpack('>IIIBBH', f[o:o + 16])
        n = nm(nameo, strt)
        if n.startswith('_vt.') and shndx < len(names) and names[shndx] in SECS and size:
            out.append((n, size))
    return out


SYM = {}
for l in io.open(os.path.join(ROOT, 'config', 'GOWE69', 'symbols.txt'), encoding='utf-8'):
    m = re.match(r'(\S+)\s*=\s*(\.\w+):(0x[0-9A-Fa-f]+);', l)
    if m:
        SYM[m.group(1)] = (m.group(2), int(m.group(3), 16))

RANG, cur = [], None
for l in io.open(os.path.join(ROOT, 'config', 'GOWE69', 'splits.txt'), encoding='utf-8'):
    if not l.startswith('\t') and l.rstrip().endswith(':'):
        cur = re.sub(r'\.(c|cpp|s)$', '', l.rstrip().rstrip(':'))
    elif l.startswith('\t') and cur:
        m = RE_RANGO.match(l)
        if m:
            # `rename:` manda: ver la nota de claimrange.py.
            RANG.append((m.group(4) or m.group(1), int(m.group(2), 16), int(m.group(3), 16), cur))


def duenyo(sec, a):
    for s, lo, hi, u in RANG:
        if s == sec and lo <= a < hi:
            return u
    return None


def main():
    todas = '--todas' in sys.argv
    donde = {}
    base = os.path.join(ROOT, 'build', 'GOWE69', 'src')
    for p in glob.glob(os.path.join(base, '**', '*.o'), recursive=True):
        u = os.path.relpath(p, base).replace(os.sep, '/')[:-2]
        for n, size in syms(p):
            donde.setdefault(n, []).append((u, size))
    filas = []
    for n, L in donde.items():
        if len(L) < 2 and not todas:
            continue
        s = SYM.get(n)
        d = duenyo(s[0], s[1]) if s else None
        sobra = sum(z for _, z in L[1:]) if len(L) > 1 else 0
        filas.append((sobra, n, L, s, d))
    filas.sort(reverse=True)
    print('%-38s %5s %6s  %-26s %s' % ('vtable', 'B', 'veces', 'DUENYO (splits.txt)', 'unidades que la emiten'))
    tot = 0
    for sobra, n, L, s, d in filas:
        tot += sobra
        print('%-38s %5d %6d  %-26s %s' % (
            n[:38], L[0][1], len(L),
            (d.split('/')[-1] if d else ('comodin' if s else 'no esta en symbols.txt')),
            ', '.join(u.split('/')[-1] for u, _ in L)[:60]))
    print()
    print('%d vtables emitidas mas de una vez; %d B de mas' % (len(filas), tot))


if __name__ == '__main__':
    main()
