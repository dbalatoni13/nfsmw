#!/usr/bin/env python3
"""claimrange.py -- propone los rangos de `splits.txt` que cada unidad de biblioteca debe RECLAMAR.

Cada simbolo de datos que nuestro objeto emite y `splits.txt` no le declara se
busca por NOMBRE en `config/GOWE69/symbols.txt`, que da su direccion exacta. Se
agrupan por seccion en rangos contiguos y se comprueba que no pisen el rango de
ninguna otra unidad.

    python scripts/promodist.py --libs --listas | python scripts/claimrange.py

Salen tres clases de fila:
  LIBRE       el rango lo sirve hoy un comodin: se puede reclamar tal cual.
  CHOCA con X el simbolo lo emitimos DOS veces -- casi siempre una vtable que el
              original emite una sola vez. Ahi no hay rango que reclamar: sobra
              una de las dos emisiones.
  sin nombre  el simbolo no esta en symbols.txt (estaticos de fdlibm que repiten
              nombre entre ficheros): hay que localizarlo por contenido.
"""
import io
import os
import re
import struct
import sys

RE_RANGO = re.compile(
    r'\s+(\.\w+)\s+start:(0x[0-9A-Fa-f]+)\s+end:(0x[0-9A-Fa-f]+)(?:.*?rename:(\.\w+))?')

DATA = ('.rodata', '.data', '.sdata', '.sdata2', '.bss', '.sbss')

SYM = {}
for l in io.open('config/GOWE69/symbols.txt', encoding='utf-8'):
    m = re.match(r'(\S+)\s*=\s*(\.\w+):(0x[0-9A-Fa-f]+);.*?size:(0x[0-9A-Fa-f]+)', l)
    if m:
        SYM[m.group(1)] = (m.group(2), int(m.group(3), 16), int(m.group(4), 16))

RANG, cur = [], None
DECL = {}
for l in io.open('config/GOWE69/splits.txt', encoding='utf-8'):
    if not l.startswith('\t') and l.rstrip().endswith(':'):
        cur = re.sub(r'\.(c|cpp|s)$', '', l.rstrip().rstrip(':'))
        DECL.setdefault(cur, set())
    elif l.startswith('\t') and cur:
        m = RE_RANGO.match(l)
        if m:
            # `rename:` manda: `.over ... rename:.rodata` ES el rango de .rodata de
            # esa unidad. Sin mirarlo, las dos overlays (zFeOverlay, zOnline) salian
            # como si no tuvieran .rodata declarada y su dato parecia huerfano.
            sec = m.group(4) or m.group(1)
            RANG.append((sec, int(m.group(2), 16), int(m.group(3), 16), cur))
            DECL[cur].add(sec)


def obj_syms(p):
    f = open(p, 'rb').read()
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
    out = []
    for o in range(S[st][4], S[st][4] + S[st][5], 16):
        nameo, val, size, info, other, shndx = struct.unpack('>IIIBBH', f[o:o + 16])
        n = nm(nameo, strt)
        if n and shndx < len(names) and names[shndx] in DATA and size:
            out.append((n, names[shndx], size))
    return out


def duenyo(sec, a, b, yo):
    return [r for r in RANG if r[0] == sec and r[3] != yo and not (r[2] <= a or r[1] >= b)]


for u in [l.strip() for l in sys.stdin if l.strip()]:
    p = os.path.join('build', 'GOWE69', 'src', *u.split('/')) + '.o'
    if not os.path.exists(p):
        continue
    dec = DECL.get(u, set())
    porsec = {}
    huerfanos_sin_nombre = []
    for n, sec, size in obj_syms(p):
        if sec in dec or n.startswith(('$LC', 'gap_', 'pad_')):
            continue
        s = SYM.get(n)
        if not s:
            huerfanos_sin_nombre.append((sec, n, size))
            continue
        porsec.setdefault(s[0], []).append((s[1], s[1] + s[2], n))
    if not porsec and not huerfanos_sin_nombre:
        continue
    print('== %s' % u)
    for sec, L in sorted(porsec.items()):
        L.sort()
        grupos, ini, fin = [], L[0][0], L[0][1]
        for a, b, n in L[1:]:
            if a <= fin + 32:
                fin = max(fin, b)
            else:
                grupos.append((ini, fin))
                ini, fin = a, b
        grupos.append((ini, fin))
        for a, b in grupos:
            d = duenyo(sec, a, b, u)
            print('   %-9s start:0x%08X end:0x%08X  %4d B   %s' %
                  (sec, a, b, b - a, 'LIBRE' if not d else 'CHOCA con ' + ', '.join(x[3].split('/')[-1] for x in d)))
    if huerfanos_sin_nombre:
        print('   sin nombre en symbols.txt: %s' %
              ', '.join('%s/%s' % (s, n) for s, n, z in huerfanos_sin_nombre[:6]))
