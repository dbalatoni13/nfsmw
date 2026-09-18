#!/usr/bin/env python3
"""rangechk.py -- valida los limites de `splits.txt` contra los STT_FILE del ELF.

El ELF original (`orig/GOWE69/NFSMWRELEASE.ELF`) trae **577 entradas STT_FILE**, y
sus nombres son las propias SourceLists (`zGameplay.cpp`, `zLua.cpp`, ...) mas los
ficheros sueltos de las bibliotecas. En un symtab de ELF, cada STT_FILE va seguido
de los simbolos LOCALES de esa unidad de traduccion: o sea que el ELF dice, sin
adivinar nada, **de que unidad es cada estatico**.

`splits.txt` es la conjetura del proyecto sobre esas mismas fronteras. Esto las
enfrenta:

  * FUERA  -- un local del fichero X cae dentro del rango declarado de Y.
             Es un limite mal puesto, y arrastra todo el dato de esa seccion.
  * HUECO  -- entre el ultimo local de una unidad y el primero de la siguiente
             hay sitio de sobra: el limite puede estar en cualquier punto del
             hueco, y `splits.txt` puede haberlo puesto en el extremo que no es.

Los simbolos GLOBALES no llevan atribucion: en un symtab van todos DETRAS de los
locales, asi que el STT_FILE que les toca es el del ultimo fichero y miente. Se
ignoran a proposito -- pero son justo los que caen en los HUECOS, que es donde
esta el trabajo.

    python scripts/rangechk.py            # resumen: solo lo que no cuadra
    python scripts/rangechk.py --huecos   # ademas, los huecos > 4 B
    python scripts/rangechk.py zLua       # detalle de una unidad
"""
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ELF = os.path.join(ROOT, 'orig', 'GOWE69', 'NFSMWRELEASE.ELF')
SPLITS = os.path.join(ROOT, 'config', 'GOWE69', 'splits.txt')
SECS = ('.text', '.rodata', '.data', '.sdata', '.sdata2', '.bss', '.sbss', '.sbss2', '.ctors', '.dtors')


def locales_del_elf():
    """-> [(addr, size, nombre, fichero)] de los simbolos LOCALES con STT_FILE."""
    f = open(ELF, 'rb').read()
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
    stro = S[sx][4]

    def sname(o):
        return f[stro + o:f.index(b'\0', stro + o)].decode(errors='replace')

    sym = next((s for s in S if sname(s[0]) == '.symtab'), None)
    if sym is None:
        return []
    st = S[sym[6]][4]
    cur, out = None, []
    for i in range(sym[5] // 16):
        a, v, z, inf, _o, shn = struct.unpack('>IIIBBH', f[sym[4] + i * 16:sym[4] + i * 16 + 16])
        nm = f[st + a:f.index(b'\0', st + a)].decode(errors='replace')
        tipo, bind = inf & 0xF, inf >> 4
        if tipo == 4:                       # STT_FILE
            cur = nm
            continue
        if bind != 0 or not cur or not v or tipo == 3:   # solo LOCAL, no SECTION
            continue
        if nm:
            out.append((v, z, nm, cur))
    return out


def rangos():
    """-> {unidad: {seccion: (start, end)}} leyendo splits.txt."""
    d, cur = {}, None
    RE = re.compile(r'\s+(\.\w+)\s+start:(0x[0-9A-Fa-f]+)\s+end:(0x[0-9A-Fa-f]+)')
    for l in open(SPLITS, encoding='utf-8'):
        if not l.startswith('\t') and l.rstrip().endswith(':'):
            cur = l.rstrip().rstrip(':')
            continue
        m = RE.match(l)
        if m and cur:
            # dos lineas declaran la seccion con `rename:` (.over -> .rodata):
            # leer solo la izquierda deja huerfano el dato de las dos overlays.
            d.setdefault(cur, {})[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
    return d


def clave(f):
    """`zLua.cpp` -> nombre corto con el que comparar; ruta -> basename."""
    return os.path.basename(f.replace(chr(92), '/'))


def marcas_de_tu(L):
    """-> [(addr, fichero)] de cada `gcc2_compiled.`, que marca el inicio de .text
    de una unidad de traduccion compilada con GCC. Es la frontera EXACTA."""
    return sorted((v, f) for v, z, nm, f in L if nm == 'gcc2_compiled.')


def modo_text(R, L, clave, idx):
    """Rangos .text que se tragan la frontera de otra unidad de traduccion."""
    M = marcas_de_tu(L)
    print()
    print('== FRONTERAS .text: %d marcas `gcc2_compiled.` en el ELF' % len(M))
    print('   Cada una es el inicio EXACTO del .text de una TU del original.')
    print('   Un rango que contiene una marca que no es la suya se traga otra unidad.')
    filas = []
    for u, secs in R.items():
        if '.text' not in secs:
            continue
        a, b = secs['.text']
        dentro = [(v, f) for v, f in M if a < v < b and clave(f) != clave(u)]
        if dentro:
            filas.append((len(dentro), clave(u), a, b, dentro))
    filas.sort(reverse=True)
    print('   %d rangos .text contienen marcas ajenas' % len(filas))
    for n, u, a, b, dentro in filas[:25]:
        print('   %-26s 0x%08X-0x%08X  %d marca(s): %s'
              % (u[:26], a, b, n, ', '.join('%s@0x%08X' % (clave(f), v) for v, f in dentro[:3])))


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    R = rangos()
    # indice (start, end) -> unidad, por seccion
    idx = []
    for u, secs in R.items():
        for s, (a, b) in secs.items():
            idx.append((a, b, u, s))
    idx.sort()

    def duenyo(v):
        lo, hi = 0, len(idx)
        while lo < hi:
            m = (lo + hi) // 2
            if idx[m][0] <= v:
                lo = m + 1
            else:
                hi = m
        if lo and idx[lo - 1][0] <= v < idx[lo - 1][1]:
            return idx[lo - 1][2], idx[lo - 1][3]
        return None, None

    L = locales_del_elf()
    if '--text' in sys.argv:
        modo_text(R, L, clave, idx)
        return
    print('%d simbolos locales con STT_FILE; %d unidades en splits.txt' % (len(L), len(R)))
    fuera = {}
    prop = {}                                # (unidad_elf, seccion) -> [min, max]
    for v, z, nm, fich in L:
        u, s = duenyo(v)
        if u is None:
            continue
        k = clave(fich)
        if clave(u) != k:
            fuera.setdefault((k, clave(u), s), []).append((v, nm))
        prop.setdefault((k, s), [v, v + max(z, 1)])
        p = prop[(k, s)]
        p[0], p[1] = min(p[0], v), max(p[1], v + max(z, 1))

    if args:
        for a in args:
            print()
            print('== %s' % a)
            for (k, s), (lo, hi) in sorted(prop.items()):
                if a not in k:
                    continue
                dec = next((R[u][s] for u in R if clave(u) == k and s in R[u]), None)
                d = ('declarado 0x%08X-0x%08X' % dec) if dec else 'sin rango declarado'
                print('   %-8s locales 0x%08X-0x%08X   %s' % (s, lo, hi, d))
        return

    print()
    if fuera:
        print('== LIMITES MAL PUESTOS: un local de X cae en el rango de Y')
        for (kf, ku, s), v in sorted(fuera.items(), key=lambda x: -len(x[1])):
            print('   %-22s -> rango de %-22s %-8s %3d simbolos, 1o 0x%08X %s'
                  % (kf, ku, s, len(v), min(v)[0], min(v)[1][:28]))
    else:
        print('ningun local cae fuera de su rango')

    if '--huecos' in sys.argv:
        print()
        print('== HUECOS: entre el ultimo local de una unidad y el fin de su rango')
        filas = []
        for (k, s), (lo, hi) in prop.items():
            dec = next((R[u][s] for u in R if clave(u) == k and s in R[u]), None)
            if not dec:
                continue
            if dec[1] - hi > 4:
                filas.append((dec[1] - hi, k, s, hi, dec[1]))
        filas.sort(reverse=True)
        for d, k, s, hi, fin in filas[:25]:
            print('   %-22s %-8s ultimo local acaba 0x%08X, rango hasta 0x%08X  (%d B)'
                  % (k, s, hi, fin, d))


if __name__ == '__main__':
    main()
