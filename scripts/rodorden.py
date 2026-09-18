#!/usr/bin/env python3
"""rodorden.py -- la PERMUTACION de la `.rodata` de una unidad, por cadenas.

`permorden.py` hace esto con las funciones y ordeno el frente de `linked`. Esto
es lo mismo para los datos, y hacia falta: cuando el deficit de `.rodata` deja
de ser contenido --cadenas que no emitimos-- y pasa a ser ORDEN, ninguna
herramienta del arbol lo lee. `dolrod.py` compara BYTES, y sobre una seccion
permutada saca cien lineas de trozos de cadena partidos.

Compara la SECUENCIA de cadenas del objetivo contra la nuestra con `difflib` y
cuenta cuantas estan en su sitio. La palanca para moverlas es el primer de pool
--una `static inline` muerta con `return "literal"`, que emite el literal en ese
punto del fichero por cero bytes-- y **solo mueve hacia ATRAS**, asi que las
filas `delete` (el objetivo la tiene antes y nosotros despues o no) son las
accionables.

    python scripts/rodorden.py zFe            # el detalle de una
    python scripts/rodorden.py                # la cola de las SourceLists
"""
import difflib
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
sys.path.insert(0, 'scripts')
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass
from extrasym import Elf

ELF = 'orig/GOWE69/NFSMWRELEASE.ELF'


def _hs():
    d = open(ELF, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    shent, shnum, _ = struct.unpack('>HHH', d[0x2E:0x34])
    return d, [struct.unpack('>IIIIII', d[shoff + i * shent:shoff + i * shent + 24])
               for i in range(shnum)]


def leer(d, hs, va, n):
    for nm, ty, fl, ad, off, sz in hs:
        if ad and ad <= va < ad + sz and ty != 8:
            return d[off + (va - ad):off + (va - ad) + n]
    return b''


def cadenas(b):
    out, cur = [], b''
    for c in b:
        if c == 0:
            if len(cur) >= 3 and all(32 <= x < 127 for x in cur):
                out.append(cur.decode('ascii'))
            cur = b''
        else:
            cur += bytes([c])
    return out


def rangos():
    s = open('config/GOWE69/splits.txt', encoding='utf-8', errors='replace').read()
    s = s.replace('\r\n', '\n')
    out = {}
    for m in re.finditer(r'^([^\s:][^\n:]*):\n((?:\t\.\w+[^\n]*\n)+)', s, re.M):
        u = m.group(1)
        if '/SourceLists/' not in u:
            continue
        r = re.search(r'\t\.(rodata|over)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)',
                      m.group(2))
        if r:
            out[os.path.basename(u).rsplit('.', 1)[0]] = (r.group(1), int(r.group(2), 16),
                                                          int(r.group(3), 16))
    return out


def nuestras(u, sec):
    p = os.path.join('build', 'GOWE69', 'src', 'Speed', 'Indep', 'SourceLists', u) + '.o'
    if not os.path.exists(p):
        return None
    e = Elf(p)
    raw = open(p, 'rb').read()
    out = []
    for s in e.sh:
        if s['name'] == '.' + sec and s['type'] != 8:
            out += cadenas(raw[s['off']:s['off'] + s['size']])
    return out


def compara(u, d, hs, R):
    sec, lo, hi = R[u]
    A = cadenas(leer(d, hs, lo, hi - lo))
    B = nuestras(u, sec)
    if B is None:
        return None
    sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
    ok = sum(n for _, _, n in sm.get_matching_blocks())
    return A, B, sm, ok


def main():
    d, hs = _hs()
    R = rangos()
    args = [a for a in sys.argv[1:] if not a.startswith('-')]

    if not args:
        filas = []
        for u in sorted(R):
            r = compara(u, d, hs, R)
            if r is None:
                continue
            A, B, sm, ok = r
            filas.append((len(A) - ok, u, ok, len(A), len(B)))
        filas.sort()
        print('LA COLA DE LA .rodata: cadenas del objetivo que NO estan en su sitio')
        print('%-20s %8s %8s %8s' % ('unidad', 'en sitio', 'objetivo', 'FUERA'))
        for fuera, u, ok, na, nb in filas:
            print('%-20s %8d %8d %8d' % (u, ok, na, fuera))
        return 0

    for u in args:
        if u not in R:
            print('%s: sin rango de .rodata en splits.txt' % u)
            continue
        r = compara(u, d, hs, R)
        if r is None:
            print('%s: sin objeto construido' % u)
            continue
        A, B, sm, ok = r
        print('%s: objetivo %d cadenas, nuestro %d, en secuencia %d (fuera %d)'
              % (u, len(A), len(B), ok, len(A) - ok))
        print()
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            if tag == 'equal':
                continue
            print('%-8s obj[%d:%d] %-44s | nue[%d:%d] %s'
                  % (tag, i1, i2, ' '.join(A[i1:i2])[:44], j1, j2,
                     ' '.join(B[j1:j2])[:44]))
    return 0


if __name__ == '__main__':
    sys.exit(main())
