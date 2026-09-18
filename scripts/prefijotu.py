#!/usr/bin/env python3
"""prefijotu.py -- el vocabulario compartido que le falta a una unidad, EN ORDEN.

El deficit de `.rodata` del frente son **literales muertos**: cadenas que el
original emite y `cc1plus` no, porque en nuestro arbol nadie las referencia.
Medido sobre el ELF original (`docs/analisis/r51-jf-prefijo.md`): de los
109.310 B de `.rodata` de las 32 SourceLists, el 15,2 % sale en veinte o mas
unidades y con el tramo de 10-19 son **23.752 B de vocabulario compartido**,
contra los 22.230 B que la r50 conto como ausentes de nuestros objetos.

Lo que **no** hay es un solo bloque que valga para todas: de las 43 cadenas que
salen en 20+ unidades, 18 unidades llevan las 43 y entre ellas hay **14 ordenes
distintos**. Solo las **14 primeras** (213 B: 82 del prefijo de bWare/STL y 131
del bloque de AttribSys) coinciden en todas.

Asi que el orden hay que sacarlo del ELF unidad por unidad, y eso es lo que hace
esto: lee la `.rodata` del objetivo en el rango que le da `splits.txt`, quita lo
que ya emitimos, y escribe las que faltan **en el orden en que las tiene el
objetivo**, listas para un `asm()` de fichero.

    python scripts/prefijotu.py zAI              # informe
    python scripts/prefijotu.py zAI --asm        # el asm() para pegar
    python scripts/prefijotu.py --todas          # el censo de las 23

AVISOS, todos medidos y todos capaces de estropear la jugada:

  * **Anadir un literal desplaza los `$LC` posteriores** y deja rancias las
    entradas de `keep.lst`. Pasa `scripts/lcfix.py --check` DESPUES, siempre.
  * `zSim`, `zMisc` y `zEcstasy` tienen SUPERAVIT de `.rodata`: salvarles mas
    dato las ALEJA.
  * En `zLua` una de las cadenas mantiene viva una vtable y resucita 196 B de
    `.text`. Mira las tres secciones con `scripts/deadlink.py`, no solo
    `.rodata`.
  * Que una cadena falte no significa que ponerla acerque el DOL: el original la
    tiene DENTRO de un `lbl_` vivo, y nosotros la emitimos como su propio `$LC`,
    que el enlazador estripa en trozos de `size & ~7`.
  * **Esto lee el OBJETO, y lo que manda es el ENLACE.** Una unidad con andamio
    de pool escrito a mano emite a proposito bytes que el estripado se lleva: en
    el objeto sobran y en el ELF enlazado cuadran. Antes de actuar sobre una
    fila, confirmala enlazando --`scripts/linkdelta.py`, y comparando el
    CONTENIDO de las secciones, no sus tamanos--.
"""
import collections
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
ELF = 'orig/GOWE69/NFSMWRELEASE.ELF'


def _secciones(p):
    d = open(p, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    shent, shnum, shstr = struct.unpack('>HHH', d[0x2E:0x34])
    hs = []
    for i in range(shnum):
        o = shoff + i * shent
        nm, ty, fl, ad, off, sz = struct.unpack('>IIIIII', d[o:o + 24])
        hs.append({'type': ty, 'addr': ad, 'off': off, 'size': sz})
    return d, hs


def _leer(d, hs, va, n):
    for h in hs:
        if h['addr'] and h['addr'] <= va < h['addr'] + h['size'] and h['type'] != 8:
            o = h['off'] + (va - h['addr'])
            return d[o:o + n]
    return b''


def cadenas(b):
    """-> [texto] de las cadenas ASCII imprimibles terminadas en NUL, en orden."""
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
            out[os.path.basename(u).rsplit('.', 1)[0]] = (int(r.group(2), 16),
                                                          int(r.group(3), 16))
    return out


def nuestras(unidad):
    """Las cadenas que YA emitimos en la `.rodata` de nuestro objeto."""
    from extrasym import Elf
    p = os.path.join('build', 'GOWE69', 'src', 'Speed', 'Indep', 'SourceLists',
                     unidad) + '.o'
    if not os.path.exists(p):
        return None
    e = Elf(p)
    raw = open(p, 'rb').read()
    out = []
    for s in e.sh:
        if s['name'] in ('.rodata', '.over') and s['type'] != 8:
            out += cadenas(raw[s['off']:s['off'] + s['size']])
    return out


def faltan(unidad, d, hs, R):
    if unidad not in R:
        return None, None
    lo, hi = R[unidad]
    obj = cadenas(_leer(d, hs, lo, hi - lo))
    nues = nuestras(unidad)
    if nues is None:
        return obj, None
    tengo = collections.Counter(nues)
    # Un andamio de pool escrito a mano emite bytes SIN NUL, asi que se pega al
    # literal siguiente y el escaner ve una sola cadena: en zSim los 8 B de
    # `MNotifyS` de la r36d convierten `MNotifySimTick` en
    # `MNotifySMNotifySimTick`, y la cadena parecia ausente estando. Por eso la
    # segunda prueba es por SUBCADENA.
    pegadas = [x for x in nues if len(x) > 8]
    out, vis = [], set()
    for t in obj:
        if t in vis:
            continue
        vis.add(t)
        if tengo[t]:
            tengo[t] -= 1
        elif not any(t in x for x in pegadas):
            out.append(t)
    return obj, out


def esc(t):
    return t.replace('\\', '\\\\').replace('"', '\\"')


def main():
    a = [x for x in sys.argv[1:] if not x.startswith('--')]
    d, hs = _secciones(ELF)
    R = rangos()

    if '--todas' in sys.argv or not a:
        print('%-20s %8s %8s %9s' % ('unidad', 'objetivo', 'faltan', 'bytes'))
        tot = 0
        for u in sorted(R):
            obj, f = faltan(u, d, hs, R)
            if obj is None:
                continue
            if f is None:
                print('%-20s %8d %8s' % (u, len(obj), 'sin .o'))
                continue
            b = sum(len(x) + 1 for x in f)
            tot += b
            print('%-20s %8d %8d %9s' % (u, len(obj), len(f), '{:,}'.format(b)))
        print()
        print('TOTAL que falta: %s B' % '{:,}'.format(tot))
        print('Ojo: faltar no es lo mismo que pagar. Lee los avisos del docstring.')
        return 0

    for u in a:
        obj, f = faltan(u, d, hs, R)
        if obj is None:
            print('%s: sin rango de .rodata en splits.txt' % u)
            continue
        if f is None:
            print('%s: no hay objeto construido (corre build_direct.py primero)' % u)
            continue
        b = sum(len(x) + 1 for x in f)
        if '--asm' in sys.argv:
            print('// %d cadenas, %d B, en el orden del objetivo. Pegar en %s.cpp'
                  % (len(f), b, u))
            print('// Despues: python scripts/lcfix.py --check')
            print('asm(".section .rodata\\n"')
            for t in f:
                print('    "  .asciz \\"%s\\"\\n"' % esc(t))
            print('    ".previous\\n");')
        else:
            print('%s: el objetivo tiene %d cadenas, nos faltan %d (%s B)'
                  % (u, len(obj), len(f), '{:,}'.format(b)))
            for i, t in enumerate(f):
                print('   %3d  %-4d %r' % (i, len(t) + 1, t))
    return 0


if __name__ == '__main__':
    sys.exit(main())
